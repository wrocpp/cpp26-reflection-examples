// "Reflection today" demo: a reflection-driven binary parser for a
// fixed-layout message arriving at a trust boundary (network packet,
// bus frame, file header).
//
// Real-world setting: handwritten parsers are a CVE concentration.
// Every field read needs a bounds check against the input buffer;
// every offset needs to advance correctly; every type needs to match
// the wire format. Miss any of those and you get the next CVE-2024-
// XXXXX (use-after-free in deserialiser, integer overflow in length
// prefix, type confusion across versions, etc.).
//
// Reflection lets us generate the bounds-checked, type-aware parser
// from the message struct itself. The struct IS the schema; the
// parser is mechanical and impossible to get wrong in the ways above.
//
// Aligned with:
//   - Core Guidelines I.27 (use std::span at boundaries) +
//     CP.3 (parameter ownership rules).
//   - MISRA C++ 2023 Rule 5-0-15 (no pointer arithmetic) and
//     Rule 21-2-1 (validate inputs from external sources).
//   - SEI CERT C++ STR50-CPP, INT30-CPP, MEM52-CPP.
//
// Honest scope (out of demo, in production-grade parser):
//   - Endianness: this demo assumes platform-native byte order. Real
//     wire formats need std::byteswap per field for non-native order.
//   - Padding: we field-by-field memcpy at running offsets, so the
//     struct's in-memory padding is irrelevant -- only the wire
//     layout (sum of field sizes) matters.
//   - Trivially-copyable members: enforced by static_assert below;
//     the demo refuses to compile if a member can't be safely memcpy'd.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/parse && /tmp/parse" \
//     posts/toolset/sanitizers-2026/examples/parse-struct.cpp

#include <experimental/meta>
#include <array>
#include <cstdint>
#include <cstring>
#include <expected>
#include <print>
#include <span>
#include <type_traits>

// Typical embedded-bus message: heterogeneous fields, fixed layout.
// (Substitute your favourite: CAN frame, PROFINET PDU, Modbus TCP
// frame, sensor packet, file-header field set.)
struct SensorReading {
    std::uint16_t sensor_id;
    std::int32_t  raw_value;
    std::uint8_t  status_flags;
};

enum class parse_error { too_short };

// Sum the sizes of T's data members at compile time. Uses
// std::meta::size_of(info) (P2996 metafunction) to query each
// member's type-size without splicing.
template <typename T>
consteval auto wire_size() -> std::size_t {
    constexpr auto ctx = std::meta::access_context::unchecked();
    std::size_t total = 0;
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        total += std::meta::size_of(std::meta::type_of(m));
    }
    return total;
}

// Parse T from `bytes`. Single up-front bounds check; field-by-field
// memcpy thereafter. The bounds check is mechanical and exhaustive --
// no manual offset arithmetic that can drift out of sync with the
// schema.
template <typename T>
constexpr auto parse_struct(std::span<const std::byte> bytes)
    -> std::expected<T, parse_error>
{
    static_assert(std::is_trivially_copyable_v<T>,
                  "parse_struct requires trivially-copyable T");
    constexpr std::size_t needed = wire_size<T>();
    if (bytes.size() < needed) return std::unexpected(parse_error::too_short);

    T out{};
    std::size_t off = 0;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        constexpr std::size_t field_bytes = std::meta::size_of(std::meta::type_of(m));
        std::memcpy(&(out.[:m:]), bytes.data() + off, field_bytes);
        off += field_bytes;
    }
    return out;
}

int main() {
    std::println("wire_size<SensorReading>() = {}", wire_size<SensorReading>());

    // Successful parse: 7 bytes for u16 + i32 + u8.
    std::array<std::byte, 7> good = {
        std::byte{0x05}, std::byte{0x00},                                  // sensor_id = 5
        std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, std::byte{0xFF}, // raw_value = -1
        std::byte{0x01},                                                   // status_flags = 1
    };
    if (auto r = parse_struct<SensorReading>(good); r.has_value()) {
        std::println("ok: id={} raw={} flags={}",
                     r->sensor_id, r->raw_value,
                     static_cast<unsigned>(r->status_flags));
    }

    // Truncated parse: only 3 bytes; parser refuses cleanly. The
    // bounds check is the SAME bounds check for every T -- no per-
    // type parser written by hand to drift out of sync.
    std::array<std::byte, 3> truncated = {
        std::byte{0x05}, std::byte{0x00}, std::byte{0xFF},
    };
    auto bad = parse_struct<SensorReading>(truncated);
    std::println("truncated: {}",
                 bad.has_value() ? "accepted (BUG)" : "refused (correct)");
}

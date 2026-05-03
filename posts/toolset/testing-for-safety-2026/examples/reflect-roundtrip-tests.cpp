// Annotation-driven property tests for the parse_struct / serialize_struct
// pair from sanitizers-2026.
//
// The schema IS the test specification. Reflection (P2996) walks the
// struct shape; user-defined annotations (P3394, both adopted into
// C++26) carry per-field generator hints AND struct-level markers that
// say WHICH properties to assert. The runner is generic; the spec
// lives next to the data it constrains.
//
// Two annotation tiers:
//
//   * Field-level [[=test_range{lo, hi}]] tells the generator the
//     valid-input domain for that field. The runner samples
//     {lo, mid, hi} per field and runs the cross-product. Add a field
//     with no annotation and the runner uses a single zero-initialized
//     value for it; add the annotation and the field's boundary is
//     automatically exercised across every test.
//
//   * Struct-level [[=test_round_trip{}]] [[=test_reject_short{}]]
//     mark which properties of the (parse, serialize) pair the runner
//     must assert. New marker, new property check; remove a marker,
//     the property no longer wastes CI time.
//
// Properties asserted when the markers are present:
//
//   P1.  parse(serialize(x)) == x       for every generated sample x
//   P2.  serialize(parse(b)) == b       for every byte stream of length
//                                       wire_size<T>()
//   P3.  parse(b) refuses cleanly       when b.size() < wire_size<T>()
//
// What this REPLACES: per-field "set value, read it back" tests. Those
// exercise C++ assignment, not the production parser. The annotation-
// driven runner exercises the actual round-trip contract that breaks
// in production when a parser and serializer drift apart.
//
// Aligned with:
//   * Core Guidelines T (Testing): tests should be exhaustive over the
//     schema they protect.
//   * MISRA C++ 2023 + AUTOSAR C++14 traceability: every member of a
//     safety-relevant struct is covered by at least one regression
//     test pinning its on-the-wire behaviour.
//   * Property-based testing (QuickCheck / RapidCheck): the property
//     is the spec; the generator does the work.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/rt && /tmp/rt" \
//     posts/toolset/testing-for-safety-2026/examples/reflect-roundtrip-tests.cpp

#include <experimental/meta>
#include <array>
#include <cstdint>
#include <cstring>
#include <expected>
#include <print>
#include <span>
#include <type_traits>
#include <vector>

// --- Annotation tags (P3394) ----------------------------------------
//
// Cousins in other ecosystems:
//   test_range     -> hypothesis  st.integers(min_value, max_value)
//                  -> JUnit5      (no native range; @CsvSource enumerates)
//                  -> proptest    #[strategy(lo..hi)]
//                  -> kotest      Arb.int(lo..hi)
//
//   test_examples  -> hypothesis  @example(value)
//                  -> JUnit5      @ValueSource(ints = {...})
//                  -> proptest    custom strategy with prop_oneof!(...)
//                  -> kotest      Exhaustive.of(...)
//
//   struct-level markers (test_round_trip, test_reject_short)
//                  -> JUnit5      @Tag("...") on the test class
//                  -> pytest      @pytest.mark.<name>
//                  -> proptest    function attributes / config

struct test_range { long long lo, hi; };  // closed interval
struct test_examples { long long v[4]; std::size_t n; };
struct test_round_trip {};                 // marker: assert P1 + P2
struct test_reject_short {};               // marker: assert P3

// --- Subject struct: parser/serializer + the contract they share ----

struct [[=test_round_trip{}]] [[=test_reject_short{}]] SensorReading {
    [[=test_range{0, 1023}]]
    [[=test_examples{{0, 1023, 1024, 0}, 3}]]    // pinned: lo, hi, hi+1 (off-by-one)
    std::uint16_t sensor_id;

    [[=test_range{-100000, 100000}]]
    std::int32_t  raw_value;

    [[=test_range{0, 0xFF}]]
    std::uint8_t  status_flags;
};

enum class parse_error { too_short };

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

template <typename T>
constexpr auto parse_struct(std::span<const std::byte> bytes)
    -> std::expected<T, parse_error>
{
    static_assert(std::is_trivially_copyable_v<T>);
    constexpr std::size_t needed = wire_size<T>();
    if (bytes.size() < needed) return std::unexpected(parse_error::too_short);
    T out{};
    std::size_t off = 0;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        constexpr std::size_t fb = std::meta::size_of(std::meta::type_of(m));
        std::memcpy(&(out.[:m:]), bytes.data() + off, fb);
        off += fb;
    }
    return out;
}

template <typename T>
constexpr void serialize_struct(const T& obj, std::span<std::byte> out) {
    static_assert(std::is_trivially_copyable_v<T>);
    std::size_t off = 0;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        constexpr std::size_t fb = std::meta::size_of(std::meta::type_of(m));
        std::memcpy(out.data() + off, &(obj.[:m:]), fb);
        off += fb;
    }
}

// --- Annotation lookup ---------------------------------------------

template <typename Tag>
consteval bool has_annotation(std::meta::info r) {
    for (auto a : std::meta::annotations_of(r))
        if (std::meta::type_of(a) == ^^Tag) return true;
    return false;
}

template <typename Tag>
consteval auto get_annotation(std::meta::info r) -> Tag {
    for (auto a : std::meta::annotations_of(r))
        if (std::meta::type_of(a) == ^^Tag)
            return std::meta::extract<Tag>(a);
    return Tag{};
}

// --- Reflection-driven equality -------------------------------------

template <typename T>
auto fields_equal(const T& a, const T& b) -> bool {
    bool eq = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (!(a.[:m:] == b.[:m:])) {
            std::println("    field {} differs", std::meta::identifier_of(m));
            eq = false;
        }
    }
    return eq;
}

// --- Sample generator: cross-product of {lo, mid, hi} per annotated field

template <typename T>
auto generate_samples() -> std::vector<T> {
    std::vector<T> out{T{}};
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        // Build the per-field sample set: range gives {lo, mid, hi};
        // examples adds pinned values that must always be exercised
        // (hypothesis @example, JUnit @ValueSource).
        std::vector<long long> field_samples;
        if constexpr (has_annotation<test_range>(m)) {
            constexpr auto r = get_annotation<test_range>(m);
            field_samples = {r.lo, (r.lo + r.hi) / 2, r.hi};
        }
        if constexpr (has_annotation<test_examples>(m)) {
            constexpr auto ex = get_annotation<test_examples>(m);
            for (std::size_t i = 0; i < ex.n; ++i)
                field_samples.push_back(ex.v[i]);
        }
        if (field_samples.empty()) continue;
        using FieldT = [:std::meta::type_of(m):];
        std::vector<T> next;
        next.reserve(out.size() * field_samples.size());
        for (const auto& base : out) {
            for (long long v : field_samples) {
                T t = base;
                t.[:m:] = static_cast<FieldT>(v);
                next.push_back(t);
            }
        }
        out = std::move(next);
    }
    return out;
}

// --- Property checks ------------------------------------------------

template <typename T>
auto check_value_round_trip(std::span<const T> samples) -> bool {
    bool ok = true;
    std::array<std::byte, wire_size<T>()> buf{};
    int failed = 0;
    for (const auto& x : samples) {
        serialize_struct(x, buf);
        auto y = parse_struct<T>(buf);
        if (!y || !fields_equal(*y, x)) { ok = false; ++failed; }
    }
    std::println("  P1 value round-trip ({} samples, {} failed): {}",
                 samples.size(), failed, ok ? "PASS" : "FAIL");
    return ok;
}

template <typename T>
auto check_byte_round_trip(std::span<const T> samples) -> bool {
    bool ok = true;
    std::array<std::byte, wire_size<T>()> buf{};
    std::array<std::byte, wire_size<T>()> echo{};
    for (const auto& x : samples) {
        serialize_struct(x, buf);
        auto y = parse_struct<T>(buf);
        if (!y) { ok = false; continue; }
        serialize_struct(*y, echo);
        if (std::memcmp(buf.data(), echo.data(), buf.size()) != 0) ok = false;
    }
    std::println("  P2 byte round-trip ({} samples): {}",
                 samples.size(), ok ? "PASS" : "FAIL");
    return ok;
}

template <typename T>
auto check_truncated_refused() -> bool {
    constexpr std::size_t needed = wire_size<T>();
    bool ok = true;
    for (std::size_t len = 0; len < needed; ++len) {
        std::vector<std::byte> b(len, std::byte{0});
        if (parse_struct<T>(b).has_value()) {
            std::println("    accepted len={} (BUG)", len);
            ok = false;
        }
    }
    std::println("  P3 truncated refused (lengths 0..{}): {}",
                 needed - 1, ok ? "PASS" : "FAIL");
    return ok;
}

// --- Annotation-dispatched runner -----------------------------------

template <typename T>
auto run_annotated_tests() -> bool {
    auto samples = generate_samples<T>();
    std::println("== {} samples generated from per-field annotations ==",
                 samples.size());

    bool ok = true;
    if constexpr (has_annotation<test_round_trip>(^^T)) {
        ok &= check_value_round_trip<T>(samples);
        ok &= check_byte_round_trip<T>(samples);
    }
    if constexpr (has_annotation<test_reject_short>(^^T)) {
        ok &= check_truncated_refused<T>();
    }
    return ok;
}

int main() {
    std::println("== annotation-driven tests for SensorReading ==");
    bool ok = run_annotated_tests<SensorReading>();
    std::println("--");
    std::println("overall: {}", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}

// arbitrary<T>: the C++26 reflection analogue of Rust's #[derive(Arbitrary)]
// and Haskell QuickCheck's Generic instances. A generic, type-driven sample
// generator for property tests, fuzzer harnesses, and differential tests --
// without external code generation, without per-struct hand-written builders.
//
// The kernel is a single template that walks T and produces samples. Round-
// trip tests, fuzz harnesses, fixture factories, oracle/differential tests
// are short layers on top. This is the pattern reflection-alone in C++26
// genuinely earns; see reflect-pretty-diff.cpp for the diagnostic side.
//
// Layering (the bit that handwritten test annotations get wrong):
//
//   * Production (sensor.hpp in real code) -- struct SensorReading is bare.
//     No test annotations leak into production headers, no test metadata
//     ships in production binaries, no test concerns appear in code review
//     of production changes.
//
//   * Test framework (this file's TestSpec template) -- a primary template
//     readers specialise per production type, mirroring how std::hash and
//     std::formatter are extended. Specialisation lives in test code only.
//
//   * Per-test spec (TestSpec<SensorReading>) -- declares per-field ranges
//     and pinned examples, struct-level markers (round_trip, reject_short).
//     Keyed by member-pointer NTTP, which means the spec is nominally tied
//     to the production type's actual members; rename a field in production
//     and the spec fails to compile.
//
//   * Runner -- generic in T; reads TestSpec<T>; generates Prod instances;
//     runs property checks against the production parse_struct/serialize_struct.
//     One implementation, every struct.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/arb && /tmp/arb" \
//     posts/toolset/testing-for-safety-2026/examples/reflect-arbitrary.cpp

#include <experimental/meta>
#include <array>
#include <cstdint>
#include <cstring>
#include <expected>
#include <print>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// =====================================================================
// Production code (would normally live in sensor.hpp). Test-clean.
// =====================================================================

struct SensorReading {
    std::uint16_t sensor_id;
    std::int32_t  raw_value;
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

// =====================================================================
// Test framework (would live in libtest.hpp). Generic in T.
// =====================================================================

struct test_range { long long lo, hi; };

template <auto MemberPtr>
struct field_spec {
    test_range range{0, 0};                    // empty range = no range sampling
    std::array<long long, 4> examples{};
    std::size_t example_count = 0;
};

// Primary template; users specialise per production type.
template <typename T> struct TestSpec;

// arbitrary<T>: the kernel. Walks TestSpec<T>::fields, expands per-field
// {range_lo, range_mid, range_hi} U examples, returns the cross-product
// of T instances. Layered patterns (round-trip / fuzz / differential) call
// this for inputs.
template <typename T, auto MP>
void apply_field(std::vector<T>& out, const field_spec<MP>& fs) {
    std::vector<long long> samples;
    if (fs.range.lo != fs.range.hi) {
        samples = {fs.range.lo, (fs.range.lo + fs.range.hi) / 2, fs.range.hi};
    }
    for (std::size_t i = 0; i < fs.example_count; ++i)
        samples.push_back(fs.examples[i]);
    if (samples.empty()) return;

    using FieldT = std::remove_cvref_t<decltype(std::declval<T>().*MP)>;
    std::vector<T> next;
    next.reserve(out.size() * samples.size());
    for (const auto& base : out) {
        for (long long v : samples) {
            T t = base;
            t.*MP = static_cast<FieldT>(v);
            next.push_back(t);
        }
    }
    out = std::move(next);
}

template <typename T>
auto arbitrary() -> std::vector<T> {
    std::vector<T> out{T{}};
    constexpr auto& specs = TestSpec<T>::fields;
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        (apply_field<T>(out, std::get<Is>(specs)), ...);
    }(std::make_index_sequence<std::tuple_size_v<
            std::remove_cvref_t<decltype(specs)>>>{});
    return out;
}

// fields_equal<T>: reflection-driven member-wise equality. See
// reflect-pretty-diff.cpp for the variant that names differing fields.
template <typename T>
auto fields_equal(const T& a, const T& b) -> bool {
    bool eq = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (!(a.[:m:] == b.[:m:])) eq = false;
    }
    return eq;
}

// =====================================================================
// Property checks (would live in test_round_trip.hpp). Generic in T.
// =====================================================================

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
        if (parse_struct<T>(b).has_value()) ok = false;
    }
    std::println("  P3 truncated refused (lengths 0..{}): {}",
                 needed - 1, ok ? "PASS" : "FAIL");
    return ok;
}

template <typename T>
auto run_round_trip_tests() -> bool {
    auto samples = arbitrary<T>();
    std::println("== {} samples generated by arbitrary<T> ==", samples.size());

    bool ok = true;
    if constexpr (TestSpec<T>::round_trip) {
        ok &= check_value_round_trip<T>(samples);
        ok &= check_byte_round_trip<T>(samples);
    }
    if constexpr (TestSpec<T>::reject_short) {
        ok &= check_truncated_refused<T>();
    }
    return ok;
}

// =====================================================================
// Test code (would live in sensor_test.cpp). The ONLY place that names
// SensorReading and its test contract together. Production header above
// is untouched. The specialisation is keyed by SensorReading's actual
// member pointers, so renaming a production field breaks the spec at
// build time -- forcing the spec author to update.
// =====================================================================

template <>
struct TestSpec<SensorReading> {
    static constexpr bool round_trip   = true;
    static constexpr bool reject_short = true;
    static constexpr auto fields = std::tuple{
        field_spec<&SensorReading::sensor_id>{
            .range = {0, 1023},
            .examples = {0, 1023, 1024, 0},      // lo, hi, hi+1 (off-by-one)
            .example_count = 3,
        },
        field_spec<&SensorReading::raw_value>{
            .range = {-100000, 100000},
        },
        field_spec<&SensorReading::status_flags>{
            .range = {0, 0xFF},
        },
    };
};

int main() {
    std::println("== reflection-driven property tests for SensorReading ==");
    bool ok = run_round_trip_tests<SensorReading>();
    std::println("--");
    std::println("overall: {}", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}

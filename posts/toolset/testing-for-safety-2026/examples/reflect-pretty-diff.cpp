// pretty_diff<T>: structural test-failure diagnostics via reflection.
//
// The other half of the test loop. arbitrary<T> (see reflect-arbitrary.cpp)
// is the cheap-inputs side; pretty_diff is the legible-outputs side. When
// an assertion fails, the difference between "T != T" and "field raw_value:
// 12345 != 12344" is the difference between "attach a debugger" and "the
// bug is on this line".
//
// Without reflection: every codebase grows a hand-written operator<< per
// type that someone debugged once. The set is partial, never current with
// the schema, and the type you actually need to dump in CI today is the
// one that doesn't have an operator<<. With reflection: one library, every
// struct, never stale -- because it walks T's members at compile time, not
// from a hand-maintained list.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/diff && /tmp/diff" \
//     posts/toolset/testing-for-safety-2026/examples/reflect-pretty-diff.cpp

#include <experimental/meta>
#include <cstdint>
#include <format>
#include <print>
#include <string>

// Production code, untouched.
struct SensorReading {
    std::uint16_t sensor_id;
    std::int32_t  raw_value;
    std::uint8_t  status_flags;
};

// pretty_print: dump every field by name. ~10 lines.
template <typename T>
auto pretty_print(const T& obj) -> std::string {
    std::string out;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        std::format_to(std::back_inserter(out), "  {} = {}\n",
                       std::meta::identifier_of(m),
                       +obj.[:m:]);     // unary + promotes uint8_t to int
    }                                    // so it formats as a number
    return out;
}

// pretty_diff: emit only the differing fields. ~12 lines.
template <typename T>
auto pretty_diff(const T& expected, const T& actual) -> std::string {
    std::string out;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (expected.[:m:] != actual.[:m:]) {
            std::format_to(std::back_inserter(out), "  {}: {} != {}\n",
                           std::meta::identifier_of(m),
                           +expected.[:m:], +actual.[:m:]);
        }
    }
    return out.empty() ? "  (no differences)\n" : out;
}

int main() {
    SensorReading expected{42, 12345, 0x0F};
    SensorReading actual  {42, 12344, 0x0F};   // raw_value off by one

    // Without reflection (the typical C++ test failure):
    std::println("== without reflection ==");
    std::println("FAIL: SensorReading != SensorReading\n");

    // With reflection -- the failure message names the bug:
    std::println("== with reflection ==");
    std::println("FAIL: SensorReading mismatch:\n{}", pretty_diff(expected, actual));

    // pretty_print works on any struct, no per-type operator<< needed:
    std::println("== full dump (any struct, no per-type code) ==");
    std::println("expected:\n{}", pretty_print(expected));
    std::println("actual:\n{}",   pretty_print(actual));

    // Add a field to SensorReading -- both helpers cover it on the next
    // build. Remove a field -- both helpers stop printing it. The
    // diagnostic surface tracks the schema mechanically.
}

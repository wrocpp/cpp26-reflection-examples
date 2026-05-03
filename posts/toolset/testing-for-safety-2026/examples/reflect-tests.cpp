// Reflection-driven test case generation.
//
// Real-world setting: as a struct grows new fields, the unit test
// suite drifts. Hand-written tests for "field A round-trips correctly"
// don't get extended when field B is added; the new field is implicitly
// tested via "the test compiled" rather than via "the test exercised
// the field".
//
// Reflection lets the test be GENERATED from the struct shape: one
// round-trip test per field, automatically. Add a field, the test
// suite grows; remove one, it shrinks. The schema is the test
// specification.
//
// Pattern works on top of any test framework -- this self-contained
// demo runs as a plain main() so it compiles on godbolt; in
// production the per-member loop becomes a Catch2 SECTION, a
// GoogleTest TEST_P, or a doctest SUBCASE.
//
// Aligned with C++ Core Guidelines T (Testing) intent: "tests should
// be exhaustive over the schema they protect". Maps cleanly to MISRA
// + AUTOSAR's traceability requirement: every member of a safety-
// relevant struct has at least one regression test pinning its
// behaviour.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/tests && /tmp/tests" \
//     posts/toolset/testing-for-safety-2026/examples/reflect-tests.cpp

#include <experimental/meta>
#include <cstdint>
#include <print>
#include <string_view>
#include <type_traits>

// Same SensorReading we parse in sanitizers-2026.
struct SensorReading {
    std::uint16_t sensor_id;
    std::int32_t  raw_value;
    std::uint8_t  status_flags;
};

// Pick a "distinct, non-default" value per type. Tests would fail
// silently if we used 0 here -- a default-constructed struct already
// has 0 in every integer field, so assignment is a no-op.
template <typename T>
constexpr T distinct_value() {
    if constexpr (std::is_integral_v<T>) return T{42};
    else if constexpr (std::is_floating_point_v<T>) return T{3.14};
    else return T{};
}

// For each member of T, generate + run one assignment / read-back
// test. Returns true if every per-member test passed. Prints the
// individual results so the output reads like a test runner.
template <typename T>
auto run_field_round_trip_tests() -> bool {
    bool all_passed = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        using FieldT = [:std::meta::type_of(m):];
        T obj{};
        FieldT new_value = distinct_value<FieldT>();
        obj.[:m:] = new_value;
        bool passed = (obj.[:m:] == new_value);
        if (!passed) all_passed = false;
        std::println("  field {:14}: {}",
                     std::meta::identifier_of(m),
                     passed ? "PASS" : "FAIL");
    }
    return all_passed;
}

int main() {
    std::println("== generated tests for SensorReading ==");
    bool ok = run_field_round_trip_tests<SensorReading>();
    std::println("--");
    std::println("overall: {}", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}

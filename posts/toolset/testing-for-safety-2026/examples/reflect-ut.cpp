// reflect+UT: ONE thing reflection gives you that the other frameworks
// cannot match without codegen or preprocessor hacks: dynamic per-field
// test registration named by the reflected member.
//
// The framework-agnostic case (a single TEST_CASE / TEST / _test that
// loops over members and asserts inside) works identically in Catch2,
// doctest, GoogleTest, or UT -- there is no UT-specific contribution
// there. This file is NOT that pattern.
//
// What macro-based frameworks cannot easily do:
//   - Emit ONE named test per reflected member, so the test report
//     reads "field name PASS", "field email PASS", "field age FAIL"
//     instead of "round-trip 1/3 sub-assertions failed".
// You can fake this in Catch2 v3 with `SECTION`, in GTest with
// `RegisterTest()`, or in doctest with `SUBCASE` -- but each requires
// being INSIDE an existing TEST_CASE, so the suite-level grouping
// disappears. UT's `ut::test(runtime_string) = lambda` is the only
// idiomatic dynamic-registration surface in mainstream C++ testing
// today, and it composes cleanly with reflection.
//
// The deliverable below is a single ~20-line generic utility:
//
//     register_per_field_tests<T>("suite-name", make_field_test);
//
// pass any T and a function that takes `auto Pmd` (a non-type template
// parameter holding the pointer-to-data-member), get one named UT test
// per nonstatic data member of T. The test name is the field name
// reflected at compile time and spliced into the runtime test name.
// Reusable across any struct in any project that already uses UT.
//
// When is this worth using?
//   YES: per-field round-trip tests, per-field invariant checks, per-
//        field property tests where each test's failure should name
//        the field that broke. Test report becomes a per-field
//        coverage matrix.
//   NO:  one assertion that examines several fields together (just
//        loop inside a single `_test`). Mock-style behaviour tests
//        (still GoogleMock / Trompeloeil). Anything where the test
//        body differs per field beyond the splice.
//
// godbolt (single-file, URL-include for ut.hpp): https://godbolt.org/z/a3Tqb7KM6
// docker (cpp-reflection container, fetches ut.hpp at runtime):
//   see "Reproduce locally" block on the testing-for-safety-2026 page.

#include <experimental/meta>
// Godbolt's UI pipeline supports `#include <https://...>` URL fetches.
// Lets the example open + run on Compiler Explorer with no tree-mode
// or library-bundling setup: the UI fetches ut.hpp on demand.
// (When building locally inside cpp-reflection container, the fetch
// goes through Python instead -- see the "Reproduce locally" block.)
#include <https://raw.githubusercontent.com/boost-ext/ut/refs/heads/master/include/boost/ut.hpp>
#include <string>

using namespace boost::ut;

// --- the reusable utility, ~20 lines ---

// Per-field test body, parameterised on a pointer-to-data-member NTTP.
// Hoisted into a function template because variables introduced by
// `template for` cannot be captured by a regular lambda. The user
// supplies this; the utility wires it into UT's runtime registration.
template <auto Pmd>
using field_test_body_t = void (*)();

// Walk T's nonstatic data members at compile time; for each, register
// a UT test named "<prefix>.<field-name>" whose body is the result of
// calling MakeBody.template operator()<T, &[:m:]>() (so the user's
// body gets both the owning type and the pointer-to-data-member as
// template parameters).
//
// Usage: see main() below.
template <typename T, typename MakeBody>
void register_per_field_tests(std::string_view prefix, MakeBody make_body) {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        constexpr auto pmd = &[:m:];
        test(std::string{prefix} + "." + std::string{std::meta::identifier_of(m)})
            = make_body.template operator()<T, pmd>();
    }
}

// --- demo: applied to two different schemas to show genericity ---

struct User {
    std::string name;
    int         age;
    bool        admin;
};

struct Address {
    std::string street;
    std::string city;
    int         postal_code;
};

// Two trivial per-field checks. In a real project these would be
// round-trip, parse(serialize) == original, or invariant checks.
template <typename T> constexpr T arbitrary();
template <> constexpr User    arbitrary<User>()    { return {"Filip", 40, true}; }
template <> constexpr Address arbitrary<Address>() { return {"Rynek 1", "Wroclaw", 50001}; }

// The user-supplied per-field test body factory. Same lambda works
// for any T because it's parameterised on both the owning type and
// the pointer-to-member.
constexpr auto identity_round_trip = []<typename T, auto Pmd>() {
    return [] {
        T sample = arbitrary<T>();
        // Identity "round-trip" stand-in: in production this is
        // `T parsed = parse(serialize(sample)); sample.*Pmd ==
        // parsed.*Pmd`. The demo just confirms the field is
        // reachable + stable under access through the splice.
        auto value = sample.*Pmd;
        expect(value == sample.*Pmd) << "field stable under access";
    };
};

int main() {
    // Two schemas, one utility. Each call registers 3 named tests.
    register_per_field_tests<User>("user", identity_round_trip);
    register_per_field_tests<Address>("address", identity_round_trip);
}

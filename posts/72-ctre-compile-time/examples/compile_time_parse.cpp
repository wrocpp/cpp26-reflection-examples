// compile_time_parse.cpp
//
// CTRE matches run in constant expressions, which means a regex can validate
// and parse a string literal while the program is being compiled. A malformed
// literal then stops the build instead of surviving to run time.
//
// Nothing here is a trick with strings known to the optimiser: every assertion
// below is a static_assert, so if the matcher could not run at compile time the
// file would not compile at all.
//
// Compile: -std=c++20 -O2, lib ctre
// verify: ce-libs: ctre
// verify: gcc-options: -std=c++20 -O2

#include <ctre.hpp>

#include <cstdio>
#include <string_view>

using namespace std::string_view_literals;

// A named pattern has to be a ctll::fixed_string, not a const char*: a pointer
// to a string literal is not a valid template argument. This is the same type
// the pre-C++20 spelling used.
static constexpr auto kSemver = ctll::fixed_string{R"((\d+)\.(\d+)\.(\d+))"};

// 1. Validation. A consteval function cannot be called at run time at all,
//    so this is a compile-time-only check by construction.
consteval bool is_semver(std::string_view s) {
    return static_cast<bool>(ctre::match<kSemver>(s));
}

static_assert(is_semver("1.2.3"));
static_assert(is_semver("10.0.42"));
static_assert(!is_semver("1.2"));
static_assert(!is_semver("1.2.3-rc1"));
static_assert(!is_semver("banana"));

// 2. Parsing. The captures are string_views into the literal, usable in a
//    constant expression like any other.
constexpr int to_int(std::string_view s) {
    int n = 0;
    for (char c : s) { n = n * 10 + (c - '0'); }
    return n;
}

struct version {
    int major;
    int minor;
    int patch;
};

consteval version parse_semver(std::string_view s) {
    auto m = ctre::match<kSemver>(s);
    if (!m) { return version{-1, -1, -1}; }
    return version{to_int(m.get<1>().to_view()),
                   to_int(m.get<2>().to_view()),
                   to_int(m.get<3>().to_view())};
}

constexpr auto v = parse_semver("2.11.4");
static_assert(v.major == 2);
static_assert(v.minor == 11);
static_assert(v.patch == 4);

// 3. The practical shape: a build-time gate. Uncomment the bad line and the
//    compiler rejects the program with the message, not a runtime surprise.
// consteval, not constexpr: a constexpr function may be called at run time, so
// it cannot forward its parameter to a consteval one. Saying consteval here is
// the honest signature, because the check only ever happens at compile time.
consteval bool require_semver(std::string_view s) { return is_semver(s); }
static_assert(require_semver("0.1.0"), "version literal is not semver");
// static_assert(require_semver("nope"), "version literal is not semver");

int main() {
    std::printf("parsed at compile time: %d.%d.%d\n", v.major, v.minor, v.patch);
    std::printf("all %d assertions were checked before main existed\n", 11);
    return 0;
}

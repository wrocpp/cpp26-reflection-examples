// cnttp_before_after.cpp
//
// The single language feature that shaped CTRE's interface: class types as
// non-type template parameters (P0732 in C++20, repaired by P1907).
//
// A regex pattern is a string. Before C++20 a string literal could not be a
// template argument, so the pattern had to be smuggled in as a constexpr
// variable of a literal class type:
//
//     static constexpr auto pattern = ctll::fixed_string{"[a-z]+([0-9]+)"};
//     ctre::match<pattern>(subject);
//
// C++20 made ctll::fixed_string itself usable as a template parameter, which
// collapses the two lines into the spelling everyone knows:
//
//     ctre::match<"[a-z]+([0-9]+)">(subject);
//
// Both forms still work. This file compiles them side by side so the
// difference is visible rather than described.
//
// Compile: -std=c++20 -O2, lib ctre
// verify: ce-libs: ctre
// verify: gcc-options: -std=c++20 -O2

#include <ctre.hpp>

#include <cstdio>
#include <string_view>

using namespace std::string_view_literals;

int main() {
    constexpr auto subject = "abc123"sv;

    // ---- the pre-C++20 spelling, still supported ----
    static constexpr auto pattern = ctll::fixed_string{"[a-z]+([0-9]+)"};
    if (auto m = ctre::match<pattern>(subject)) {
        std::printf("fixed_string form : capture = %.*s\n",
                    (int)m.get<1>().size(), m.get<1>().data());
    }

    // ---- the C++20 spelling, pattern straight into the template argument ----
    if (auto m = ctre::match<"[a-z]+([0-9]+)">(subject)) {
        std::printf("cNTTP form        : capture = %.*s\n",
                    (int)m.get<1>().size(), m.get<1>().data());
    }

    // Both instantiate the same matcher: the literal is converted to the same
    // ctll::fixed_string, it just no longer needs a name.
    static_assert(ctre::match<pattern>(subject));
    static_assert(ctre::match<"[a-z]+([0-9]+)">(subject));

    std::printf("same matcher, two spellings, both constant-evaluated\n");
    return 0;
}

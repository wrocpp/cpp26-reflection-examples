// unevaluated_strings.cpp
//
// C++26 P2361R6 tidied up "unevaluated strings", the ones in static_assert,
// [[deprecated]], [[nodiscard]], extern "C" and asm. They never become objects
// at run time, so an encoding prefix or a numeric escape in them never meant
// anything. The paper makes both ill-formed.
//
// Both halves are in the paper. Only one is in GCC.
//
//   encoding prefix, u8"..." or L"...":  GCC rejects, clang rejects
//   numeric escape, "\x07" or "\007":    GCC ACCEPTS, clang rejects
//
// This file contains a numeric escape in a static_assert message, which the
// standard says is ill-formed. GCC 16.1 compiles it. Build the same source on
// clang 22.1 and it is rejected.
//
// Compile (gcc, accepts): g++     -std=c++26 unevaluated_strings.cpp
// Compile (clang, fails): clang++ -std=c++26 unevaluated_strings.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++26 -O1

#include <cstdio>

// Ill-formed per P2361R6: a numeric escape in an unevaluated string.
// GCC 16.1 accepts this. clang 22.1 rejects it.
static_assert(sizeof(int) >= 2, "needs \x07 at least two bytes");

// The other half of the rule, which GCC does enforce. Uncomment to see
// "error: a wide string is invalid in this context".
// static_assert(true, u8"encoded message");

// Evaluated string literals are untouched by the paper: every prefix and every
// escape still means what it always did.
constexpr const char* kBell = "bell \x07 here";
constexpr const char8_t* kUtf8 = u8"utf-8 é";

[[deprecated("plain text is fine")]]
static void old_function() {}

int main() {
    std::printf("this compiled, which on a conforming C++26 compiler it should not\n");
    std::printf("evaluated literal still has its escape: len=%zu\n",
                __builtin_strlen(kBell));
    std::printf("u8 literal still fine: %d\n", kUtf8 != nullptr);
    old_function();
    return 0;
}

// cplusplus_macro.cpp
//
// Four lines that show where each compiler thinks it is.
//
// LLVM 23.1.0 shipped on 2026-08-25 with -std=c++2d, a mode for the standard
// after C++26. GCC trunk has it too. Neither current release accepts the flag:
// clang 22.1 and GCC 16.2 both reject it at the driver.
//
// Measured on Compiler Explorer, 2026-08-28:
//
//   clang trunk  -std=c++2c   __cplusplus = 202400
//   clang trunk  -std=c++2d   __cplusplus = 202700
//   GCC trunk    -std=c++26   __cplusplus = 202603
//   GCC trunk    -std=c++2d   __cplusplus = 202700
//
// The two agree on 202700 for the next standard and disagree on the current
// one. 202603 is the correct value: C++26 was finished in March 2026, and the
// macro is the year and month of ratification. clang is still reporting the
// 202400 placeholder it used while C++26 was in progress.
//
// Flag spelling differs too. GCC accepts -std=c++29 and -std=c++2d. clang
// accepts only -std=c++2d and rejects -std=c++29 outright.
//
// Compile: g++ -std=c++2d cplusplus_macro.cpp
//          clang++ -std=c++2d cplusplus_macro.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++2d -O1

#include <cstdio>

int main() {
    std::printf("__cplusplus = %ld\n", (long)__cplusplus);

    // Named for the standard they belong to, so the numbers are readable
    // without a lookup table.
    if constexpr (__cplusplus > 202603L) {
        std::printf("mode        = after C++26\n");
    } else if constexpr (__cplusplus == 202603L) {
        std::printf("mode        = C++26 (ratified value)\n");
    } else if constexpr (__cplusplus == 202400L) {
        std::printf("mode        = C++26 (placeholder value, not yet updated)\n");
    } else if constexpr (__cplusplus == 202302L) {
        std::printf("mode        = C++23\n");
    } else {
        std::printf("mode        = older than C++23\n");
    }

    return 0;
}

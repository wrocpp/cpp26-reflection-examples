// ubsan_overflow.cpp  (~16 lines)
// UndefinedBehaviorSanitizer instruments the code the compiler would otherwise
// assume never happens. Signed overflow is undefined; with -fsanitize=undefined
// the program does not trap, it prints a precise diagnostic (file, line, the
// exact values) and keeps running, so the process still exits 0.
//
// Compile (GCC 16.1): g++ -std=c++20 -O1 -g -fsanitize=undefined ubsan_overflow.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++20 -O1 -g -fsanitize=undefined

#include <climits>
#include <cstdio>

int main() {
    int x = INT_MAX;
    std::printf("INT_MAX     = %d\n", x);
    int y = x + 1;  // signed integer overflow: UBSan reports it here
    std::printf("INT_MAX + 1 = %d (wrapped; undefined without the sanitizer)\n", y);
    return 0;
}

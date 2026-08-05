// float_cast.cpp  (~20 lines)
// Converting a floating-point value to an integer type is UNDEFINED BEHAVIOR
// when the truncated value will not fit in the target type, and also for NaN.
// Nothing in the source looks dangerous, no compiler warns by default, and the
// two major compilers produce DIFFERENT wrong answers, which is the clearest
// possible demonstration that "undefined" means undefined.
//
// GCC deliberately leaves float-cast-overflow OUT of -fsanitize=undefined, so
// you have to name the check. Clang includes it in -fsanitize=undefined.
//
//   g++     -std=c++20 -O1 -g -fsanitize=float-cast-overflow float_cast.cpp
//   clang++ -std=c++20 -O1 -g -fsanitize=undefined           float_cast.cpp
//
// verify: gcc-only
// verify: gcc-options: -std=c++20 -O1 -g -fsanitize=float-cast-overflow

#include <cstdio>

int main() {
    double big = 1e18;        // far outside the range of int
    double nan = 0.0 / 0.0;

    std::printf("in range   : %d\n", static_cast<int>(3.9));   // fine: 3
    std::fflush(stdout);

    int a = static_cast<int>(big);   // UB: does not fit
    std::printf("1e18 -> %d\n", a);
    std::fflush(stdout);

    int b = static_cast<int>(nan);   // UB: NaN
    std::printf("NaN  -> %d\n", b);
    return 0;
}

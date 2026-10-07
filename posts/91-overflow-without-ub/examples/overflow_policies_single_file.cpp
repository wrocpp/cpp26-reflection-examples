// Three policies for INT_MAX + 1 and x / 0 that are not undefined behavior.
// Build: g++ -std=c++26 -O2 -Wall -Wextra overflow_policies_single_file.cpp
#include <climits>
#include <cmath>
#include <cstdio>
#include <numeric>
#include <stdckdint.h>

int main() {
  volatile int vmax = INT_MAX;  // volatile: the values are not known at compile time
  volatile int vzero = 0;
  const int hi = vmax;
  const int lo = INT_MIN + vzero;
  const int zero = vzero;

  // Policy 1: clamp. The result changes.
  std::printf("saturating_add(INT_MAX, 1)  = %d\n", std::saturating_add(hi, 1));
  std::printf("saturating_sub(INT_MIN, 1)  = %d\n", std::saturating_sub(lo, 1));
  std::printf("saturating_mul(INT_MAX, 2)  = %d\n", std::saturating_mul(hi, 2));
  std::printf("saturating_div(INT_MIN, -1) = %d\n", std::saturating_div(lo, -1));
  std::printf("saturating_div(10, 3)       = %d\n", std::saturating_div(10 + zero, 3));

  // Policy 2: report. The caller decides.
  int r = 0;
  bool o = ckd_add(&r, hi, 1);
  std::printf("ckd_add(INT_MAX, 1)         : overflow=%d result=%d\n", o, r);
  o = ckd_mul(&r, hi, 2);
  std::printf("ckd_mul(INT_MAX, 2)         : overflow=%d result=%d\n", o, r);
  o = ckd_add(&r, 40, 2 + zero);
  std::printf("ckd_add(40, 2)              : overflow=%d result=%d\n", o, r);
  o = __builtin_add_overflow(hi, 1, &r);
  std::printf("__builtin_add_overflow      : overflow=%d result=%d\n", o, r);

  // Division by zero: the integer case needs a check of your own, the floating-point one is IEEE 754.
  std::printf("1.0 / 0 (double)            = %f\n", 1.0 / zero);
  std::printf("isnan(0.0 / 0)              = %d\n", std::isnan(0.0 / zero));
  std::printf("feature macros              : sat=%ld ckd=%ld\n",
              static_cast<long>(__cpp_lib_saturation_arithmetic), static_cast<long>(__cpp_lib_stdckdint_h));
  return 0;
}

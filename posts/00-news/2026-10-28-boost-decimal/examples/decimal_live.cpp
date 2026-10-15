// verify: gcc-only
// Manual mint, not the pipeline: needs the Boost 1.91 library on Compiler Explorer.
// g++ -std=c++20 -O2 (Boost 1.91 or later on the include path)
#include <boost/decimal.hpp>
#include <cmath>
#include <cstdio>
#include <iostream>

int main() {
  using boost::decimal::decimal32_t;
  using boost::decimal::decimal64_t;

  double d = 0.0;
  for (int i = 0; i < 1000; ++i) d += 0.1;
  std::printf("double   0.1 x 1000: %.15f\n", d);

  decimal32_t s32{0};
  const decimal32_t tenth{"0.1"};
  for (int i = 0; i < 1000; ++i) s32 += tenth;
  std::cout << "decimal32_t 0.1 x 1000: " << s32 << '\n';

  decimal64_t s64{0};
  const decimal64_t cent{"0.01"};
  for (int i = 0; i < 100; ++i) s64 += cent;
  std::cout << "decimal64_t 0.01 x 100: " << s64 << '\n';
  std::cout << "decimal64_t equals 1: " << (s64 == decimal64_t{1}) << '\n';
}

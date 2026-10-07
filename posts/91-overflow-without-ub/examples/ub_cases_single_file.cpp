// The undefined cases, one per run. Not embedded: most of these runs end in a trap on purpose.
// Build: g++ -std=c++26 -Wall -Wextra -DCASE=<n> [-O0|-O2] [-ftrapv|-fwrapv|-fsanitize=...] ub_cases_single_file.cpp
//   CASE=1  signed INT_MAX + 1, printed
//   CASE=2  the compiler's view of x + 1 > x
//   CASE=3  integer x / 0
//   CASE=4  INT_MIN / -1
#include <climits>
#include <cstdio>

#ifndef CASE
#define CASE 1
#endif

[[gnu::noinline]] bool plus_one_is_greater(int x) { return x + 1 > x; }
[[gnu::noinline]] int divide(int a, int b) { return a / b; }

int main() {
  [[maybe_unused]] volatile int vmax = INT_MAX;
  [[maybe_unused]] volatile int vmin = INT_MIN;
  [[maybe_unused]] volatile int vzero = 0;
  [[maybe_unused]] volatile int vminus1 = -1;
#if CASE == 1
  int r = vmax + 1;
  std::printf("INT_MAX + 1 = %d\n", r);
#elif CASE == 2
  std::printf("x + 1 > x for x = INT_MAX: %d\n", plus_one_is_greater(vmax));
#elif CASE == 3
  std::printf("10 / 0 = %d\n", divide(10, vzero));
#elif CASE == 4
  std::printf("INT_MIN / -1 = %d\n", divide(vmin, vminus1));
#endif
  return 0;
}

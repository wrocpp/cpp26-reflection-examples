// verify: gcc-only
// Build:  g++ -std=c++26 -O2 sum_codegen.cpp   (read the assembly with -S)
// Prints: nothing. The file exists to compare the code of three sums over
//         a std::span<const int>; main only keeps the functions linked.
// Limits: x86-64 GCC 16.2 and Clang 22.1 were read; other targets were not.
#include <algorithm>
#include <array>
#include <functional>
#include <numeric>
#include <span>

int sum_loop(std::span<const int> v) {
  int s = 0;
  for (int x : v) s += x;
  return s;
}

int sum_accumulate(std::span<const int> v) {
  return std::accumulate(v.begin(), v.end(), 0);
}

int sum_fold(std::span<const int> v) {
  return std::ranges::fold_left(v, 0, std::plus{});
}

int main() {
  constexpr std::array<int, 3> data{1, 2, 3};
  constexpr int kExpected = 6;
  const bool same = sum_loop(data) == kExpected && sum_accumulate(data) == kExpected && sum_fold(data) == kExpected;
  return same ? 0 : 1;
}

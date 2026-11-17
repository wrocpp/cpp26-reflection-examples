// verify: gcc-only
// Build:  g++ -std=c++26 -O1 sum_three_ways.cpp
// Prints: the three sums of {0.5, 0.5, 0.5}, then std::saturating_add at INT_MAX.
// Limits: needs std::ranges::fold_left (C++23) and std::saturating_add (C++26).
//         The truncating accumulate compiles without a warning on the
//         compilers and flags listed in the README.
#include <algorithm>
#include <climits>
#include <functional>
#include <numeric>
#include <print>
#include <type_traits>
#include <vector>

namespace {

constexpr double kHalf = 0.5;
constexpr int kCopies = 3;
constexpr int kLimit = INT_MAX;

double sum_loop(const std::vector<double>& v) {
  double total = 0;
  for (double x : v) total += x;
  return total;
}

auto sum_accumulate(const std::vector<double>& v) {
  return std::accumulate(v.begin(), v.end(), 0);
}

auto sum_fold(const std::vector<double>& v) {
  return std::ranges::fold_left(v, 0, std::plus{});
}

static_assert(std::is_same_v<decltype(sum_accumulate(std::vector<double>{})), int>);
static_assert(std::is_same_v<decltype(sum_fold(std::vector<double>{})), double>);

int sum_saturating(const std::vector<int>& v) {
  int total = 0;
  for (int x : v) total = std::saturating_add(total, x);
  return total;
}

}  // namespace

int main() {
  const std::vector<double> halves(kCopies, kHalf);
  std::println("loop        {}", sum_loop(halves));
  std::println("accumulate  {}", sum_accumulate(halves));
  std::println("fold_left   {}", sum_fold(halves));

  const std::vector<int> big{kLimit, 1};
  std::println("saturating_add(INT_MAX, 1)  {}", std::saturating_add(kLimit, 1));
  std::println("saturating loop      {}", sum_saturating(big));
}

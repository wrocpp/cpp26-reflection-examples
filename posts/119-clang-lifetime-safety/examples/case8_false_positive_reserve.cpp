// verify: gcc-only
// Build: g++ -std=c++26 -Wall -Wextra case8_false_positive_reserve.cpp (a correct program that Clang -Wlifetime-safety flags).
// Prints: "7" and exits 0. The reference stays valid because reserve(8) leaves room for both push_back calls.
// Limits: Clang 23.1 reports the reference as invalidated; this file exists to show that false positive.

#include <cstdio>
#include <vector>
namespace {
constexpr int kCapacity = 8;
constexpr int kFirst = 7;
constexpr int kSecond = 8;
}  // namespace

int main() {
  std::vector<int> v;
  v.reserve(kCapacity);
  v.push_back(kFirst);
  const int& first = v[0];
  v.push_back(kSecond);
  std::printf("%d\n", first);
}

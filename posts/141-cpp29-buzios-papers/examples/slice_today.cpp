// verify: gcc-only
// views::slice(E, F, G) in P3216R6 is expression-equivalent to drop(F), then take(G - F): the half-open interval
// [F, G) of positions, cut off at the end of a shorter range. This file builds that adaptor by hand for index arguments
// of type ptrdiff_t and runs the examples from the paper: a string_view, a vector with an end index past the end,
// two empty results, and the endless view iota(0).
// Build: g++ -std=c++26 -O1   (Compiler Explorer: g162). None of the library builds I tried on 2026-11-21 has
// std::views::slice: see the README for the compilers and flags.
// Prints each slice. Exits 0 when every slice equals its expected contents. The program has no macros.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

constexpr std::string_view kText = "Hello, world!";
constexpr std::array kWord{'w', 'o', 'r', 'l', 'd'};
constexpr std::array kTail{2, 3, 4, 5};
constexpr std::array kFifties{50, 51, 52, 53, 54, 55, 56, 57, 58, 59};
constexpr std::ptrdiff_t kWordFirst = 7;
constexpr std::ptrdiff_t kWordLast = 12;
constexpr std::ptrdiff_t kFiftyFirst = 50;
constexpr std::ptrdiff_t kFiftyLast = 60;
constexpr std::ptrdiff_t kPastTheEnd = 10;
constexpr std::ptrdiff_t kSecond = 1;
constexpr std::ptrdiff_t kThird = 2;
constexpr std::ptrdiff_t kAtTheEnd = 5;

constexpr auto slice(std::ptrdiff_t first, std::ptrdiff_t last) {
  return std::views::drop(first) | std::views::take(last - first);
}

bool word_matches() { return std::ranges::equal(kText | slice(kWordFirst, kWordLast), kWord); }

bool vector_cases_match() {
  const std::vector<int> v{1, 2, 3, 4, 5};
  return std::ranges::equal(v | slice(kSecond, kPastTheEnd), kTail) && std::ranges::empty(v | slice(kThird, kThird)) &&
         std::ranges::empty(v | slice(kAtTheEnd, kPastTheEnd));
}

bool endless_matches() { return std::ranges::equal(std::views::iota(0) | slice(kFiftyFirst, kFiftyLast), kFifties); }

int main() {
  std::printf("%s\n", std::string{std::from_range, kText | slice(kWordFirst, kWordLast)}.c_str());
  for (int n : std::views::iota(0) | slice(kFiftyFirst, kFiftyLast)) std::printf("%d ", n);
  std::printf("\n");
  bool ok = word_matches() && vector_cases_match() && endless_matches();
  std::printf("all slices as in the paper: %s\n", ok ? "yes" : "no");
  return ok ? 0 : 1;
}

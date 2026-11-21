// verify: gcc-only
// The one placeholder return type that a defaulted function may have today: auto on a defaulted operator<=>.
// P2952R5 (deduced return types for defaulted operator functions) extends that to operator==, the relational
// operators, copy and move assignment and the increment operators, so auto operator==(const X&) const = default; and
// auto& operator=(const X&) = default; become well-formed. No compiler accepts those two yet: see the README for the
// error each of GCC 16.2, GCC trunk, Clang 23.1, Clang trunk and MSVC gives. The paper says "Implementation
// experience: None yet."
// This file shows what already works: the deduced category follows the members (strong for ints, partial for a double).
// Build: g++ -std=c++26 -O1   (Compiler Explorer: g162; also builds with clang2310)
// Prints three results. Exits 0 when they are as expected. The program has no macros.
#include <compare>
#include <cstdio>
#include <limits>
#include <type_traits>

struct Version {
  int major;
  int minor;
  auto operator<=>(const Version&) const = default;
};

struct Reading {
  double value;
  int sensor;
  auto operator<=>(const Reading&) const = default;
};

static_assert(std::is_same_v<decltype(Version{} <=> Version{}), std::strong_ordering>);
static_assert(std::is_same_v<decltype(Reading{} <=> Reading{}), std::partial_ordering>);

int main() {
  constexpr double kNotANumber = std::numeric_limits<double>::quiet_NaN();
  constexpr Version kOlder{.major = 1, .minor = 2};
  constexpr Version kNewer{.major = 1, .minor = 3};
  constexpr Reading kBroken{.value = kNotANumber, .sensor = 0};
  constexpr Reading kNormal{.value = 1.0, .sensor = 0};
  bool older = kOlder < kNewer;
  bool unordered = (kBroken <=> kNormal) == std::partial_ordering::unordered;
  std::printf("Version 1.2 < 1.3: %s\n", older ? "yes" : "no");
  std::printf("Reading with NaN is unordered against 1.0: %s\n", unordered ? "yes" : "no");
  return older && unordered ? 0 : 1;
}

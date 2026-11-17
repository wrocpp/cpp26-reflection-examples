// verify: gcc-only
// Build: g++ -std=c++26 -Wall -Wextra case5_reference_across_push_back.cpp (or any Clang with -std=c++26 and the warning flags under test).
// Prints: "case5: safe path" and exits 0 when run with no argument. With the argument "bug" it runs the dangling access.
// Limits: the "bug" path is undefined behaviour; it is meant to be run under -fsanitize=address.

#include <cstdio>
#include <vector>

namespace {
constexpr int kFirst = 7;
constexpr int kFill = 1000;
}  // namespace

int main(int argc, char**) {
  if (argc > 1) {
    std::vector<int> v{kFirst};
    const int& first = v[0];
    for (int i = 0; i < kFill; ++i) v.push_back(i);
    std::printf("%d\n", first);
    return 0;
  }
  std::puts("case5: safe path");
  return 0;
}

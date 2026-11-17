// verify: gcc-only
// Build: g++ -std=c++26 -Wall -Wextra case2_view_of_by_value_owner.cpp (or any Clang with -std=c++26 and the warning flags under test).
// Prints: "case2: safe path" and exits 0 when run with no argument. With the argument "bug" it runs the dangling access.
// Limits: the "bug" path is undefined behaviour; it is meant to be run under -fsanitize=address.

#include <cstdio>
#include <string>
#include <string_view>

namespace {
constexpr const char* kText = "a string long enough to leave the small buffer";

std::string_view view_of(std::string owner) {
  return owner;
}
}  // namespace

int main(int argc, char**) {
  if (argc > 1) {
    const std::string_view v = view_of(std::string{kText});
    std::printf("%c\n", v.front());
    return 0;
  }
  std::puts("case2: safe path");
  return 0;
}

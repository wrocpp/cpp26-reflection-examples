// verify: gcc-only
// Build: g++ -std=c++26 -Wall -Wextra case1_return_view_of_local.cpp (or any Clang with -std=c++26 and the warning flags under test).
// Prints: "case1: safe path" and exits 0 when run with no argument. With the argument "bug" it runs the dangling access.
// Limits: the "bug" path is undefined behaviour; it is meant to be run under -fsanitize=address.

#include <cstdio>
#include <string>
#include <string_view>

namespace {
constexpr const char* kText = "a string long enough to leave the small buffer";

std::string_view view_of_local() {
  std::string text{kText};
  return text;
}
}  // namespace

int main(int argc, char**) {
  if (argc > 1) {
    const std::string_view v = view_of_local();
    std::printf("%c\n", v.front());
    return 0;
  }
  std::puts("case1: safe path");
  return 0;
}

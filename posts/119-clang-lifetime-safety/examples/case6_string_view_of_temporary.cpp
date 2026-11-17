// verify: gcc-only
// Build: g++ -std=c++26 -Wall -Wextra case6_string_view_of_temporary.cpp (or any Clang with -std=c++26 and the warning flags under test).
// Prints: "case6: safe path" and exits 0 when run with no argument. With the argument "bug" it runs the dangling access.
// Limits: the "bug" path is undefined behaviour; it is meant to be run under -fsanitize=address.

#include <cstdio>
#include <string>
#include <string_view>

namespace {
constexpr const char* kText = "a string long enough to leave the small buffer";
}  // namespace

int main(int argc, char**) {
  if (argc > 1) {
    const std::string_view v = std::string{kText};
    std::printf("%c\n", v.front());
    return 0;
  }
  std::puts("case6: safe path");
  return 0;
}

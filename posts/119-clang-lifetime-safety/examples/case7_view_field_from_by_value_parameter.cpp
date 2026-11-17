// verify: gcc-only
// Build: g++ -std=c++26 -Wall -Wextra case7_view_field_from_by_value_parameter.cpp (or any Clang with -std=c++26 and the warning flags under test).
// Prints: "case7: safe path" and exits 0 when run with no argument. With the argument "bug" it runs the dangling access.
// Limits: the "bug" path is undefined behaviour; it is meant to be run under -fsanitize=address.

#include <cstdio>
#include <string>
#include <string_view>

namespace {
constexpr const char* kText = "a string long enough to leave the small buffer";

struct Holder {
  std::string_view name;
  explicit Holder(std::string s) : name{s} {}
};
}  // namespace

int main(int argc, char**) {
  if (argc > 1) {
    const Holder h{std::string{kText}};
    std::printf("%c\n", h.name.front());
    return 0;
  }
  std::puts("case7: safe path");
  return 0;
}

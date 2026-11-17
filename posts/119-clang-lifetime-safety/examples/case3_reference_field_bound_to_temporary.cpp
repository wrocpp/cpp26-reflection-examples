// verify: gcc-only
// Build: g++ -std=c++26 -Wall -Wextra case3_reference_field_bound_to_temporary.cpp (or any Clang with -std=c++26 and the warning flags under test).
// Prints: "case3: safe path" and exits 0 when run with no argument. With the argument "bug" it runs the dangling access.
// Limits: the "bug" path is undefined behaviour; it is meant to be run under -fsanitize=address.

#include <cstdio>
#include <string>

namespace {
constexpr const char* kText = "a string long enough to leave the small buffer";

struct Holder {
  const std::string& name;
  explicit Holder(const std::string& n) : name{n} {}
};
}  // namespace

int main(int argc, char**) {
  if (argc > 1) {
    const Holder h{std::string{kText}};
    std::printf("%c\n", h.name.front());
    return 0;
  }
  std::puts("case3: safe path");
  return 0;
}

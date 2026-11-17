// verify: gcc-only
// Build: g++ -std=c++26 -Wall -Wextra case4_lambda_capture_escapes.cpp (or any Clang with -std=c++26 and the warning flags under test).
// Prints: "case4: safe path" and exits 0 when run with no argument. With the argument "bug" it runs the dangling access.
// Limits: the "bug" path is undefined behaviour; it is meant to be run under -fsanitize=address.

#include <cstdio>
#include <functional>

namespace {
constexpr int kValue = 41;

std::function<int()> make_counter() {
  int local = kValue;
  return [&local] { return local + 1; };
}
}  // namespace

int main(int argc, char**) {
  if (argc > 1) {
    const auto f = make_counter();
    std::printf("%d\n", f());
    return 0;
  }
  std::puts("case4: safe path");
  return 0;
}

// verify: gcc-only
// Two classes that differ only in the access of one member: only the one with public members can be a template argument.
// The check is a concept over a requires-expression, so the
// program compiles; the README quotes what the compilers say when the private class is used directly.
// Build: g++ -std=c++26   (Compiler Explorer: g162 and clang2210)
// No macros; the program exits 0.
#include <cstdio>

constexpr int kWidth = 3;

struct Open {
  constexpr Open(int v) : value(v) {}
  int value;
};

class Hidden {
 public:
  constexpr Hidden(int v) : value_(v) {}

 private:
  int value_;
};

template <auto V>
struct holder {};

template <typename T>
concept usable_as_argument = requires { typename holder<T{kWidth}>; };

static_assert(usable_as_argument<Open>);
static_assert(!usable_as_argument<Hidden>);

int main() {
  holder<Open{kWidth}> h;
  (void)h;
  std::printf("Open: %d, Hidden: %d\n", usable_as_argument<Open>, usable_as_argument<Hidden>);
  return 0;
}

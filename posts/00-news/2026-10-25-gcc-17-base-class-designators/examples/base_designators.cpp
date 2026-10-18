// verify: gcc-only
// Manual mint, not the pipeline: needs GCC trunk with -std=c++29.
// g++ -std=c++29   (GCC 17 development branch)
#include <cstdio>

struct A { int a; };
struct B : A { int b; };

int main() {
  auto b1 = B{.a = 1, .b = 2};    // P2287R6: name the inherited member directly
  auto b2 = B{{.a = 1}, .b = 2};  // or brace the base subobject
  auto b3 = B{A{1}, .b = 2};      // or initialize the base by type
  std::printf("b1: %d %d\nb2: %d %d\nb3: %d %d\n", b1.a, b1.b, b2.a, b2.b, b3.a, b3.b);
  std::printf("compiler: %s\n", __VERSION__);
}

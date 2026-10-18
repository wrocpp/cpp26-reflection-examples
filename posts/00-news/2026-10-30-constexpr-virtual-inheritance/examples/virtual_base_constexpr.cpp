// verify: gcc-only
// Manual mint, not the pipeline: needs a compiler that accepts P3533R2.
// g++ -std=c++26 [-DCALL_THROUGH_BASE]    clang++ -std=c++26 [-DCALL_THROUGH_BASE]
#include <cstdio>

struct A {
  int v;
  constexpr A(int x) : v(x) {}
  constexpr virtual int f() const { return v; }
};
struct B : virtual A {
  constexpr B() : A(1) {}
  constexpr int f() const override { return 2; }
};
struct C : virtual A {
  constexpr C() : A(1) {}
};
struct D : B, C {
  constexpr D() : A(5) {}
};

constexpr int read_base() {  // a data member of the virtual base
  D d;
  const A& a = d;
  return a.v;
}
constexpr int direct_call() {  // a virtual call on the complete object
  B b;
  return b.f();
}
static_assert(read_base() == 5);
static_assert(direct_call() == 2);

#ifdef CALL_THROUGH_BASE
constexpr int through_base() {  // a virtual call through a reference to the virtual base
  B b;
  const A& a = b;
  return a.f();
}
static_assert(through_base() == 2);
#endif

int main() {
  std::printf("read_base = %d\n", read_base());
  std::printf("direct_call = %d\n", direct_call());
#ifdef CALL_THROUGH_BASE
  std::printf("through_base = %d\n", through_base());
#endif
#ifdef __cpp_constexpr_virtual_inheritance
  std::printf("__cpp_constexpr_virtual_inheritance = %ld\n", (long)__cpp_constexpr_virtual_inheritance);
#else
  std::puts("__cpp_constexpr_virtual_inheritance: undefined");
#endif
}

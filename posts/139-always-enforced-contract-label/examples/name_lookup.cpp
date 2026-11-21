// verify: ce-only
// The label is found by ordinary name lookup (P4451R2 section 2.4), so four spellings denote the same label:
// the qualified name, a namespace alias, a using-directive and a using-declaration. All four are checked here,
// and each function is called with a valid value, so the program prints one line per spelling and exits 0.
//
// Build (Compiler Explorer, GCC and Clang with P4451 support):
//   gcc_notadragon_contracts_p3850     -std=c++26 -fcontracts-p4451
//   clang_notadragon_contracts_p3850   -std=c++26 -fcontracts-p4451
// bare_name.cpp is the spelling that does not compile.
#include <contracts>
#include <cstdio>

namespace {

constexpr int kValid = 1;

namespace scl = std::contracts::controls;

namespace viaDirective {
using namespace std::contracts::controls;
int check(const int x) pre<enforced>(x > 0) { return x; }
}  // namespace viaDirective

int qualified(const int x) pre<std::contracts::controls::enforced>(x > 0) { return x; }

int aliased(const int x) pre<scl::enforced>(x > 0) { return x; }

int declared(const int x) {
  using std::contracts::controls::enforced;
  contract_assert<enforced>(x > 0);
  return x;
}

}  // namespace

int main() {
  std::printf("qualified name: %d\n", qualified(kValid));
  std::printf("namespace alias: %d\n", aliased(kValid));
  std::printf("using-directive: %d\n", viaDirective::check(kValid));
  std::printf("using-declaration: %d\n", declared(kValid));
}

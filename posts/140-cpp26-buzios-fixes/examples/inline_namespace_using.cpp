// verify: ce-only
// The rule of P4466R2 in a few lines, with no modules: a using-declarator at namespace scope that is separated
// from the declaration it names only by an inline namespace. libstdc++'s std module exports std::basic_string in this
// shape, because basic_string lives in the inline namespace std::__cxx11. The reflection names the function, not the
// using-declarator.
// Build: g++ -std=c++26 -freflection
//   Compiler Explorer gcontracts-p4324-trunk (the GCC fork that P4466R2 names as implementation experience): exits 0.
//   Compiler Explorer g162 and gsnapshot: error: '^^' cannot be applied to a using-declaration
// The program has no macros. It needs a compiler that implements the rule, which is why the first line says ce-only.
#include <meta>

#include <print>

namespace NS {
inline namespace Inline {
void f();
}
using Inline::f;
}  // namespace NS

constexpr std::meta::info reflected = ^^NS::f;
static_assert(reflected == ^^NS::Inline::f);
static_assert(std::meta::parent_of(reflected) == ^^NS::Inline);

int main() {
  std::println("{} lives in {}", std::meta::identifier_of(reflected),
               std::meta::identifier_of(std::meta::parent_of(reflected)));
}

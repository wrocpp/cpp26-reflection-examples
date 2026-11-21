// verify: ce-only
// The same two reflections as reflect_through_include.cpp, with import std; in place of the includes. libstdc++'s std
// module re-exports each entity with a using-declaration, and that is the case FI-067 reported and P4466R2 changes.
// Build: g++ -std=c++26 -freflection -fmodules -fsearch-include-path bits/std.cc
//   Compiler Explorer gcontracts-p4324-trunk (the GCC fork that P4466R2 names as implementation experience): exits 0.
//   Compiler Explorer g162 and gsnapshot: error: '^^' cannot be applied to a using-declaration   (on the basic_string line)
// The module build needs the compile gate to produce a binary (the API option "binary"); the Compiler Explorer pages
// do that on their own. The program has no macros. The Clang module form was not run.
import std;

constexpr std::meta::info string_template = ^^std::basic_string;
constexpr std::meta::info vector_template = ^^std::vector;
static_assert(std::meta::is_template(string_template));
static_assert(std::meta::is_template(vector_template));

int main() {
  std::println("{} in {}", std::meta::identifier_of(string_template),
               std::meta::identifier_of(std::meta::parent_of(string_template)));
  std::println("{} in {}", std::meta::identifier_of(vector_template),
               std::meta::identifier_of(std::meta::parent_of(vector_template)));
}

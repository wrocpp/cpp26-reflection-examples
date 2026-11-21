// verify: gcc-only
// Reflect two library templates after #include. The same lines with import std; instead of the includes are in
// reflect_through_module.cpp, which is the case P4466R2 (adopted at Buzios) is about.
// Build: g++ -std=c++26 -freflection   (Compiler Explorer: g162, gsnapshot, gcontracts-p4324-trunk; clang_bb_p2996 with
//        -std=c++26 -freflection-latest -stdlib=libc++). The program has no macros and exits 0 on each of them.
#include <meta>

#include <print>
#include <string>
#include <vector>

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

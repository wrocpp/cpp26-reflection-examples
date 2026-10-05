// reflect_three_compilers.cpp
//
// One P2996 snippet for three front ends: list the data members of a struct
// with their types. It compiles and runs unchanged on GCC 16.2, EDG 7.0 and
// EDG trunk (both Compiler Explorer reflection builds), and on clang-p2996.
//
// The members are walked with an index_sequence rather than `template for`:
// EDG 7.0 and trunk reject expansion statements (no __cpp_expansion_statements,
// "explicit instantiation is not allowed in the current scope"). The table is
// static constexpr so the runtime lambda does not capture a consteval-only
// value; capturing it by reference is rejected by GCC 16.2.
//
// Output differs in one place: display_string_of(^^char const*) is
// implementation-defined, and GCC spells it "const char*" where clang-p2996
// and EDG print "const char *". See outputs.txt.
//
// Compile (GCC 16.2):    g++ -std=c++26 -freflection reflect_three_compilers.cpp
// Compile (clang-p2996): clang++ -std=c++26 -freflection-latest -stdlib=libc++ reflect_three_compilers.cpp
// Compile (EDG 7.0):     --c++26 --set_flag reflection   (Compiler Explorer: edg-7_0-reflection)
//
// verify: ce-only
//
// Minted by hand: shorten-examples.py has no EDG profile. The EDG links are
// executor sessions; a compile-only EDG session reports success for sources
// the build step rejects.

#include <meta>
#include <cstddef>
#include <cstdio>
#include <utility>

struct Point {
    int x;
    double y;
    char const* label;
};

template <class T>
void print_members() {
    static constexpr auto members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()));
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        (std::printf("%s : %s\n",
                     std::define_static_string(std::meta::identifier_of(members[I])),
                     std::define_static_string(std::meta::display_string_of(std::meta::type_of(members[I])))),
         ...);
    }(std::make_index_sequence<members.size()>{});
}

int main() {
    print_members<Point>();
}

// Post 3 — Splicing: [: r :] and putting reflections back into code.
// Compile: ./cpp posts/03-splicing/examples/splice_basics.cpp \
//                -o posts/03-splicing/examples/splice_basics
// Run:     ./run posts/03-splicing/examples/splice_basics

#include <experimental/meta>
#include <print>
#include <string>

// Splice a reflection in type position.
constexpr auto r_int = ^^int;
using IntAgain = [: r_int :];
static_assert(std::is_same_v<IntAgain, int>);

// Splice a member reflection in member-access position.
struct Point { int x; int y; };

template <std::meta::info M, typename T>
auto read(T const& obj) {
    return obj.[: M :];
}

template <typename T>
void dump(T const& obj) {
    constexpr auto ctx = std::meta::access_context::unchecked();
    constexpr auto members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, ctx));
    std::println("{}:", std::meta::display_string_of(^^T));
    template for (constexpr auto m : members) {
        std::println("  {} = {}",
                     std::meta::identifier_of(m),
                     obj.[: m :]);
    }
}

int main() {
    Point p{3, 4};
    std::println("read<Point::x>: {}", read<std::meta::nonstatic_data_members_of(
        ^^Point, std::meta::access_context::unchecked())[0]>(p));
    dump(p);
}

// Post 4 — template for: iterating reflections at compile time.
// Compile: ./cpp posts/04-expansion-statements/examples/dump.cpp \
//                -o posts/04-expansion-statements/examples/dump
// Run:     ./run posts/04-expansion-statements/examples/dump

#include <experimental/meta>
#include <print>
#include <string>

template <typename T>
void dump(T const& obj) {
    constexpr auto ctx = std::meta::access_context::unchecked();
    std::println("{}:", std::meta::display_string_of(^^T));
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        std::println("  {} = {}",
                     std::meta::identifier_of(m),
                     obj.[: m :]);
    }
}

struct Point { int x; int y; };
struct Line  { Point a; Point b; std::string name; };

int main() {
    dump(Point{3, 4});
    // template for over a tuple of constexpr-friendly values.
    // (std::string inside a tuple isn't a structural NTTP type; use array-like data instead.)
    template for (constexpr auto x : std::tuple{1, 2.5, 'c'}) {
        std::println("tuple element: {}", x);
    }
}

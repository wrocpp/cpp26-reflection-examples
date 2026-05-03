// Post 4 — template for: iterating reflections at compile time.
// Compile: ./cpp posts/04-expansion-statements/examples/dump.cpp \
//                -o posts/04-expansion-statements/examples/dump
// Run:     ./run posts/04-expansion-statements/examples/dump

#include <experimental/meta>
#include <format>
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

// Tiny {fmt} adapter so the nested Point member prints inside the
// template-for body. Without this, "  {} = {}" cannot format a Point
// directly. With it, every member of Line gets a clean rendering and
// the post-text output matches the actual program output.
template <>
struct std::formatter<Point> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(Point const& p, auto& ctx) const {
        return std::format_to(ctx.out(), "Point{{x={}, y={}}}", p.x, p.y);
    }
};

int main() {
    dump(Line{{0,0}, {3,4}, "diag"});
}

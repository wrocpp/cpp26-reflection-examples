// custom_formatter.cpp  (~30 lines)
// "Make {} work on your smart pointer": write the std::formatter the standard
// leaves out. The catch worth knowing: you may only add a specialization to a
// std template when a PROGRAM-DEFINED type is involved. Specializing
// std::formatter<std::unique_ptr<Point>> is fine because Point is yours;
// std::formatter<std::unique_ptr<int>> would be non-conforming (all types are
// from the standard library). That program-defined pointee is also the realistic
// case -- you want to print unique_ptr<YourType>, not unique_ptr<int>.
//
// Compile (GCC 16.1):              g++-16 -std=c++26 custom_formatter.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ custom_formatter.cpp -o demo

#include <format>
#include <memory>
#include <print>

struct Point { int x, y; };

// Allowed: the specialization mentions Point, a program-defined type.
template <class CharT>
struct std::formatter<std::unique_ptr<Point>, CharT> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(const std::unique_ptr<Point>& p, auto& ctx) const {
        if (p) return std::format_to(ctx.out(), "Point({}, {})", p->x, p->y);
        return std::format_to(ctx.out(), "(null)");
    }
};

int main() {
    auto p = std::make_unique<Point>(3, 4);
    std::println("{}", p);   // {} now works directly: Point(3, 4)
}

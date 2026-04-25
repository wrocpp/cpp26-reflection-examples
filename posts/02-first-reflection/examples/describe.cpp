// Post 2 — Your first ^^: reflecting types and walking members.
// Compile: ./cpp posts/02-first-reflection/examples/describe.cpp \
//                -o posts/02-first-reflection/examples/describe
// Run:     ./run posts/02-first-reflection/examples/describe

#include <experimental/meta>
#include <print>
#include <string>

template <typename T>
consteval char const* describe() {
    std::string out;
    constexpr auto ctx = std::meta::access_context::unchecked();
    for (auto m : std::meta::nonstatic_data_members_of(^^T, ctx)) {
        out += std::meta::identifier_of(m);
        out += ": ";
        out += std::meta::display_string_of(std::meta::type_of(m));
        out += '\n';
    }
    return std::define_static_string(out);
}

struct Point { int x; int y; };
struct Line  { Point a; Point b; std::string name; };

int main() {
    // describe() returns const char* backed by compile-time-static storage,
    // so the result is a valid constant expression usable anywhere.
    static constexpr char const* p = describe<Point>();
    static constexpr char const* l = describe<Line>();
    std::println("Point:\n{}", p);
    std::println("Line:\n{}", l);
}

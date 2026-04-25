// Post 6 — Auto-generating std::formatter<T> for any aggregate.
// Compile: ./cpp posts/06-auto-formatter/examples/formatter.cpp \
//                -o posts/06-auto-formatter/examples/formatter
// Run:     ./run posts/06-auto-formatter/examples/formatter

#include <experimental/meta>
#include <format>
#include <print>
#include <string>
#include <type_traits>

template <typename T>
    requires std::is_aggregate_v<T> && (!std::is_array_v<T>)
struct std::formatter<T> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }

    auto format(T const& obj, auto& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "{}{{",
                             std::meta::display_string_of(^^T));
        bool first = true;
        constexpr auto mctx = std::meta::access_context::unchecked();
        template for (constexpr auto m
                      : std::define_static_array(
                          std::meta::nonstatic_data_members_of(^^T, mctx))) {
            if (!first) out = std::format_to(out, ", ");
            first = false;
            out = std::format_to(out, "{}: {}",
                                 std::meta::identifier_of(m),
                                 obj.[:m:]);
        }
        return std::format_to(out, "}}");
    }
};

struct Address { std::string city; int postal_code; };
struct User    { std::string name; int age; bool admin; Address home; };

int main() {
    User u{"Filip", 40, true, {"Warsaw", 12345}};
    std::println("{}", u);
}

// Post 6 -- enum integration. The generic aggregate formatter from
// formatter.cpp recurses into each field's std::formatter<...>. If that
// formatter is the renum-based one from post 5 (enum-to-string), enum
// fields print their name instead of the numeric value.
//
// Compile + run:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/f && /tmp/f" \
//     posts/06-auto-formatter/examples/formatter_enum.cpp

#include <experimental/meta>
#include <format>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>

namespace renum {
template <typename E>
consteval auto enumerators_of_static() {
    return std::define_static_array(std::meta::enumerators_of(^^E));
}
template <typename E>
constexpr std::string_view to_string(E value) {
    template for (constexpr auto e : enumerators_of_static<E>()) {
        if ([:e:] == value) return std::meta::identifier_of(e);
    }
    return "<unknown>";
}
}  // namespace renum

// Generic enum formatter: drops to renum::to_string for the name.
template <typename E>
    requires std::is_enum_v<E>
struct std::formatter<E> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(E value, auto& ctx) const {
        return std::format_to(ctx.out(), "{}", renum::to_string(value));
    }
};

// Generic aggregate formatter (from formatter.cpp).
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

enum class Role { user, admin };
struct User { std::string name; Role role; };

int main() {
    User u{"filip", Role::admin};
    std::println("{}", u);
    // User{name: filip, role: admin}
}

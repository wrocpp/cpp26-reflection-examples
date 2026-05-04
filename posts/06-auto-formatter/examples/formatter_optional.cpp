// Post 6 -- optional fields. C++26 ships std::formatter<std::optional<T>>;
// for older toolchains drop in the small specialisation below. The
// generic aggregate formatter then prints optional fields as either the
// underlying value or "nullopt".
//
// Compile + run:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/f && /tmp/f" \
//     posts/06-auto-formatter/examples/formatter_optional.cpp

#include <experimental/meta>
#include <format>
#include <optional>
#include <print>
#include <string>
#include <type_traits>

// Small std::formatter<std::optional<T>> for environments that don't
// ship the C++26 standard one yet.
template <typename T>
struct std::formatter<std::optional<T>> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(std::optional<T> const& opt, auto& ctx) const {
        if (opt) return std::format_to(ctx.out(), "{}", *opt);
        return std::format_to(ctx.out(), "nullopt");
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

struct User { std::string name; std::optional<int> age; };

int main() {
    std::println("{}", User{"filip", std::nullopt});
    std::println("{}", User{"alice", 42});
    // User{name: filip, age: nullopt}
    // User{name: alice, age: 42}
}

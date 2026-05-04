// Post 6 -- explicit per-type formatter coexisting with the generic
// aggregate one. The generic specialisation is constrained on
// is_aggregate_v<T>; an explicit std::formatter<MyType> is more
// constrained still and wins overload resolution. Use this for types
// that need bespoke formatting (dates, currencies, redacted strings).
//
// Compile + run:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/f && /tmp/f" \
//     posts/06-auto-formatter/examples/formatter_custom.cpp

#include <experimental/meta>
#include <format>
#include <print>
#include <string>
#include <type_traits>

// Generic aggregate formatter (catches everything by default).
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

struct MyDate { int year; int month; int day; };

// Explicit specialisation -- more constrained than the generic one,
// so it wins overload resolution and emits ISO-8601 instead of the
// default field dump.
template <>
struct std::formatter<MyDate> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(MyDate const& d, auto& ctx) const {
        return std::format_to(ctx.out(), "{:04d}-{:02d}-{:02d}",
                              d.year, d.month, d.day);
    }
};

struct Event { std::string name; MyDate when; };

int main() {
    std::println("{}", MyDate{2026, 5, 14});
    std::println("{}", Event{"C++26 ratification", {2026, 3, 28}});
    // 2026-05-14
    // Event{name: C++26 ratification, when: 2026-03-28}
}

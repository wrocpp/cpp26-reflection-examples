// Post 6 -- sensitive-field redaction. P3394 user-defined annotations
// (also adopted into C++26) let a field carry a `[[=debug_print::skip{}]]`
// marker. The aggregate formatter checks the annotation per field and
// substitutes "<redacted>" for the value.
//
// Compile + run:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/f && /tmp/f" \
//     posts/06-auto-formatter/examples/formatter_skip.cpp

#include <experimental/meta>
#include <format>
#include <print>
#include <string>
#include <type_traits>

namespace debug_print { struct skip {}; }

template <typename Tag>
consteval bool has_annotation(std::meta::info r) {
    for (auto a : std::meta::annotations_of(r))
        if (std::meta::type_of(a) == ^^Tag) return true;
    return false;
}

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
            if constexpr (has_annotation<debug_print::skip>(m)) {
                out = std::format_to(out, "{}: <redacted>",
                                     std::meta::identifier_of(m));
            } else {
                out = std::format_to(out, "{}: {}",
                                     std::meta::identifier_of(m),
                                     obj.[:m:]);
            }
        }
        return std::format_to(out, "}}");
    }
};

struct User {
    std::string name;
    [[=debug_print::skip{}]] std::string password_hash;
};

int main() {
    User u{"filip", "hash-goes-here"};
    std::println("{}", u);
    // User{name: filip, password_hash: <redacted>}
}

// define_enum_today.cpp  (~60 lines)
// "The next wave of reflection": what P4033 (define_enum) will add, and the
// closest thing that compiles on today's pinned toolchain.
//
// P4033 "Synthesizing enums with define_enum" (R0, 2026-04-13) is one of the
// reflection follow-on papers carried into the C++29 cycle that opened at the
// Brno meeting (June 2026). It will let you GENERATE an enum type at compile
// time from a list of (name, value) pairs -- the synthesis direction. None of
// the June-2026 toolchains (GCC 16.1, clang-p2996) implement it yet, so the
// synthesis block at the bottom is illustrative and kept in a comment.
//
// What DOES compile today is the READ direction: reflect an existing enum and
// derive a name<->value mapping with zero macros. That is the P2996 base these
// papers build on. (Revision numbers and Brno outcomes are provisional until
// the post-Brno mailing, ~2026-07-15.)
//
// Compile (GCC 16.1 or later):     g++-16 -std=c++26 -freflection define_enum_today.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -freflection-latest -stdlib=libc++ define_enum_today.cpp -o demo

#include <experimental/meta>
#include <optional>
#include <print>
#include <string_view>

template <typename E>
consteval auto enumerators_static() {
    return std::define_static_array(std::meta::enumerators_of(^^E));
}

template <typename E>
constexpr std::string_view name_of(E value) {
    template for (constexpr auto e : enumerators_static<E>()) {
        if ([:e:] == value) return std::meta::identifier_of(e);
    }
    return "<unknown>";
}

template <typename E>
constexpr std::optional<E> value_of(std::string_view name) {
    template for (constexpr auto e : enumerators_static<E>()) {
        if (std::meta::identifier_of(e) == name) return E{[:e:]};
    }
    return std::nullopt;
}

enum class HttpStatus { ok = 200, not_found = 404, teapot = 418 };

int main() {
    // READ direction -- works today on the P2996 base:
    std::println("418            -> {}", name_of(HttpStatus::teapot));
    auto v = value_of<HttpStatus>("not_found");
    std::println("\"not_found\"    -> {}", v ? static_cast<int>(*v) : -1);

    // SYNTHESIS direction -- P4033, not yet implemented anywhere in June 2026:
    //
    //   constexpr std::meta::info Status =
    //       std::meta::define_enum(^^int, {
    //           {"ok", 200}, {"not_found", 404}, {"teapot", 418}});
    //   using Generated = [:Status:];   // a brand-new enum type, from data
    //
    // Until define_enum lands, generating an enum FROM data still needs codegen
    // or X-macros; reflection today only walks enums that already exist.
}

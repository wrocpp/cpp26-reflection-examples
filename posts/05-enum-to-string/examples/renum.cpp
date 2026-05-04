// Post 5 — Goodbye magic_enum: enum reflection done right.
// Compile: ./cpp posts/05-enum-to-string/examples/renum.cpp \
//                -o posts/05-enum-to-string/examples/renum
// Run:     ./run posts/05-enum-to-string/examples/renum

#include <experimental/meta>
#include <algorithm>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <string_view>

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

template <typename E>
constexpr std::optional<E> from_string(std::string_view name) {
    template for (constexpr auto e : enumerators_of_static<E>()) {
        if (std::meta::identifier_of(e) == name) return E{[:e:]};
    }
    return std::nullopt;
}

template <typename E>
constexpr std::string to_flags_string(E value) {
    using U = std::underlying_type_t<E>;
    std::string out;
    template for (constexpr auto e : enumerators_of_static<E>()) {
        if ((static_cast<U>(value) & static_cast<U>([:e:])) != 0) {
            if (!out.empty()) out += '|';
            out += std::meta::identifier_of(e);
        }
    }
    return out.empty() ? "0" : out;
}

// Inverse of to_flags_string. Tokenise on '|', look up each token via
// from_string, OR-combine into the underlying type. Returns nullopt
// on empty input or any unknown token -- fail-closed for the trust
// boundary (config files, CLI args, network input).
//
// What we want, written imperatively (kept here to make the ranges
// chain below readable -- the two are equivalent):
//
//   if (s.empty()) return std::nullopt;
//   U combined{};
//   for (auto part : std::views::split(s, '|')) {
//       std::string_view token(part.begin(), part.end());
//       auto match = from_string<E>(token);
//       if (!match) return std::nullopt;     // typo -> fail closed
//       combined |= static_cast<U>(*match);
//   }
//   return E{combined};
//
// Same logic as a ranges chain: fold_left (C++23) accumulates an
// optional<U>; the binary op uses optional's monadic .and_then to gate
// further work on prior success and .transform to combine the matched
// value, so fail-closed falls out of the monad rather than explicit
// early returns. The outer .transform puts the final U back into E.
template <typename E>
constexpr std::optional<E> from_flags_string(std::string_view s) {
    using U = std::underlying_type_t<E>;
    if (s.empty()) return std::nullopt;
    return std::ranges::fold_left(
        std::views::split(s, '|'),
        std::optional<U>{U{}},
        [](std::optional<U> acc, auto part) -> std::optional<U> {
            return acc.and_then([&](U a) -> std::optional<U> {
                return from_string<E>(std::string_view(part.begin(), part.end()))
                    .transform([a](E e) { return a | static_cast<U>(e); });
            });
        }
    ).transform([](U v) { return E{v}; });
}

}  // namespace renum

enum class Color { red, green, blue };
enum class Permission : unsigned { read = 1, write = 2, exec = 4 };

int main() {
    static_assert(renum::to_string(Color::green) == "green");
    static_assert(renum::from_string<Color>("blue") == Color::blue);
    static_assert(renum::from_string<Color>("purple") == std::nullopt);

    std::println("{}", renum::to_string(Color::green));
    auto p = static_cast<Permission>(
        static_cast<unsigned>(Permission::read) | static_cast<unsigned>(Permission::exec));
    std::println("{}", renum::to_flags_string(p));

    // Round-trip: string -> flags -> string.
    auto parsed = renum::from_flags_string<Permission>("read|write");
    std::println("parsed read|write -> {}",
                 parsed ? renum::to_flags_string(*parsed) : std::string{"<error>"});

    // Trust-boundary safety: typo rejected, no surprise zero or partial mask.
    auto bad = renum::from_flags_string<Permission>("read|writ");
    std::println("typo 'writ' -> {}",
                 bad ? renum::to_flags_string(*bad) : std::string{"nullopt"});
}

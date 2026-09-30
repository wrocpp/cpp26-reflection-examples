// enum_aliases.cpp
//
// Two enumerators with the same value, and what C++26 reflection can and
// cannot tell apart. Builds on renum from post 5 (enum-to-string).
//
//   names_of(v)          every enumerator whose value is v (a value is all of them)
//   unique_values<E>()   consteval check that throws std::meta::exception on a clash
//   [[=renum::alias{}]]  annotation (P3394R4) marking the spelling to_string skips
//   name_of<^^E::x>()    a reflection keeps the identity a value has lost
//
// Minted by hand against g162: shorten-examples.py defaults to clang_bb_p2996,
// and this file includes <meta>, which clang-p2996 spells <experimental/meta>.
//
// Compile (GCC 16.2): g++ -std=c++26 -freflection enum_aliases.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++26 -freflection

#include <meta>
#include <cstddef>
#include <cstdlib>
#include <new>
#include <print>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace renum {

// Annotation tag: [[=renum::alias{}]] on an enumerator marks a second spelling.
struct alias {};

template <typename E>
consteval auto enumerators_of_static() {
    return std::define_static_array(std::meta::enumerators_of(^^E));
}

template <typename E>
consteval E value_of(std::meta::info enumerator) {
    return std::meta::extract<E>(std::meta::constant_of(enumerator));
}

consteval bool is_alias(std::meta::info enumerator) {
    return !std::meta::annotations_of_with_type(enumerator, ^^alias).empty();
}

// The names of every enumerator equal to v, as a static array of C strings.
// std::string_view is not structural, so the table holds char const* from
// define_static_string.
template <typename E>
consteval std::span<char const* const> names_with_value(E v) {
    return std::define_static_array(
        std::meta::enumerators_of(^^E)
        | std::views::filter([v](std::meta::info e) { return value_of<E>(e) == v; })
        | std::views::transform([](std::meta::info e) {
              return std::define_static_string(std::meta::identifier_of(e));
          }));
}

template <typename E>
constexpr std::span<char const* const> names_of(E v) {
    template for (constexpr auto e : enumerators_of_static<E>()) {
        if ([:e:] == v) return names_with_value<E>([:e:]);
    }
    return {};
}

// First match, as in post 5: the first declared enumerator with the value wins.
template <typename E>
constexpr std::string_view to_string_first(E v) {
    template for (constexpr auto e : enumerators_of_static<E>()) {
        if ([:e:] == v) return std::meta::identifier_of(e);
    }
    return "<unknown>";
}

template <typename E>
consteval auto canonical_enumerators_of() {
    return std::define_static_array(
        std::meta::enumerators_of(^^E)
        | std::views::filter([](std::meta::info e) { return !is_alias(e); }));
}

// Alias-aware: skip enumerators annotated [[=renum::alias{}]], and fall back to
// the first match only when every spelling of the value is an alias.
template <typename E>
constexpr std::string_view to_string(E v) {
    template for (constexpr auto e : canonical_enumerators_of<E>()) {
        if ([:e:] == v) return std::meta::identifier_of(e);
    }
    return to_string_first(v);
}

// A reflection names one enumerator, so it still knows which spelling you wrote.
template <std::meta::info Enumerator>
constexpr std::string_view name_of() {
    return std::meta::identifier_of(Enumerator);
}

consteval std::string decimal(long long n) {
    constexpr long long base = 10;
    if (n < 0) return "-" + decimal(-n);
    std::string digit(1, static_cast<char>('0' + n % base));
    return n < base ? digit : decimal(n / base) + digit;
}

// Throws on the first value two enumerators share, naming both. Use as
// static_assert(renum::unique_values<E>()) on enums that must not alias.
template <typename E>
consteval bool unique_values() {
    auto const es = std::meta::enumerators_of(^^E);
    for (std::size_t i = 0; i < es.size(); ++i) {
        for (std::size_t j = 0; j < i; ++j) {
            if (value_of<E>(es[i]) == value_of<E>(es[j])) {
                std::string const type{std::meta::identifier_of(^^E)};
                throw std::meta::exception(
                    type + "::" + std::string{std::meta::identifier_of(es[i])}
                        + " duplicates " + type + "::"
                        + std::string{std::meta::identifier_of(es[j])}
                        + " (= " + decimal(std::to_underlying(value_of<E>(es[i]))) + ")",
                    es[i]);
            }
        }
    }
    return true;
}

}  // namespace renum

// Counts heap allocations, to compare a std::string return with a string_view one.
namespace {
std::size_t allocations = 0;
}

void* operator new(std::size_t n) {
    ++allocations;
    if (void* p = std::malloc(n)) return p;
    throw std::bad_alloc{};
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

// The enum from the LinkedIn comment.
enum Color { red = 1, green = 1, blue = 2, some_long_name_enum };

// A renamed enumerator: the old spelling stays for source compatibility.
enum class Level {
    debug,
    info,
    warn [[=renum::alias{}]],
    warning = warn,
    error,
};

enum class Suit { clubs, diamonds, hearts, spades };

static_assert(renum::unique_values<Suit>());
// static_assert(renum::unique_values<Color>());
//   error: uncaught exception of type 'std::meta::exception'; 'what()':
//          'Color::green duplicates Color::red (= 1)'

template <typename E>
void print_names(std::string_view label, E v) {
    std::print("names_of({}) = ", label);
    std::string_view sep;
    for (char const* name : renum::names_of(v)) {
        std::print("{}{}", sep, name);
        sep = "|";
    }
    std::println("");
}

int main() {
    Color c = green;
    print_names("green", c);
    std::println("first match(green) = {}", renum::to_string_first(c));
    std::println("name_of<^^green> = {}", renum::name_of<^^green>());

    print_names("Level::warning", Level::warning);
    std::println("first match(Level::warning) = {}", renum::to_string_first(Level::warning));
    std::println("to_string(Level::warning) = {} (alias-aware)", renum::to_string(Level::warning));

    // identifier_of returns a view of a null-terminated array with static
    // storage duration ([meta.syn] p4), so it can initialize a constexpr view.
    static constexpr std::string_view long_name = renum::to_string(some_long_name_enum);
    static_assert(long_name.data()[long_name.size()] == '\0');

    std::size_t const before = allocations;
    std::string_view const as_view = renum::to_string(some_long_name_enum);
    std::size_t const view_allocs = allocations - before;
    std::string const as_string{renum::to_string(some_long_name_enum)};
    std::size_t const string_allocs = allocations - before - view_allocs;
    std::println("{} ({} chars): string_view {} allocations, std::string {}",
                 as_view, as_string.size(), view_allocs, string_allocs);
}

// captures_and_bindings.cpp
//
// Getting values out of a CTRE match: numbered captures, named captures, and
// structured bindings straight off the result object.
//
// A note on the C++26 spelling. P0963 lets the binding itself be the condition:
//
//     if (auto [whole, y, m, d] = ctre::match<...>(s)) { ... }
//
// That works at run time on both GCC 16.1 and clang 22.1. Inside a constant
// expression it currently works on clang only: GCC rejects the combination of
// a P0963 condition, a tuple-protocol type and constant evaluation. The
// portable spelling keeps the explicit condition, which is what this file uses
// where it matters.
//
// Compile: -std=c++20 -O2, lib ctre
// verify: ce-libs: ctre
// verify: gcc-options: -std=c++20 -O2

#include <ctre.hpp>

#include <cstdio>
#include <optional>
#include <string_view>

using namespace std::string_view_literals;

struct date {
    std::string_view year;
    std::string_view month;
    std::string_view day;
};

// Structured bindings with the explicit condition: portable everywhere,
// including inside constant expressions on both compilers.
constexpr std::optional<date> extract_date(std::string_view s) {
    if (auto [whole, y, m, d] = ctre::match<R"((\d{4})/(\d{1,2})/(\d{1,2}))">(s); whole) {
        return date{y.to_view(), m.to_view(), d.to_view()};
    }
    return std::nullopt;
}

// It really is compile-time: these run before the program does.
static_assert(extract_date("2026/8/22").has_value());
static_assert(!extract_date("not a date").has_value());

int main() {
    // 1. Numbered captures. Index 0 is the whole match.
    if (auto m = ctre::match<"([a-z]+)([0-9]+)">("abc123"sv)) {
        std::printf("whole = %.*s\n", (int)m.get<0>().size(), m.get<0>().data());
        std::printf("  <1> = %.*s\n", (int)m.get<1>().size(), m.get<1>().data());
        std::printf("  <2> = %.*s\n", (int)m.get<2>().size(), m.get<2>().data());
    }

    // 2. Named captures, addressed by the name in the pattern.
    if (auto m = ctre::match<"(?<word>[a-z]+)(?<number>[0-9]+)">("abc123"sv)) {
        std::printf("word   = %.*s\n",
                    (int)m.get<"word">().size(), m.get<"word">().data());
        std::printf("number = %.*s\n",
                    (int)m.get<"number">().size(), m.get<"number">().data());
    }

    // 3. A capture that did not participate converts to false, which is how
    //    you tell "matched empty" from "did not match".
    if (auto m = ctre::match<"([a-z]+)|([0-9]+)">("abc"sv)) {
        std::printf("letters engaged = %d, digits engaged = %d\n",
                    (bool)m.get<1>(), (bool)m.get<2>());
    }

    // 4. The parsed date, via structured bindings.
    if (auto d = extract_date("2026/8/22")) {
        std::printf("date   = %.*s-%.*s-%.*s\n",
                    (int)d->year.size(), d->year.data(),
                    (int)d->month.size(), d->month.data(),
                    (int)d->day.size(), d->day.data());
    }

    return 0;
}

// search_all_split_tokenize.cpp
//
// CTRE's iteration API, as it exists today rather than as the manual
// describes it.
//
// IMPORTANT: the readthedocs page still teaches ctre::range and calls range
// support "preliminary". On current CTRE, ctre::range is DEPRECATED and the
// compiler says so:
//
//     warning: 'ctre::range<...>' is deprecated: use search_all
//
// The docs are also incomplete by their own admission; the regex syntax page
// carries a literal "TODO more detailed regex information". Read the header,
// not the manual.
//
// Compile: -std=c++20 -O2, lib ctre
// verify: ce-libs: ctre
// verify: gcc-options: -std=c++20 -O2

#include <ctre.hpp>

#include <cstdio>
#include <string_view>

using namespace std::string_view_literals;

int main() {
    // search_all: every match of the pattern, in order.
    std::printf("search_all : ");
    for (auto m : ctre::search_all<"([0-9]+)">("123,456,768"sv)) {
        std::printf("[%.*s] ", (int)m.get<1>().size(), m.get<1>().data());
    }
    std::printf("\n");

    // split: the pieces BETWEEN matches of the separator.
    std::printf("split      : ");
    for (auto piece : ctre::split<",">("alpha,beta,gamma"sv)) {
        std::printf("[%.*s] ", (int)piece.size(), piece.data());
    }
    std::printf("\n");

    // tokenize: consumes the input from the front, match after match. It stops
    // at the first position where the pattern does not match, which is what
    // makes it the right tool for a lexer and the wrong one for scanning.
    std::printf("tokenize   : ");
    for (auto t : ctre::tokenize<"[a-z]+|[0-9]+|\\s+">("ab 12 cd"sv)) {
        std::printf("[%.*s] ", (int)t.size(), t.data());
    }
    std::printf("\n");

    // The difference that matters: search_all skips over what it cannot match,
    // tokenize stops there. Same pattern, same input, different results.
    std::printf("search_all over 'ab!!cd' : ");
    for (auto m : ctre::search_all<"[a-z]+">("ab!!cd"sv)) {
        std::printf("[%.*s] ", (int)m.size(), m.data());
    }
    std::printf("\n");

    std::printf("tokenize   over 'ab!!cd' : ");
    for (auto t : ctre::tokenize<"[a-z]+">("ab!!cd"sv)) {
        std::printf("[%.*s] ", (int)t.size(), t.data());
    }
    std::printf("\n");

    return 0;
}

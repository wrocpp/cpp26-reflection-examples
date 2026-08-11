// lexer.cpp
//
// The practical payoff: a tokeniser built from one pattern, with no allocation
// and no runtime regex compilation.
//
// The shape is an alternation where each branch is a capture group. Whichever
// capture engaged tells you the token kind, so the pattern doubles as the token
// table. ctre::tokenize consumes the input from the front and stops at the
// first position nothing matches, which is exactly the behaviour a lexer wants:
// unmatched input is a lexical error, not something to skip past.
//
// Compile: -std=c++20 -O2, lib ctre
// verify: ce-libs: ctre
// verify: gcc-options: -std=c++20 -O2

#include <ctre.hpp>

#include <cstdio>
#include <string_view>

using namespace std::string_view_literals;

enum class kind { ident, number, op, space, unknown };

constexpr const char* name(kind k) {
    switch (k) {
        case kind::ident:  return "ident";
        case kind::number: return "number";
        case kind::op:     return "op";
        case kind::space:  return "space";
        default:           return "unknown";
    }
}

struct token {
    kind k;
    std::string_view text;
};

// One pattern, one capture per token kind.
//
// The dash in the operator class is ESCAPED. PCRE treats a dash at the start or
// end of a character class as a literal; CTRE accepts neither and rejects the
// pattern at compile time, pointing at the exact offset:
//
//     [-+*/=()]   -> problem_at_position<23>   (dash first)
//     [+*/=()-]   -> problem_at_position<30>   (dash last)
//     [+*/=()\-]  -> compiles
constexpr auto kToken =
    ctll::fixed_string{R"(([A-Za-z_]\w*)|(\d+)|([+*/=()\-])|(\s+))"};

template <typename Fn>
constexpr void lex(std::string_view src, Fn&& sink) {
    for (auto m : ctre::tokenize<kToken>(src)) {
        if (m.template get<1>())      { sink(token{kind::ident,  m.template get<1>().to_view()}); }
        else if (m.template get<2>()) { sink(token{kind::number, m.template get<2>().to_view()}); }
        else if (m.template get<3>()) { sink(token{kind::op,     m.template get<3>().to_view()}); }
        else if (m.template get<4>()) { sink(token{kind::space,  m.template get<4>().to_view()}); }
    }
}

int main() {
    constexpr auto src = "total = price * 3 + tax_rate"sv;

    std::printf("input: %.*s\n\n", (int)src.size(), src.data());
    lex(src, [](token t) {
        if (t.k == kind::space) { return; }          // drop whitespace
        std::printf("  %-7s %.*s\n", name(t.k), (int)t.text.size(), t.text.data());
    });

    // Counting tokens is a constant expression, so the lexer runs at compile
    // time as readily as at run time.
    constexpr int counted = [] {
        int n = 0;
        lex("a = 1 + 22"sv, [&n](token t) { if (t.k != kind::space) { ++n; } });
        return n;
    }();
    static_assert(counted == 5);
    std::printf("\ncompile-time token count for 'a = 1 + 22': %d\n", counted);

    // Unmatched input stops the scan rather than being skipped.
    std::printf("stops at bad input: ");
    lex("ok then #bad"sv, [](token t) {
        if (t.k != kind::space) {
            std::printf("[%.*s] ", (int)t.text.size(), t.text.data());
        }
    });
    std::printf("\n");
    return 0;
}

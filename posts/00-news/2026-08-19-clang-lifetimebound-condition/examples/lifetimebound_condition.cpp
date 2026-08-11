// lifetimebound_condition.cpp  (~40 lines)
//
// [[clang::lifetimebound]] tells the compiler that the result borrows from a
// parameter, which lets -Wdangling see through a function call. It came from
// WG21 paper P0936R0, was never standardised, and ships today as a vendor
// extension (clang 7+, and MSVC 17.7+ spells it [[msvc::lifetimebound]]).
//
// The attribute is unconditional, and that is the gap under discussion on the
// LLVM forum: for a function whose return type is sometimes a reference and
// sometimes a value, there is no way to say "bound only when I return a
// reference". Annotate it and you warn on correct code; leave it off and you
// miss the real dangle.
//
// Case A below is a genuine dangle the attribute correctly catches.
// Case B is a FALSE POSITIVE: value_or returns by value, so nothing can
// dangle, yet the warning fires anyway. The program runs clean under
// AddressSanitizer to prove it.
//
// Compile (clang): clang++ -std=c++23 -Wall -Wdangling -fsanitize=address
// verify: clang-only
// verify: clang-options: -std=c++23 -Wall -Wdangling -fsanitize=address

#include <cstdio>
#include <string>
#include <string_view>

std::string_view first_word(const std::string& s [[clang::lifetimebound]]) {
    return std::string_view(s).substr(0, s.find(' '));
}

struct OptString {
    bool has;
    std::string val;

    // Returns BY VALUE. The result is a copy, so it can never point into
    // `fallback` -- but the attribute cannot be made conditional.
    std::string value_or(const std::string& fallback [[clang::lifetimebound]]) const {
        return has ? val : fallback;
    }
};

int main() {
    std::string text = "hello world";
    std::string_view good = first_word(text);          // fine: text outlives good
    std::printf("first_word     -> %.*s\n", (int)good.size(), good.data());

    // Case A: a real dangle. clang is right to warn. We never read `bad`,
    // so creating it is well defined and the program stays clean.
    std::string_view bad = first_word(std::string("temporary here"));
    (void)bad;

    // Case B: the false positive. -Wdangling fires on this line, yet the
    // value is a copy and printing it is perfectly correct.
    OptString opt{false, {}};
    std::string copied = opt.value_or(std::string("fallback"));
    std::printf("value_or       -> %s\n", copied.c_str());
    std::printf("no dangle here: the return type is std::string, not a reference\n");
    return 0;
}

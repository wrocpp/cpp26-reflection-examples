// postfix_default.cpp  (~40 lines)
// "C++29 quality-of-life": defaultable postfix ++ / -- -- P3668, "Defaulting
// Postfix Increment and Decrement Operations", adopted for C++29 at the Brno
// meeting (June 2026).
//
// Every postfix operator has the same canonical body: copy *this, call the
// prefix operator, return the copy. P3668 lets you = default it. Not on the
// June-2026 toolchains yet, so the C++29 form is kept in a comment; what
// compiles TODAY is the hand-written postfix it replaces.
//
// Compile (GCC 16.1 or later):     g++-16 -std=c++26 postfix_default.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ postfix_default.cpp -o demo

#include <print>

struct Counter {
    int n = 0;
    constexpr Counter& operator++() { ++n; return *this; }  // you write prefix

    // TODAY: the canonical postfix body, by hand (everyone gets it subtly wrong once).
    constexpr Counter operator++(int) { Counter copy = *this; ++*this; return copy; }

    // C++29 -- P3668, not yet implemented anywhere in June 2026:
    //
    //   constexpr Counter operator++(int) = default;   // synthesizes exactly that
};

int main() {
    Counter c;
    Counter old = c++;            // postfix returns the pre-increment value
    std::println("old = {}, now = {}", old.n, c.n);
}

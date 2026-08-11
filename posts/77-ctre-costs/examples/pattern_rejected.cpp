// pattern_rejected.cpp  -- THIS FILE IS MEANT TO FAIL TO COMPILE.
//
// CTRE validates the pattern while compiling, so a malformed regex is a build
// error rather than a runtime throw. The diagnostic carries the offset:
//
//     error: invalid use of incomplete type 'struct ctre::problem_at_position<23>'
//
// Position 23 is the dash. PCRE accepts a dash at the start or the end of a
// character class as a literal; CTRE accepts neither, and wants it escaped:
//
//     [-+*/=()]   -> problem_at_position<23>   (dash first)
//     [+*/=()-]   -> problem_at_position<30>   (dash last)
//     [+*/=()\-]  -> compiles
//
// The offset is genuinely useful. The wrapper around it is template noise, and
// it is exactly what C++26 P2741 (user-generated static_assert messages) exists
// to let a library replace with a sentence.
//
// Compile: -std=c++20, lib ctre  -- expected to FAIL
// verify: ce-libs: ctre
// verify: gcc-options: -std=c++20 -O1

#include <ctre.hpp>
#include <string_view>

using namespace std::string_view_literals;

int main() {
    // Dash first in the character class: rejected at compile time.
    auto m = ctre::match<R"(([A-Za-z_]\w*)|(\d+)|([-+*/=()])|(\s+))">("total"sv);
    return m ? 0 : 1;
}

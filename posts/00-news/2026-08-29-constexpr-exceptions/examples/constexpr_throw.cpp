// constexpr_throw.cpp  (~19 lines)
// Before C++26, reaching a `throw` during constant evaluation made the
// expression non-constant, so constexpr code had to report failure some other
// way. C++26 (P3068) allows throwing and catching inside a constant
// evaluation: only an exception that ESCAPES the evaluation fails the build.
// The second static_assert below is the interesting one -- an exception is
// thrown and caught entirely at compile time.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 constexpr_throw.cpp

#include <print>
#include <stdexcept>

constexpr int checked_div(int a, int b) {
    if (b == 0) throw std::domain_error("divide by zero");
    return a / b;
}

constexpr int try_div(int a, int b) {
    try { return checked_div(a, b); }
    catch (const std::domain_error&) { return -1; }
}

int main() {
    static_assert(try_div(10, 2) == 5);
    static_assert(try_div(10, 0) == -1);  // thrown AND caught during constant evaluation
    std::println("try_div(10,2) = {}", try_div(10, 2));
    std::println("try_div(10,0) = {}", try_div(10, 0));
}

// contracts_basics.cpp  (~26 lines)
// C++26 contracts (P2900): pre and post attach to the function declaration,
// contract_assert goes in the body. They are core language, so no header is
// needed, but GCC 16.1 still requires -fcontracts to enable them.
//
// The const on the by-value parameters is not decoration. A postcondition may
// only name a parameter the body cannot have modified, and the compiler
// enforces it: drop the const and you get
//   error: a value parameter used in a postcondition must be const
//
// Compile (GCC 16.1): g++ -std=c++26 -fcontracts -O2 contracts_basics.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++26 -fcontracts -O2

#include <cstdio>

int divide(const int a, const int b)
    pre (b != 0)                        // precondition
    post (r: b == 1 ? r == a : true)    // postcondition; r names the result
{
    contract_assert(b != 0);            // assertion inside the body
    return a / b;
}

// No postcondition here, so n does not have to be const and the body may
// modify it freely.
int shrink(int n)
    pre (n > 0)
{
    n = n / 2;
    return n;
}

int main() {
    std::printf("10 / 2 = %d\n", divide(10, 2));
    std::printf("7 / 1  = %d\n", divide(7, 1));
    std::printf("shrink(9) = %d\n", shrink(9));
    std::printf("all contracts held\n");
    return 0;
}

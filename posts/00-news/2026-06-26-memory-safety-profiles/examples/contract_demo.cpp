// contract_demo.cpp  (~45 lines)
// verify: gcc-only  -- the local clang-p2996 container has no P2900 support;
// this file is verified on Compiler Explorer's GCC 16.1 instead (contracts
// need -fcontracts, set via the gcc-options marker below).
// verify: gcc-options: -std=c++26 -fcontracts
//
// A real C++26 Contracts (P2900) demo that runs TODAY on mainline GCC 16.1.
// Contracts shipped in C++26; P3097 "Contracts for virtual functions" (R2,
// 2026-05-11) extends them and is part of the C++29 cycle opened at Brno.
//
// Compile + run (GCC 16.1):
//   g++-16 -std=c++26 -fcontracts contract_demo.cpp -o contract_demo && ./contract_demo
//
// The default contract evaluation semantic aborts on a violated contract, so
// the commented-out call at the bottom would terminate the program with a
// contract-violation diagnostic rather than returning a bad value.

#include <climits>
#include <print>

// Precondition(s) + named-result postcondition on a free function. With P2900
// these are part of the function's interface, checkable at the call boundary.
// Value parameters named in a postcondition must be const (P2900): the post
// is evaluated after the body, so the language pins the values it may read.
int saturating_add(const int a, const int b)
    pre (a >= 0)
    pre (b >= 0)
    post (r: r >= a && r >= b)
{
    long sum = static_cast<long>(a) + b;
    contract_assert(sum >= 0);            // mid-body assertion
    return sum > INT_MAX ? INT_MAX : static_cast<int>(sum);
}

// P3097 (C++29) will let a contract on a virtual function be inherited and
// checked through a base pointer -- not expressible on GCC 16.1 today:
//
//   struct Codec {
//       virtual int decode(int n) pre (n >= 0) = 0;   // P3097
//   };

int main() {
    std::println("saturating_add(2, 3)                     = {}",
                 saturating_add(2, 3));
    std::println("saturating_add(2000000000, 2000000000)   = {}",
                 saturating_add(2000000000, 2000000000));

    // saturating_add(-1, 0);  // violates pre(a >= 0): contract fires, aborts
}

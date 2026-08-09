// contracts_violation.cpp  (~16 lines)
// What a contract violation actually does depends on the evaluation semantic
// you compile with, not on the source. The same binary source behaves three
// different ways:
//
//   -fcontract-evaluation-semantic=ignore    no check at all
//   -fcontract-evaluation-semantic=observe   report and keep running
//   -fcontract-evaluation-semantic=enforce   report and terminate (default)
//
// Built with `observe` below so it can report the violation and still exit 0.
// The body is guarded so the program stays well defined either way: a broken
// precondition means the CALLER has a bug, and the callee still has to cope.
//
// Compile (GCC 16.1):
//   g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe -O2 contracts_violation.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe -O2

#include <cstdio>

int divide(const int a, const int b)
    pre (b != 0)
{
    return b == 0 ? 0 : a / b;
}

int main() {
    std::printf("10 / 2 = %d\n", divide(10, 2));
    std::fflush(stdout);
    std::printf("10 / 0 = %d\n", divide(10, 0));   // violates the precondition
    std::printf("still running after the violation\n");
    return 0;
}

// erroneous_behavior.cpp  (~30 lines)
// For 40 years, reading an uninitialized variable was UNDEFINED behavior: the
// program could do anything, and optimizers exploited that freedom (deleting
// branches, "time-travel" miscompiles). C++26 (P2795) reclassifies it as
// ERRONEOUS behavior: still a bug the compiler is encouraged to diagnose, but
// with defined, bounded, non-exploitable semantics. The value read is a fixed
// erroneous value (implementations pick a pattern; GCC uses 0), NOT the
// indeterminate garbage of before.
//
// This program deliberately dirties the stack, then reads an uninitialized int.
// Built as C++23 it printed whatever was left on the stack (and it was UB);
// built as C++26 it prints a defined 0 -- deterministically -- and GCC still
// warns (-Wuninitialized). UBSan stays silent, because this is no longer UB.
//
// Compile (GCC 16.1):  g++ -std=c++26 -O2 -Wall erroneous_behavior.cpp

#include <print>

[[gnu::noinline]] void poison() {
    volatile int junk[8];
    for (int i = 0; i < 8; ++i) junk[i] = 0xDEADBEEF;   // dirty this stack frame
}

[[gnu::noinline]] int read_uninitialized() {
    int x;                     // C++26: erroneous behavior, not undefined
    return x;                  // GCC warns here; the value is a defined 0
}

int main() {
    poison();
    // Under C++23 this line printed leftover garbage. Under C++26 it is 0, every run.
    std::println("built as C++{}", __cplusplus >= 202400L ? 26 : 23);
    std::println("erroneous value = {}", read_uninitialized());
}

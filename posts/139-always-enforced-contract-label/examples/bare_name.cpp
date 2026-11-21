// verify: ce-only
// The bare name does not compile: nothing makes `enforced` visible to ordinary name lookup here (P4451R2 section 2.5).
// This file fails to build on purpose. The other spellings are in name_lookup.cpp, which builds and exits 0.
//
// Build (Compiler Explorer, GCC and Clang with P4451 support): -std=c++26 -fcontracts-p4451
#include <contracts>

int positive(const int x) pre<enforced>(x > 0) { return x; }

int main() { return positive(1) - 1; }

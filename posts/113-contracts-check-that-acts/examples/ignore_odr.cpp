// verify: gcc-only
// What the ignore semantic leaves in place: the predicate is parsed and its names are odr-used, so a
// function it names needs a definition even though the predicate is never evaluated (P2900R14 3.5.4).
// The default build defines the predicate and links under every semantic. -DNO_DEFINITION removes the
// definition: GCC then links under ignore and fails to link under observe, enforce and quick_enforce.
// Build: g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe [-DNO_DEFINITION] ignore_odr.cpp
//        (Compiler Explorer: g162)
#include <cstdio>

namespace {

constexpr int kInput = 8;

bool isEven(int value);

#ifndef NO_DEFINITION
bool isEven(int value) { return value % 2 == 0; }
#endif

int half(const int value) pre(isEven(value)) { return value / 2; }

}  // namespace

int main() {
  std::printf("half(%d) = %d\n", kInput, half(kInput));
}

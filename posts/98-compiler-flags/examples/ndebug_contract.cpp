// verify: gcc-only
// Compiler flags, part 2: what -DNDEBUG removes. assert() is a macro, so NDEBUG deletes it. contract_assert
// is a language feature, so NDEBUG does not touch it; its evaluation semantic does. The condition reads a
// global, so the optimizer cannot fold it away. Output is flushed after each line because a terminating
// contract violation otherwise loses buffered stdout.
// Build: g++-16 -std=c++26 -fcontracts -O2 [-DNDEBUG] [-fcontract-evaluation-semantic=ignore|observe|enforce]
//        (Compiler Explorer: g162)
#include <cassert>
#include <contracts>
#include <cstdio>

int g_zero = 0;  // not a constant: the compiler cannot decide the checks below at compile time

static void say(const char* line) {
  std::fputs(line, stdout);
  std::fputc('\n', stdout);
  std::fflush(stdout);
}

int main(int argc, char**) {
  const int x = argc - 1 + g_zero;  // 0 when run without arguments, so both checks below are false
  say("before assert");
  assert(x != 0);            // a macro: -DNDEBUG deletes it
  say("after assert");
  contract_assert(x != 0);   // a language feature: -DNDEBUG does not touch it
  say("after contract_assert");
}

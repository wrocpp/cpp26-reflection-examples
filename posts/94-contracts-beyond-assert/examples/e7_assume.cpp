// E7: [[assume]] is the thing a postcondition is not. It is a promise the compiler believes and does not check:
// when it is false, the program has undefined behaviour. Here the "defensive" test after the assume is removed,
// so a call that breaks the promise returns a value the test was meant to rule out.
#include <cstdio>

[[gnu::noinline]] int checked(int x) {
  [[assume(x > 0)]];
  return x <= 0 ? -1 : x;  // the compiler may treat this test as always false
}

int main(int argc, char**) {
  std::printf("%d\n", checked(1 - argc));  // argc is 1 or more, so x is 0 or less and the promise is false
  return 0;
}

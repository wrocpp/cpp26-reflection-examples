// verify: gcc-only
// A by-value parameter named in a postcondition must be const on every declaration (P2900R14 3.4.4).
// The default build has const on both declarations and runs. -DSHOW_ERROR=n builds one way of breaking the rule:
//   1  a non-const by-value parameter named in the postcondition
//   2  const on the first declaration, not on the definition
//   3  const on the first declaration, a later redeclaration without a postcondition drops it
//   4  an array parameter named in the postcondition
//   5  a coroutine definition; the paper calls this ill-formed, GCC 16.2 and trunk accept it
// Build: g++ -std=c++26 -fcontracts by_value_post.cpp   (Compiler Explorer: g162)
#include <coroutine>
#include <cstdio>

#ifndef SHOW_ERROR
#define SHOW_ERROR 0
#endif

namespace {

constexpr int kInput = 4;

#if SHOW_ERROR == 0
int twice(const int value) post(result : result == 2 * value);
int twice(const int value) { return value * 2; }
#elif SHOW_ERROR == 1
int twice(int value) post(result : result == 2 * value) { return value * 2; }
#elif SHOW_ERROR == 2
int twice(const int value) post(result : result == 2 * value);
int twice(int value) { return value * 2; }
#elif SHOW_ERROR == 3
int twice(const int value) post(result : result == 2 * value);
int twice(const int value) { return value * 2; }
int twice(int value);
#elif SHOW_ERROR == 4
int twice(const int values[]) post(result : result == 2 * values[0]) { return values[0] * 2; }
#elif SHOW_ERROR == 5
struct Task {
  struct promise_type {
    Task get_return_object() { return {}; }
    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
  };
};
Task twice(const int value) post(value > 0) { co_return; }
#endif

}  // namespace

int main() {
#if SHOW_ERROR == 4
  const int values[] = {kInput};
  std::printf("twice(%d) = %d\n", kInput, twice(values));
#elif SHOW_ERROR == 5
  twice(kInput);
  std::puts("coroutine ran");
#else
  std::printf("twice(%d) = %d\n", kInput, twice(kInput));
#endif
}

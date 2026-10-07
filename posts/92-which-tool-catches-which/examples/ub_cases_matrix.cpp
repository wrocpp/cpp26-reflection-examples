// One undefined statement per run, to be built with each tool in the matrix.
// Not embedded: most of these runs end in a trap, an abort or a sanitizer report on purpose.
// Build: g++ -std=c++26 -Wall -Wextra -DCASE=<n> [-DGUARD] [-O0|-O2] [tool flags] ub_cases_matrix.cpp [-lstdc++exp]
//   CASE=1  std::vector<int> index one past the end
//   CASE=2  built-in array index one past the end
//   CASE=3  signed INT_MAX + 1
//   CASE=4  integer 10 / 0
//   CASE=5  dereference of a null int*
//   CASE=6  dereference of an empty std::unique_ptr<int>
//   CASE=7  read of an uninitialised local int (C++26: erroneous behaviour)
//   CASE=8  the same read with [[indeterminate]] (undefined behaviour again)
//   -DGUARD puts a contract_assert on the line before the statement (cases 1 to 6).
//   -DCONSTEVAL_CHECK=<n> instead evaluates case n inside a static_assert (cases 1 to 6).
#include <climits>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <vector>

#ifndef CASE
#define CASE 1
#endif

[[gnu::noinline]] int divide(int a, int b) { return a / b; }

#ifdef CONSTEVAL_CHECK
constexpr int vec_index(std::size_t i) { std::vector<int> v{10, 20, 30}; return v[i]; }
constexpr int arr_index(std::size_t i) { int a[3] = {10, 20, 30}; return a[i]; }
constexpr int add_one(int x) { return x + 1; }
constexpr int div_by(int a, int b) { return a / b; }
constexpr int deref(const int* p) { return *p; }
constexpr int deref_up() { std::unique_ptr<int> up; return *up; }
#if CONSTEVAL_CHECK == 1
static_assert(vec_index(3) == 0);
#elif CONSTEVAL_CHECK == 2
static_assert(arr_index(3) == 0);
#elif CONSTEVAL_CHECK == 3
static_assert(add_one(INT_MAX) == 0);
#elif CONSTEVAL_CHECK == 4
static_assert(div_by(10, 0) == 0);
#elif CONSTEVAL_CHECK == 5
static_assert(deref(nullptr) == 0);
#elif CONSTEVAL_CHECK == 6
static_assert(deref_up() == 0);
#elif CONSTEVAL_CHECK == 7
constexpr int uninit() { int u; return u; }
static_assert(uninit() == 0);
#endif
int main() {}
#else
int main() {
  std::setvbuf(stdout, nullptr, _IONBF, 0);
  [[maybe_unused]] volatile std::size_t vi = 3;
  [[maybe_unused]] volatile int vmax = INT_MAX;
  [[maybe_unused]] volatile int vzero = 0;
  [[maybe_unused]] int* volatile vnull = nullptr;
#if CASE == 1
  std::vector<int> v{10, 20, 30};
  std::size_t i = vi;
#ifdef GUARD
  contract_assert(i < v.size());
#endif
  std::printf("v[3] = %d\n", v[i]);
#elif CASE == 2
  int a[3] = {10, 20, 30};
  std::size_t i = vi;
#ifdef GUARD
  contract_assert(i < 3);
#endif
  std::printf("a[3] = %d\n", a[i]);
#elif CASE == 3
  int x = vmax;
#ifdef GUARD
  contract_assert(x < INT_MAX);
#endif
  std::printf("INT_MAX + 1 = %d\n", x + 1);
#elif CASE == 4
  int d = vzero;
#ifdef GUARD
  contract_assert(d != 0);
#endif
  std::printf("10 / 0 = %d\n", divide(10, d));
#elif CASE == 5
  int* p = vnull;
#ifdef GUARD
  contract_assert(p != nullptr);
#endif
  std::printf("*p = %d\n", *p);
#elif CASE == 6
  std::unique_ptr<int> up(vnull);
#ifdef GUARD
  contract_assert(up != nullptr);
#endif
  std::printf("*up = %d\n", *up);
#elif CASE == 7
  int u;
  std::printf("u = %d\n", u);
#elif CASE == 8
  [[indeterminate]] int u;
  std::printf("u = %d\n", u);
#endif
  std::puts("continued");
  return 0;
}
#endif

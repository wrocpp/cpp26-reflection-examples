// A constexpr test suite for four kinds of undefined behavior, and the same
// functions called at run time. Build the checked suite as-is; build with
// -DCASE=N (1..6) to add one failing static_assert and read the diagnostic.
// Run with no argument, or with the argument div or null.
#include <array>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <span>
#include <string_view>
#include <vector>

constexpr int array_at(std::size_t i) {
    std::array<int, 3> a{10, 20, 30};
    return a[i];
}
constexpr int vector_at(std::size_t i) {
    std::vector<int> v{10, 20, 30};
    return v[i];
}
constexpr int span_at(std::size_t i) {
    std::array<int, 3> a{10, 20, 30};
    std::span<int> s{a};
    return s[i];
}
constexpr int add_one(int x) { return x + 1; }
constexpr int divide(int a, int b) { return a / b; }
constexpr int deref(const int* p) { return *p; }

// The suite: every line here must hold at compile time.
static_assert(array_at(2) == 30);
static_assert(vector_at(2) == 30);
static_assert(span_at(2) == 30);
static_assert(add_one(41) == 42);
static_assert(divide(84, 2) == 42);
static_assert([] { int x = 42; return deref(&x); }() == 42);

// One bad call per kind of UB. Each one stops the build.
#if CASE == 1
static_assert(array_at(3) == 0);
#elif CASE == 2
static_assert(vector_at(3) == 0);
#elif CASE == 3
static_assert(span_at(3) == 0);
#elif CASE == 4
static_assert(add_one(INT_MAX) == 0);
#elif CASE == 5
static_assert(divide(1, 0) == 0);
#elif CASE == 6
static_assert(deref(nullptr) == 0);
#endif

int main(int argc, char** argv) {
    // The same functions with values the compiler cannot see. No argument:
    // index and overflow. Argument "div" or "null": the other two.
    const std::string_view mode = argc > 1 ? argv[1] : "index";
    if (mode == "div") {
        std::printf("divide(1, 0) = %d\n", divide(1, argc - 2));  // argc is 2, so the divisor is 0
    } else if (mode == "null") {
        const int* p = argc > 2 ? &argc : nullptr;  // nullptr when run with one argument
        std::printf("deref(nullptr) = %d\n", deref(p));
    } else {
        const std::size_t i = static_cast<std::size_t>(argc) + 2;  // 3 when run without arguments
        std::printf("array_at(%zu)  = %d\n", i, array_at(i));
        std::printf("vector_at(%zu) = %d\n", i, vector_at(i));
        std::printf("span_at(%zu)   = %d\n", i, span_at(i));
        const int big = INT_MAX - 1 + argc;  // INT_MAX when run without arguments
        std::printf("add_one(INT_MAX) > INT_MAX: %s\n", add_one(big) > big ? "true" : "false");
    }
}

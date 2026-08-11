// p0963_tuple_constexpr.cpp  (~20 lines of substance)
//
// C++26 P0963R3 lets a structured binding declaration BE the condition:
//
//     if (auto [a, b] = f()) { ... }
//
// The condition is operator bool() on the underlying object. GCC 16.1 and
// clang 22.1 both implement it, and both agree on plain structs. They diverge
// on one specific combination:
//
//     P0963 condition  +  tuple protocol (tuple_size/get)  +  constant evaluation
//
// Remove any one of the three and GCC is happy. Keep all three and GCC 16.1
// rejects the program:
//
//     error: accessing '<anonymous>' outside its lifetime
//
// clang 22.1 compiles and runs it. This file is the reduced reproducer; no
// third-party library is involved. It was found via CTRE, whose regex_results
// decomposes through the tuple protocol.
//
// Compile (clang, works):  clang++ -std=c++26 p0963_tuple_constexpr.cpp
// Compile (gcc, fails):    g++     -std=c++26 p0963_tuple_constexpr.cpp
// verify: clang-only
// verify: clang-options: -std=c++26 -O2

#include <cstddef>
#include <cstdio>
#include <tuple>

struct Result {
    int a;
    int b;
    bool ok;

    constexpr explicit operator bool() const { return ok; }

    // The tuple protocol is what makes the difference. Bind Result
    // member-wise instead and GCC accepts the same code.
    template <std::size_t I> constexpr int get() const { return I == 0 ? a : b; }
};

template <> struct std::tuple_size<Result> : std::integral_constant<std::size_t, 2> {};
template <std::size_t I> struct std::tuple_element<I, Result> { using type = int; };

constexpr int sum_if_ok(int v) {
    if (auto [a, b] = Result{v, v * 2, v != 0}) {
        return a + b;
    }
    return -1;
}

// GCC 16.1 rejects these two lines. clang 22.1 accepts them.
static_assert(sum_if_ok(1) == 3);
static_assert(sum_if_ok(0) == -1);

int main() {
    std::printf("sum_if_ok(1) = %d\n", sum_if_ok(1));
    std::printf("sum_if_ok(0) = %d\n", sum_if_ok(0));
    std::printf("evaluated at compile time on clang, rejected by GCC 16.1\n");
    return 0;
}

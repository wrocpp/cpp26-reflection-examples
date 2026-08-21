// void_t_shim.cpp
//
// What the C++14 compatibility shims were for, and why deleting them is a
// real change rather than a tidy-up.
//
// The detection idiom needs void_t. It arrived in the standard library in
// C++17, so every library that wanted it before then shipped its own:
// absl::void_t, boost::void_t, and a lot of private copies.
//
// Abseil LTS 20260817.0 (2026-08-18) deprecates absl::void_t and tells you to
// use std::void_t directly. Boost 1.92.0 (2026-08-12) says Heap and Lockfree
// will require C++17 from the next release. Both are deleting shims that only
// ever existed to serve pre-C++17 users.
//
// The catch is that the obvious hand-rolled shim did not work. This file shows
// all three spellings side by side and prints which of them actually detects.
//
// Compile: g++ -std=c++17 void_t_shim.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++17 -O1

#include <cstdio>
#include <type_traits>
#include <utility>
#include <vector>
#include <memory>

// ---------------------------------------------------------------------------
// 1. The standard one, C++17 onwards. Swap -std=c++17 for -std=c++14 and this
//    line is where the build stops: 'void_t' is not a member of 'std'.
// ---------------------------------------------------------------------------
template <typename, typename = std::void_t<>>
struct has_reserve_std : std::false_type {};

template <typename T>
struct has_reserve_std<T, std::void_t<decltype(std::declval<T&>().reserve(0u))>>
    : std::true_type {};

// ---------------------------------------------------------------------------
// 2. The obvious C++14 shim. This is the one people wrote first, and on a
//    strict pre-CWG-1558 reading it does not work: unused parameters in an
//    alias template were not required to participate in substitution, so the
//    partial specialization could collapse to void_t<> and match everything.
//
//    GCC and Clang both implement the resolution, so today this behaves. That
//    is exactly the problem with shims: they encode a compiler landscape from
//    the year they were written.
// ---------------------------------------------------------------------------
template <typename...>
using naive_void_t = void;

template <typename, typename = naive_void_t<>>
struct has_reserve_naive : std::false_type {};

template <typename T>
struct has_reserve_naive<T, naive_void_t<decltype(std::declval<T&>().reserve(0u))>>
    : std::true_type {};

// ---------------------------------------------------------------------------
// 3. The shim libraries actually shipped. Routing through a class template
//    forces the substitution the alias template was allowed to skip. This is
//    the shape absl::void_t and boost::void_t have carried for years, and it
//    is what the deprecation is retiring.
// ---------------------------------------------------------------------------
template <typename...>
struct make_void { using type = void; };

template <typename... Ts>
using shim_void_t = typename make_void<Ts...>::type;

template <typename, typename = shim_void_t<>>
struct has_reserve_shim : std::false_type {};

template <typename T>
struct has_reserve_shim<T, shim_void_t<decltype(std::declval<T&>().reserve(0u))>>
    : std::true_type {};

// ---------------------------------------------------------------------------

struct NoReserve { int x; };

int main() {
    std::printf("detecting .reserve(size_t)\n\n");
    std::printf("%-22s %-14s %-14s %-14s\n", "", "std::void_t", "naive alias", "make_void shim");

    std::printf("%-22s %-14s %-14s %-14s\n", "std::vector<int>",
                has_reserve_std<std::vector<int>>::value   ? "yes" : "no",
                has_reserve_naive<std::vector<int>>::value ? "yes" : "no",
                has_reserve_shim<std::vector<int>>::value  ? "yes" : "no");

    std::printf("%-22s %-14s %-14s %-14s\n", "NoReserve",
                has_reserve_std<NoReserve>::value   ? "yes" : "no",
                has_reserve_naive<NoReserve>::value ? "yes" : "no",
                has_reserve_shim<NoReserve>::value  ? "yes" : "no");

    std::printf("%-22s %-14s %-14s %-14s\n", "std::unique_ptr<int>",
                has_reserve_std<std::unique_ptr<int>>::value   ? "yes" : "no",
                has_reserve_naive<std::unique_ptr<int>>::value ? "yes" : "no",
                has_reserve_shim<std::unique_ptr<int>>::value  ? "yes" : "no");

    // All three agree on this compiler. The shim existed for the compilers
    // where they did not.
    static_assert(has_reserve_std<std::vector<int>>::value, "vector reserves");
    static_assert(!has_reserve_std<NoReserve>::value, "plain struct does not");

    std::printf("\nthe standard spelling needs C++17; the other two exist because it did not\n");
    return 0;
}

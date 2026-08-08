// compile_time_array.cpp  (~22 lines)
// Every line of build() runs during compilation. That was not always possible:
// std::array's own members have been constexpr since C++17, but the ALGORITHMS
// caught up later. std::iota and std::sort (and the ranges versions) only
// became constexpr in C++20, which is what makes this function usable in a
// constant expression today.
//
// The static_asserts are the proof: if any part of build() escaped to runtime,
// they would not compile.
//
// Compile (GCC 16.1): g++ -std=c++23 -O2 compile_time_array.cpp

#include <algorithm>
#include <array>
#include <numeric>
#include <print>

constexpr auto build() {
    std::array<int, 8> a{};
    std::iota(a.begin(), a.end(), 1);        // constexpr since C++20
    for (auto& x : a) x = x * x;
    std::ranges::sort(a, std::greater{});    // constexpr since C++20
    return a;
}

int main() {
    constexpr auto a = build();
    static_assert(a.front() == 64 && a.back() == 1);
    static_assert(std::accumulate(a.begin(), a.end(), 0) == 204);
    std::println("{}", a);
    std::println("sum = {}", std::accumulate(a.begin(), a.end(), 0));
}

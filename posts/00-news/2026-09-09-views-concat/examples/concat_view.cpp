// concat_view.cpp  (~17 lines)
// std::views::concat (P2542, C++26) presents several ranges as ONE sequence
// without copying any of them. Unlike views::join, which flattens a range OF
// ranges (all the same type), concat takes the ranges as separate arguments and
// they may be different types, as long as their elements share a common
// reference type. The result composes with every other adaptor, and stays
// sized/random-access when all the inputs are.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 concat_view.cpp

#include <array>
#include <print>
#include <ranges>
#include <vector>

int main() {
    std::vector<int> a{1, 2, 3};
    std::array<int, 2> b{4, 5};       // a different type entirely
    std::vector<int> c{6};

    auto all = std::views::concat(a, b, c);   // one view, three containers, no copy
    std::println("concat  : {}", std::vector(std::from_range, all));
    std::println("size    : {}", std::ranges::size(all));
    std::println("doubled : {}", std::vector(std::from_range,
                 all | std::views::transform([](int x) { return x * 2; })));
}

// callback.cpp  (~24 lines)
// C++26's std::function_ref (P0792) is a non-owning, non-allocating callable
// reference: the right parameter type for "call this back, I will not store it."
// One function_ref<int(int)> binds to a capturing lambda, a plain function, or
// anything else with the signature, with no copy of the callable and no heap
// allocation. std::function would own and possibly allocate; function_ref just
// borrows, the way string_view borrows a string.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 callback.cpp

#include <functional>
#include <print>

int transform_sum(std::function_ref<int(int)> f) {
    int total = 0;
    for (int x : {1, 2, 3, 4}) total += f(x);
    return total;
}

int square(int x) { return x * x; }

int main() {
    int bias = 10;
    std::println("capturing lambda -> {}", transform_sum([bias](int x) { return x + bias; }));
    std::println("function pointer -> {}", transform_sum(square));
    return 0;
}

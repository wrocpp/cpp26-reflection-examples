// fixed_capacity.cpp  (~22 lines)
// C++26's std::inplace_vector<T, N> (P0843) is a sequence container with a
// compile-time capacity and its storage held inline, in the object itself.
// It has a vector's dynamic size and push_back, with zero heap allocation:
// the elements live where the inplace_vector lives (stack, static, or inside
// another object). Past capacity, push_back throws; try_push_back reports the
// failure instead, so the full case is a branch rather than an exception.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 fixed_capacity.cpp

#include <inplace_vector>
#include <print>

int main() {
    std::inplace_vector<int, 4> v;  // capacity 4, no heap: storage is inline
    for (int i = 1; i <= 4; ++i) v.push_back(i * 10);

    std::println("size={} capacity={} elements={}", v.size(), v.capacity(), v);

    auto slot = v.try_push_back(50);  // already full: no room left
    std::println("try_push_back(50) -> {}", slot ? "stored" : "rejected (at capacity)");
    return 0;
}

// compile_time_value.cpp  (~18 lines)
// A template parameter is a compile-time value you cannot pass as an argument;
// a function argument is a runtime value you cannot use as a template
// parameter. C++26's std::constant_wrapper (P2781) bridges the two: it is an
// empty object that CARRIES a compile-time value, so you can pass it around
// like an ordinary argument and still use it where a constant is required.
//
// Arithmetic on constant_wrappers yields constant_wrappers, so the value stays
// available to static_assert and to array extents. GCC 16.1 ships it
// (__cpp_lib_constant_wrapper = 202603).
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 compile_time_value.cpp

#include <array>
#include <cstdio>
#include <type_traits>

int main() {
    auto a = std::constant_wrapper<5>{};
    auto b = std::constant_wrapper<3>{};
    auto c = a + b;                             // still a compile-time constant
    static_assert(decltype(c)::value == 8);

    std::array<int, decltype(c)::value> arr{};  // usable as an array extent
    std::printf("cw<5> + cw<3> = %d, extent = %zu\n",
                (int)decltype(c)::value, arr.size());
    return 0;
}

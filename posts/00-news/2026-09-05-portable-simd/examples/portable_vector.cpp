// portable_vector.cpp  (~15 lines)
// C++26 standardises portable SIMD as <simd> (P1928): the element type and the
// width are part of the type, and ordinary arithmetic operators emit vector
// instructions for whatever the target supports (SSE, AVX, NEON) with no
// intrinsics and no per-architecture #ifdef.
//
// GCC 16.1's libstdc++ does not ship the standard <simd> header yet
// (__cpp_lib_simd is undefined). It does ship the Parallelism TS version the
// standard type grew out of, which is the same programming model:
// std::experimental::native_simd<float> becomes std::simd<float> later.
//
// The printed width is a property of the TARGET: 4 on baseline x86-64,
// 8 with -march=x86-64-v3.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 portable_vector.cpp

#include <cstdio>
#include <experimental/simd>

namespace stdx = std::experimental;

int main() {
    stdx::native_simd<float> a = 1.5f;
    stdx::native_simd<float> b = 2.0f;
    auto c = a * b;                       // one multiply, every lane at once

    std::printf("width = %zu\n", (size_t)c.size());
    for (std::size_t i = 0; i < c.size(); ++i) std::printf("%g ", (double)c[i]);
    std::printf("\n");
}

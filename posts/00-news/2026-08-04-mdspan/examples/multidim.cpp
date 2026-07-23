// multidim.cpp  (~24 lines)
// std::mdspan (C++23, P0009) is a non-owning multidimensional view over a flat
// buffer. It separates storage (a contiguous array you already have) from shape
// (extents) and indexing (a layout mapping), so one std::vector can be viewed as
// a 3x4 matrix with real m[r, c] indexing and no copy. GCC 16.1's libstdc++
// ships it. Mark Hoemmen's C++Now 2026 keynote is on where this and the parallel
// algorithms are heading; this is the everyday core of it.
//
// Compile (GCC 16.1): g++ -std=c++23 -O2 multidim.cpp

#include <mdspan>
#include <print>
#include <vector>

int main() {
    std::vector<int> storage(12);
    for (int i = 0; i < 12; ++i) storage[i] = i;

    // One flat buffer, viewed as 3 rows by 4 columns. No allocation, no copy.
    std::mdspan m(storage.data(), 3, 4);
    std::println("shape: {} x {}", m.extent(0), m.extent(1));

    for (std::size_t r = 0; r < m.extent(0); ++r) {
        for (std::size_t c = 0; c < m.extent(1); ++c)
            std::print("{:3}", m[r, c]);   // C++23 multidimensional subscript
        std::println("");
    }
}

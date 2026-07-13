// bench_arena.cpp  (~55 lines)
// Does swapping the default allocator for a pmr arena still pay in 2026? This
// times the SAME workload (build a vector of 64 ints, 20000 times over) once on
// the default allocator and once on a monotonic_buffer_resource reset between
// rounds. It also counts global operator new calls, which are deterministic, so
// the structural difference is visible even when wall-clock on shared CI is noisy.
//
// Treat the printed milliseconds as illustrative. The honest, controlled numbers
// are in the post. What is NOT noisy: the allocation count.
//
// Build (GCC 16.1):  g++ -std=c++17 -O2 bench_arena.cpp

#include <memory_resource>
#include <vector>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <new>

static long g_new_calls = 0;
void* operator new(std::size_t n) { ++g_new_calls; if (void* p = std::malloc(n)) return p; throw std::bad_alloc{}; }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

using clk = std::chrono::steady_clock;

int main() {
    constexpr int rounds = 20000;
    constexpr int n = 64;
    long sink = 0;

    long new_before = g_new_calls;
    auto t0 = clk::now();
    for (int r = 0; r < rounds; ++r) {
        std::vector<int> v;
        for (int i = 0; i < n; ++i) v.push_back(i);
        sink += v.back();
    }
    auto t1 = clk::now();
    long default_new = g_new_calls - new_before;

    std::byte buffer[64 * 1024];
    std::pmr::monotonic_buffer_resource arena(buffer, sizeof(buffer));
    new_before = g_new_calls;
    auto t2 = clk::now();
    for (int r = 0; r < rounds; ++r) {
        std::pmr::vector<int> v(&arena);
        for (int i = 0; i < n; ++i) v.push_back(i);
        sink += v.back();
        arena.release();                 // hand the round's memory back to the buffer
    }
    auto t3 = clk::now();
    long arena_new = g_new_calls - new_before;

    auto ms = [](auto a, auto b){ return std::chrono::duration<double, std::milli>(b - a).count(); };
    std::printf("default allocator: %6.1f ms, %7ld heap allocations\n", ms(t0, t1), default_new);
    std::printf("pmr arena        : %6.1f ms, %7ld heap allocations\n", ms(t2, t3), arena_new);
    std::printf("(sink=%ld; times are illustrative on shared CI, the allocation counts are not)\n", sink);
}

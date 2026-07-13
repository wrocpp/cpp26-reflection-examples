// zero_heap.cpp  (~45 lines)
// The HFT and game-loop promise of pmr: pre-own the memory, then run a hot path
// that never calls the global allocator. We prove it by overriding operator new
// to COUNT heap allocations, then filling a pmr::vector that draws from a
// monotonic_buffer_resource backed by a 256 KB stack buffer. The upstream is
// null_memory_resource, so a spill past the buffer would THROW rather than
// silently reach the heap. The new-counter stays at zero.
//
// Build (GCC 16.1):  g++ -std=c++17 -O2 zero_heap.cpp

#include <memory_resource>
#include <vector>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <new>

static int g_new_calls = 0;

void* operator new(std::size_t n) {
    ++g_new_calls;
    if (void* p = std::malloc(n)) return p;
    throw std::bad_alloc{};
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

int main() {
    std::array<std::byte, 256 * 1024> buffer;
    std::pmr::monotonic_buffer_resource arena(
        buffer.data(), buffer.size(), std::pmr::null_memory_resource());

    const int before = g_new_calls;
    std::pmr::vector<int> v(&arena);
    for (int i = 0; i < 10000; ++i) v.push_back(i);   // grows, all from the buffer
    const int during = g_new_calls - before;

    std::printf("pushed %zu ints; the vector grew through several reallocations\n", v.size());
    std::printf("global operator new calls during the fill: %d\n", during);
    std::printf("upstream is null_memory_resource, so any heap spill would have thrown\n");
}

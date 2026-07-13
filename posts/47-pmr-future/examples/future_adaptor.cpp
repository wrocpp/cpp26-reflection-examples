// future_adaptor.cpp  (~50 lines)
// pmr is not winding down. WG21's 2024 policy (P3002) says every new allocating
// standard type should ship a pmr alias; P3153 makes optional allocator-aware;
// and P1083 (targeting C++26) adds resource_adaptor to wrap ANY classic C++
// allocator as a memory_resource. Here is the bridge P1083 standardizes,
// hand-rolled in a few lines and working today: take a classic allocator and
// expose it through the pmr interface, then drive a pmr::vector with it.
//
// Build (GCC 16.1):  g++ -std=c++17 -O2 future_adaptor.cpp

#include <memory_resource>
#include <vector>
#include <memory>
#include <cstddef>
#include <cstdio>

// A classic C++ allocator (the pre-pmr, compile-time model).
template <class T>
struct shouting_allocator {
    using value_type = T;
    shouting_allocator() = default;
    template <class U> shouting_allocator(const shouting_allocator<U>&) noexcept {}
    T* allocate(std::size_t n) {
        std::printf("  classic allocator hands out %zu bytes\n", n * sizeof(T));
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    void deallocate(T* p, std::size_t) noexcept { ::operator delete(p); }
};

// The bridge P1083 standardizes: adapt a classic allocator to memory_resource.
template <class Alloc>
struct resource_adaptor : std::pmr::memory_resource {
    using bytes = typename std::allocator_traits<Alloc>::template rebind_alloc<std::byte>;
    Alloc alloc;
    void* do_allocate(std::size_t n, std::size_t) override {
        bytes b(alloc);
        return b.allocate(n);
    }
    void do_deallocate(void* p, std::size_t n, std::size_t) override {
        bytes b(alloc);
        b.deallocate(static_cast<std::byte*>(p), n);
    }
    bool do_is_equal(const std::pmr::memory_resource& o) const noexcept override { return this == &o; }
};

int main() {
    resource_adaptor<shouting_allocator<std::byte>> res;
    std::pmr::vector<int> v(&res);         // a pmr container driving a classic allocator
    for (int i = 0; i < 8; ++i) v.push_back(i);
    std::printf("a pmr::vector just ran on a classic C++ allocator, through one adaptor\n");
}

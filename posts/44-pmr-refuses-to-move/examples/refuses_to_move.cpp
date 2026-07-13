// refuses_to_move.cpp  (~40 lines)
// Move-assignment normally steals the buffer in O(1). For pmr containers with
// DIFFERENT resources it cannot: the source's memory belongs to another arena.
// So move-assignment falls back to allocating in the destination's resource and
// moving elements one at a time. We watch the destination's resource get an
// allocation during what looks like a move.
//
// Build (GCC 16.1):  g++ -std=c++17 -O2 refuses_to_move.cpp

#include <memory_resource>
#include <vector>
#include <cstdio>

struct counting_resource : std::pmr::memory_resource {
    int allocations = 0;
    void* do_allocate(std::size_t b, std::size_t a) override {
        ++allocations;
        return std::pmr::new_delete_resource()->allocate(b, a);
    }
    void do_deallocate(void* p, std::size_t b, std::size_t a) override {
        std::pmr::new_delete_resource()->deallocate(p, b, a);
    }
    bool do_is_equal(const std::pmr::memory_resource& o) const noexcept override { return this == &o; }
};

int main() {
    counting_resource res_a, res_b;
    std::pmr::vector<int> src(&res_a);
    for (int i = 0; i < 100; ++i) src.push_back(i);

    std::pmr::vector<int> dst(&res_b);
    int before = res_b.allocations;
    dst = std::move(src);                 // different resources: cannot steal the buffer
    int during_move = res_b.allocations - before;

    std::printf("elements in dst after the move: %zu\n", dst.size());
    std::printf("allocations in dst's resource during the 'move': %d\n", during_move);
    std::printf("(a real move steals the pointer and allocates 0; this one copied)\n");
}

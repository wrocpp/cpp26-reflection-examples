// asymmetric_fence.cpp  (~55 lines)
// Asymmetric fences move the cost of a memory barrier OFF the hot path. The
// common path pays only a compiler barrier (std::atomic_signal_fence); the rare
// path pays a real, process-wide barrier via Linux membarrier(), which forces
// every OTHER thread to run a full hardware fence. This is the trick Folly uses
// in hazard pointers and RCU, and the one C++ blessed in [atomics.order]/4
// (proposal P1202).
//
// Below is a store-buffer (Dekker) test: under sequential consistency the
// outcome a==0 && b==0 is forbidden. We run it many times with the asymmetric
// pair (light fence on one thread, heavy membarrier on the other); it never
// occurs.
//
// Build (x86-64 GCC): g++ -std=c++20 -O2 -pthread asymmetric_fence.cpp

#include <atomic>
#include <barrier>
#include <cstdio>
#include <thread>
#include <linux/membarrier.h>
#include <sys/syscall.h>
#include <unistd.h>

static int membarrier(int cmd) { return static_cast<int>(syscall(SYS_membarrier, cmd, 0u, 0)); }

// Light fence: compiler barrier only. Zero hardware cost. (hot / common path)
static inline void light_fence() { std::atomic_signal_fence(std::memory_order_seq_cst); }

// Heavy fence: make every other thread execute a full barrier. (rare path)
static inline void heavy_fence() { membarrier(MEMBARRIER_CMD_PRIVATE_EXPEDITED); }

std::atomic<int> x{0}, y{0};
int a = 0, b = 0;

int main() {
    if (membarrier(MEMBARRIER_CMD_REGISTER_PRIVATE_EXPEDITED) != 0) {
        std::puts("membarrier registration failed");
        return 0;
    }
    constexpr int N = 50000;
    int forbidden = 0;
    std::barrier start(2), done(2);

    std::thread light([&] {
        for (int i = 0; i < N; ++i) {
            start.arrive_and_wait();
            x.store(1, std::memory_order_relaxed);
            light_fence();                                  // common path: cheap
            a = y.load(std::memory_order_relaxed);
            done.arrive_and_wait();
        }
    });

    for (int i = 0; i < N; ++i) {
        x.store(0, std::memory_order_relaxed);
        y.store(0, std::memory_order_relaxed);
        start.arrive_and_wait();                            // release both into the trial
        y.store(1, std::memory_order_relaxed);
        heavy_fence();                                      // rare path: process-wide barrier
        b = x.load(std::memory_order_relaxed);
        done.arrive_and_wait();
        if (a == 0 && b == 0) ++forbidden;
    }
    light.join();

    std::printf("asymmetric fence: %d trials, %d forbidden (a==0 && b==0)\n", N, forbidden);
    return 0;
}

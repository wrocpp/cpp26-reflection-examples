// Pattern: data race on a non-atomic shared variable. ThreadSanitizer
// (TSan) finds these by instrumenting every memory access and detecting
// concurrent unordered access without synchronisation.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-safety:2026-05 \
//     containers/scripts/run-tsan.sh \
//     posts/toolset/sanitizers-2026/examples/data-race.cpp
//
// Expected: program prints
//   WARNING: ThreadSanitizer: data race
// with read + write stack frames pointing at the increment in run().
// Final counter value will be wrong (less than 200000) on most runs.
//
// The fix: change `int counter` to `std::atomic<int> counter` and use
// counter.fetch_add(1, std::memory_order_relaxed) -- TSan then reports
// no warnings. ThreadSanitizer in CI catches concurrency bugs that
// stress tests miss because they only fire under specific schedules.

#include <print>
#include <thread>

int counter = 0;              // <- shared, non-atomic

void run() {
    for (int i = 0; i < 100'000; ++i) {
        ++counter;            // <- TSan flags: data race vs the other thread
    }
}

int main() {
    std::thread a(run);
    std::thread b(run);
    a.join();
    b.join();
    std::println("counter = {} (expected 200000)", counter);
}

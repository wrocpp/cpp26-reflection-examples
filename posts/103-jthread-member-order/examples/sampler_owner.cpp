// A worker thread that sums a vector owned by the same object.
// Default build: the thread is declared last (correct), the program exits 0.
// -DWORKER_BEFORE_DATA declares the thread before the vector it reads (the bug).
// -DREPORT_ONLY makes ThreadSanitizer print its report and still exit 0.
// The ready flag is declared first in both builds and set at the end of the
// constructor, so only the destruction end of the lifetime is exercised.
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <numeric>
#include <stop_token>
#include <thread>
#include <vector>

#ifdef REPORT_ONLY
extern "C" const char* __tsan_default_options() { return "exitcode=0"; }
#endif

constexpr auto kObserve = std::chrono::milliseconds{10};
constexpr std::size_t kSamples = 1024;

struct Sampler {
    std::atomic<bool> ready{false};
#ifdef WORKER_BEFORE_DATA
    std::jthread worker{[this](std::stop_token st) { run(st); }};
    std::vector<int> samples = std::vector<int>(kSamples, 1);
#else
    std::vector<int> samples = std::vector<int>(kSamples, 1);
    std::jthread worker{[this](std::stop_token st) { run(st); }};
#endif

    Sampler() { ready.store(true, std::memory_order_release); }

    void run(std::stop_token st) {
        while (!ready.load(std::memory_order_acquire)) {}
        long total = 0;
        while (!st.stop_requested()) {
            total += std::accumulate(samples.begin(), samples.end(), 0);
        }
        std::printf("worker saw total > 0: %s\n", total > 0 ? "yes" : "no");
    }
};

int main() {
    Sampler sampler;
    std::this_thread::sleep_for(kObserve);
    std::puts("main done");
}

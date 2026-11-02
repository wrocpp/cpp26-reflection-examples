// A worker thread that counts until it is asked to stop.
//
// Default build: the thread is declared last, the program exits 0.
// -DWORKER_FIRST      declares the thread before the counter it increments (the bug).
// -DGUARDED           adds a ready flag, declared before the thread and set at the end
//                     of the constructor, so the worker waits and only the destruction
//                     end of the lifetime is exercised.
// -DREPORT_ONLY       makes ThreadSanitizer print its report and still exit 0, so a
//                     Compiler Explorer executor shows the report.
#include <atomic>
#include <chrono>
#include <cstdio>
#include <stop_token>
#include <thread>

#ifdef REPORT_ONLY
extern "C" const char* __tsan_default_options() { return "exitcode=0"; }
#endif

constexpr auto kObserve = std::chrono::milliseconds{10};

struct Meter {
#ifdef GUARDED
    std::atomic<bool> ready{false};
#endif
#ifdef WORKER_FIRST
    std::jthread worker{[this](std::stop_token st) { run(st); }};
    std::atomic<unsigned> ticks{0};
#else
    std::atomic<unsigned> ticks{0};
    std::jthread worker{[this](std::stop_token st) { run(st); }};
#endif

    Meter() {
#ifdef GUARDED
        ready.store(true, std::memory_order_release);
#endif
    }

    void run(std::stop_token st) {
#ifdef GUARDED
        while (!ready.load(std::memory_order_acquire)) {}
#endif
        while (!st.stop_requested()) {
            ticks.fetch_add(1, std::memory_order_relaxed);
        }
    }
};

int main() {
    Meter meter;
    std::this_thread::sleep_for(kObserve);
    std::printf("ticks > 0: %s\n", meter.ticks.load() > 0 ? "yes" : "no");
}

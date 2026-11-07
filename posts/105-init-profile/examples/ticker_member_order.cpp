[[profiles::enforce(std::init)]];

#include <atomic>
#include <chrono>
#include <cstdio>
#include <stop_token>
#include <thread>

namespace {

constexpr std::chrono::milliseconds kRunTime{10};

struct Ticker {
#ifdef THREAD_LAST
    std::atomic<unsigned> ticks = 0;
    std::jthread worker;
#else
    std::jthread worker;                 // starts running in the constructor...
    std::atomic<unsigned> ticks = 0;     // ...before this member is initialized
#endif

    Ticker()
        : worker{[this](std::stop_token stop) {
              while (!stop.stop_requested()) {
                  ticks.fetch_add(1, std::memory_order_relaxed);
              }
          }} {}
};

}  // namespace

int main() {
    {
        Ticker ticker;
        std::this_thread::sleep_for(kRunTime);
    }
    std::puts("done");
}

// mutex_magic_statics.cpp  (~35 lines)
// The pre-C++20 fix: guard the shared sink with a mutex. A function-local
// `static std::mutex` is the idiom, and since C++11 "magic statics" its
// initialization is itself thread-safe: if two threads reach the declaration at
// once, one constructs it and the other waits. We prove that by counting how
// many times a function-local static is constructed under a thread stampede.
//
// Build (GCC 16.1):  g++ -std=c++20 -O2 -pthread mutex_magic_statics.cpp

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>

std::atomic<int> construction_count{0};

struct Sink {
    Sink() { ++construction_count; }   // count how many times this actually runs
};

void log_line(const char* who, int i) {
    static std::mutex mtx;             // magic static: initialized exactly once, safely
    static Sink the_sink;              // same guarantee, even under contention
    const std::lock_guard lock(mtx);
    std::cout << "[thread " << who << "] log record #" << i
              << " -- the quick brown fox jumps over the lazy dog\n";
}

int main() {
    std::vector<std::thread> threads;
    for (const char* who : {"A", "B", "C"})
        threads.emplace_back([who]{ for (int i = 0; i < 6; ++i) log_line(who, i); });
    for (auto& t : threads)
        t.join();
    std::cout << "\nthe function-local static Sink was constructed "
              << construction_count.load() << " time(s)\n";
}

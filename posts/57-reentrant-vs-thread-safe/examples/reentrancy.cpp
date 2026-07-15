// reentrancy.cpp  (~40 lines)
// "Thread-safe" and "reentrant" are different properties, and a std::mutex gives
// you the first without the second. Part 1: a mutex makes a shared counter safe
// across threads (thread-safe). Part 2: a function that re-enters itself while
// holding the lock needs a std::recursive_mutex; a plain std::mutex would
// deadlock on the second lock() -- thread-safe code that is not reentrant.
//
// Build (GCC 16.1):  g++ -std=c++20 -O2 -pthread reentrancy.cpp

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex counter_mtx;
long counter = 0;

void bump(int times) {
    for (int i = 0; i < times; ++i) {
        const std::lock_guard lock(counter_mtx);
        ++counter;                        // thread-safe: the mutex serializes it
    }
}

std::recursive_mutex rec_mtx;
void countdown(int n) {
    const std::lock_guard lock(rec_mtx);  // re-locked on each recursive call
    std::cout << "countdown " << n << "\n";
    if (n > 0) countdown(n - 1);          // reentrant: same thread, lock still held
}

int main() {
    std::vector<std::thread> t;
    for (int k = 0; k < 4; ++k) t.emplace_back(bump, 10000);
    for (auto& x : t) x.join();
    std::cout << "thread-safe counter (4 threads x 10000): " << counter << "\n\n";

    // Reentrant call chain. With a plain std::mutex the second lock() here would
    // deadlock; std::recursive_mutex permits the same thread to re-lock.
    countdown(3);
}

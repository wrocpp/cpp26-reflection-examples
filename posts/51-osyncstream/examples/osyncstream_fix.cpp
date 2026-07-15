// osyncstream_fix.cpp  (~30 lines)
// std::osyncstream (C++20) buffers each line privately and transfers it to the
// wrapped buffer as one contiguous chunk when the temporary is destroyed. Give
// every logging thread its own osyncstream targeting std::cout and the lines
// stop interleaving. The catch: the no-interleave guarantee holds only if EVERY
// writer to that buffer uses a syncstream; one stray raw std::cout << re-tears it.
//
// Build (GCC 16.1):  g++ -std=c++20 -O2 -pthread osyncstream_fix.cpp

#include <syncstream>
#include <iostream>
#include <thread>
#include <vector>

void emit(const char* who) {
    for (int i = 0; i < 6; ++i) {
        std::osyncstream(std::cout)
            << "[thread " << who << "] "
            << "log record #" << i
            << " -- the quick brown fox jumps over the lazy dog"
            << '\n';   // the whole line is emitted atomically as the temporary dies
    }
}

int main() {
    std::vector<std::thread> threads;
    for (const char* who : {"A", "B", "C"})
        threads.emplace_back(emit, who);
    for (auto& t : threads)
        t.join();
}

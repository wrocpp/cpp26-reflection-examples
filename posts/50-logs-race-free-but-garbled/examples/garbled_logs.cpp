// garbled_logs.cpp  (~35 lines)
// Three threads log to std::cout at once, the way a Qt qInstallMessageHandler
// callback would when the framework calls it "from different threads, in
// parallel." The C++ standard promises this is free of DATA RACES (no undefined
// behavior) as long as sync_with_stdio is true. It does NOT promise the
// characters will not INTERLEAVE. Those are two different guarantees, and this
// program prints the difference: whole log lines tear into each other.
//
// Build (GCC 16.1):  g++ -std=c++20 -O2 -pthread garbled_logs.cpp

#include <iostream>
#include <thread>
#include <vector>

// One "log line" is several << operations, like a real handler assembling a
// prefix, a category, and a message. Nothing synchronizes the pieces, so
// another thread can slip its own output in between them.
void emit(const char* who) {
    for (int i = 0; i < 6; ++i) {
        std::cout << "[thread " << who << "] "
                  << "log record #" << i
                  << " -- the quick brown fox jumps over the lazy dog"
                  << '\n';
    }
}

int main() {
    std::vector<std::thread> threads;
    for (const char* who : {"A", "B", "C"})
        threads.emplace_back(emit, who);
    for (auto& t : threads)
        t.join();
}

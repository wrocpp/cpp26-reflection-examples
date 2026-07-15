// print_atomic.cpp  (~30 lines)
// std::print / std::println (C++23) format into a temporary and write it to the
// stream in one shot, so a SINGLE call's output never interleaves -- printf-style
// atomicity, no syncstream and no mutex needed. But the guarantee is per-call:
// split one logical line across two prints and the two calls can still interleave.
//
// Build (GCC 16.1):  g++ -std=c++23 -O2 -pthread print_atomic.cpp

#include <print>
#include <thread>
#include <vector>

void one_call(const char* who) {
    for (int i = 0; i < 6; ++i)
        std::println("[thread {}] log record #{} -- the quick brown fox", who, i);
}

void two_calls(const char* who) {
    for (int i = 0; i < 6; ++i) {
        std::print("[thread {}] log record #{}", who, i);   // call 1: prefix
        std::this_thread::yield();                          // let another thread cut in
        std::println(" -- the quick brown fox");            // call 2: a separate call
    }
}

int main() {
    std::vector<std::thread> t;
    for (const char* who : {"A", "B", "C"}) t.emplace_back(one_call, who);
    for (auto& x : t) x.join();

    std::println("--- above: one call per line (whole). below: two calls per line ---");

    std::vector<std::thread> t2;
    for (const char* who : {"A", "B", "C"}) t2.emplace_back(two_calls, who);
    for (auto& x : t2) x.join();
}

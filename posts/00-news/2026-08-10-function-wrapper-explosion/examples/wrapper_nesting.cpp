// wrapper_nesting.cpp  (~30 lines)
// std::function and C++23's std::copyable_function are both type-erased
// callable wrappers, and NEITHER recognises the other. Converting between them
// does not unwrap and re-wrap the underlying lambda: it wraps the whole
// previous wrapper. Round-trip in a loop and you build a linked list of
// wrappers, each call walking one more level of indirection.
//
// 200 round trips is enough to make the cost obvious. Arthur O'Dwyer found the
// same effect at larger scale; this version is sized to run inside Compiler
// Explorer's execute timeout.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 wrapper_nesting.cpp

#include <chrono>
#include <cstdio>
#include <functional>

static long long time_10k(std::function<int(int)>& f) {
    auto t0 = std::chrono::steady_clock::now();
    long long acc = 0;
    for (int i = 0; i < 10000; ++i) acc += f(i);
    auto t1 = std::chrono::steady_clock::now();
    std::printf("  (checksum %lld)\n", acc);
    return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

int main() {
    std::function<int(int)> plain = [](int x) { return x + 1; };
    std::printf("baseline, no conversions:\n");
    long long before = time_10k(plain);

    std::function<int(int)> f = [](int x) { return x + 1; };
    for (int i = 0; i < 200; ++i) {
        std::copyable_function<int(int)> c = f;   // wraps f whole
        f = std::move(c);                          // wraps that whole
    }
    std::printf("after 200 round trips:\n");
    long long after = time_10k(f);

    std::printf("\n10k calls before : %lld us\n", before);
    std::printf("10k calls after  : %lld us\n", after);
    if (before > 0) std::printf("slowdown         : %lldx\n", after / before);
    return 0;
}

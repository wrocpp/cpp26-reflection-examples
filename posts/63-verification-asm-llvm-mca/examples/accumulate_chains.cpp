// accumulate_chains.cpp  (~24 lines)
// Both functions add the same n floats. The first uses one accumulator, so
// every add waits for the previous one to retire: a serial dependency chain
// whose speed is bounded by the latency of a single add. The second keeps four
// independent accumulators, so the CPU can have four adds in flight and the
// loop becomes throughput-bound instead.
//
// You do not need a profiler to see this. The generated assembly shows it, and
// llvm-mca will predict the throughput difference from that assembly without
// ever running the program. Note -ffast-math: without it the compiler may not
// reassociate floating-point adds, which is exactly why the hand-written
// version below breaks the chain explicitly.
//
// Compile (GCC 16.1): g++ -std=c++20 -O3 -march=x86-64-v3 -c accumulate_chains.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++20 -O3 -march=x86-64-v3

#include <cstddef>

float sum_serial(const float* v, std::size_t n) {
    float acc = 0.0f;                    // one chain: add -> add -> add ...
    for (std::size_t i = 0; i < n; ++i) acc += v[i];
    return acc;
}

float sum_unrolled(const float* v, std::size_t n) {
    float a = 0, b = 0, c = 0, d = 0;    // four independent chains
    std::size_t i = 0;
    for (; i + 4 <= n; i += 4) {
        a += v[i]; b += v[i + 1]; c += v[i + 2]; d += v[i + 3];
    }
    for (; i < n; ++i) a += v[i];
    return (a + b) + (c + d);
}

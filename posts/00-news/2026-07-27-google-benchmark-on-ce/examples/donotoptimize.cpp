// donotoptimize.cpp  (~26 lines)
// Google Benchmark links and runs on Compiler Explorer: add the `benchmark`
// library, turn on execution, and the timing table prints in the output pane.
// The lesson is benchmark::DoNotOptimize. Without it, the result of the loop
// body is unused, so the optimizer deletes the whole loop and reports a time
// near zero for a trillion "iterations" -- a number that is worse than useless.
// DoNotOptimize forces the value to be materialized, restoring an honest timing.
//
// Compile (GCC 16.1): g++ -std=c++20 -O2 -pthread donotoptimize.cpp -lbenchmark
// verify: ce-libs: benchmark
// verify: gcc-only
// verify: gcc-options: -std=c++20 -O2 -pthread

#include <benchmark/benchmark.h>
#include <cmath>

static void BM_Unfenced(benchmark::State& state) {
    for (auto _ : state) {
        double x = std::sqrt(2.0);  // result unused: the loop is optimized away
        (void)x;
    }
}
BENCHMARK(BM_Unfenced);

static void BM_Fenced(benchmark::State& state) {
    for (auto _ : state) {
        double x = std::sqrt(2.0);
        benchmark::DoNotOptimize(x);  // the compiler must keep the computation
    }
}
BENCHMARK(BM_Fenced);

BENCHMARK_MAIN();

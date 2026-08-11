// regex_vs_ctre_bench.cpp
//
// std::regex vs CTRE on the same pattern and the same input, measured with
// google/benchmark.
//
// FAIRNESS NOTE, because this comparison is easy to rig:
//   * The std::regex object is constructed ONCE, outside the timed loop.
//     Building it inside the loop would measure pattern parsing every
//     iteration and flatter CTRE enormously. Steady-state matching is the
//     honest comparison, and it is the one std::regex does best at.
//   * Both sides run the same pattern against the same subject and both
//     extract the captures, so neither is doing less work than the other.
//   * DoNotOptimize keeps the compiler from deleting either loop body. CTRE
//     resolves the pattern at compile time, so without it the whole match
//     can fold away and the "benchmark" measures nothing.
//
// Compile: -std=c++20 -O2, libs: ctre + google/benchmark
// verify: ce-libs: ctre, benchmark
// verify: gcc-options: -std=c++20 -O2

#include <benchmark/benchmark.h>
#include <ctre.hpp>

#include <regex>
#include <string>
#include <string_view>

namespace {

constexpr std::string_view kPattern = R"((\d{4})/(\d{1,2})/(\d{1,2}))";
const std::string kSubject = "2026/08/22";

// Constructed once. This is the charitable setup for std::regex.
const std::regex kStdRe{std::string{kPattern}};

void StdRegexMatch(benchmark::State& state) {
    for (auto _ : state) {
        std::smatch m;
        bool ok = std::regex_match(kSubject, m, kStdRe);
        benchmark::DoNotOptimize(ok);
        benchmark::DoNotOptimize(m);
    }
}
BENCHMARK(StdRegexMatch);

void CtreMatch(benchmark::State& state) {
    std::string_view subject{kSubject};
    for (auto _ : state) {
        auto m = ctre::match<R"((\d{4})/(\d{1,2})/(\d{1,2}))">(subject);
        bool ok = static_cast<bool>(m);
        benchmark::DoNotOptimize(ok);
        benchmark::DoNotOptimize(m);
    }
}
BENCHMARK(CtreMatch);

// The other half of the story: many programs build the regex near where they
// use it. This measures construction plus one match, which is the shape a
// one-shot validation actually has.
void StdRegexConstructAndMatch(benchmark::State& state) {
    for (auto _ : state) {
        std::regex re{std::string{kPattern}};
        std::smatch m;
        bool ok = std::regex_match(kSubject, m, re);
        benchmark::DoNotOptimize(ok);
    }
}
BENCHMARK(StdRegexConstructAndMatch);

}  // namespace

BENCHMARK_MAIN();

// verify: gcc-only
// Build: g++ -std=c++26 -O2 -pthread load_cost.cpp
// Prints the cost of one atomic<shared_ptr>::load() next to one plain shared_ptr copy, as nanoseconds
// per operation, in one thread. Each case is run kRuns times and printed as min-max over those runs,
// because the machine is shared. A contended run (readers plus a writer) is left out on purpose: the
// Compiler Explorer executor reports hardware_concurrency=2, too few cores for a meaningful result.
// A warm-up thread runs first so that libstdc++ takes its multi-threaded path for the plain copy too
// (it checks __libc_single_threaded, which stays false once any thread has been created).
// Limits: timings from a shared machine are indicative only; the exit code is always 0.
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <memory>
#include <thread>

struct Data {
  int value;
};

constexpr int kRuns = 9;
constexpr int kLoadsSingle = 2'000'000;
constexpr double kNanosPerSecond = 1e9;

struct Range {
  double lo;
  double hi;
};

std::shared_ptr<Data> plain = std::make_shared<Data>(0);
std::atomic<std::shared_ptr<Data>> shared{std::make_shared<Data>(0)};
std::atomic<long> sink{0};

template <class Op>
double nanos_per_op(int ops, Op op) {
  const auto start = std::chrono::steady_clock::now();
  long keep = 0;
  for (int i = 0; i < ops; ++i) {
    keep += op()->value;
  }
  const std::chrono::duration<double> took = std::chrono::steady_clock::now() - start;
  sink += keep;
  return took.count() * kNanosPerSecond / ops;
}

template <class Case>
Range over_runs(Case one_run) {
  Range range{.lo = 1e18, .hi = 0};
  for (int run = 0; run < kRuns; ++run) {
    const double ns = one_run();
    range = {.lo = std::min(range.lo, ns), .hi = std::max(range.hi, ns)};
  }
  return range;
}

void print(const char* label, Range r) { std::printf("%-34s %7.1f - %7.1f ns/op\n", label, r.lo, r.hi); }

int main() {
  std::jthread warm_up([] {});
  warm_up.join();
  const auto copy_plain = [] { return std::shared_ptr<Data>(plain); };
  const auto load_atomic = [] { return shared.load(); };
  std::printf("hardware_concurrency=%u runs=%d\n", std::thread::hardware_concurrency(), kRuns);
  print("1 thread, plain copy", over_runs([&] { return nanos_per_op(kLoadsSingle, copy_plain); }));
  print("1 thread, atomic load", over_runs([&] { return nanos_per_op(kLoadsSingle, load_atomic); }));
  return 0;
}

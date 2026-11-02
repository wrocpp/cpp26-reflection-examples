// When is the temporary behind a coroutine's const& parameter destroyed? The Probe prints when it is
// constructed and destroyed, and the generator body prints how many probes are alive when it first
// runs. Only the alive counter is read, never a destroyed object. Needs <generator>, so -std=c++23
// or later (/std:c++latest on MSVC). The output differs between compilers on purpose: that is the finding.
#include <generator>
#include <print>

struct Probe {
  static inline int alive = 0;
  Probe() {
    ++alive;
    std::println("  Probe constructed");
  }
  Probe(const Probe&) = delete;
  ~Probe() {
    --alive;
    std::println("  Probe destroyed");
  }
};

std::generator<int> numbers([[maybe_unused]] const Probe& probe) {
  std::println("  body starts, probes alive = {}", Probe::alive);
  co_yield 1;
  std::println("  body resumes, probes alive = {}", Probe::alive);
}

int main() {
#ifdef __cpp_range_based_for
  std::println("__cpp_range_based_for = {}", __cpp_range_based_for);
#endif

  std::println("temporary written in the range-for:");
  for (int value : numbers(Probe{})) std::println("  loop body, value {}, probes alive = {}", value, Probe::alive);

  std::println("generator stored first:");
  auto stored = numbers(Probe{});
  for (int value : stored) std::println("  loop body, value {}, probes alive = {}", value, Probe::alive);
}

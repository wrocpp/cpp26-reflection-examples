// Control for destruction_order.cpp: the same temporary in a range-for initializer, passed by const& to an
// ordinary function instead of a coroutine. Prints how many probes are alive in the loop body. A compiler
// that implements P2718 prints 1 for both lines; one that differs only for the coroutine prints 1 and 0.
#include <generator>
#include <print>
#include <vector>

struct Probe {
  static inline int alive = 0;
  Probe() { ++alive; }
  Probe(const Probe&) = delete;
  ~Probe() { --alive; }
};

std::vector<int> plain([[maybe_unused]] const Probe& probe) { return {1}; }

std::generator<int> coroutine([[maybe_unused]] const Probe& probe) { co_yield 1; }

int main() {
  for (int value : plain(Probe{})) std::println("ordinary function: value {}, probes alive in the loop body = {}", value, Probe::alive);
  for (int value : coroutine(Probe{})) std::println("coroutine:         value {}, probes alive in the loop body = {}", value, Probe::alive);
}

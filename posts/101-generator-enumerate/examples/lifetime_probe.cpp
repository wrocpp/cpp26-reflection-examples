// When does the temporary behind a coroutine parameter die? The Probe counts live objects, so the
// answer is printed without reading a destroyed object. Needs <generator>, so -std=c++23 or later.
// Each generator yields the number of live probes at its first resume; main prints it next to the
// number alive in the loop body.
#include <generator>
#include <print>

struct Probe {
  static inline int alive = 0;
  Probe() { ++alive; }
  Probe(const Probe&) { ++alive; }
  ~Probe() { --alive; }
};

std::generator<int> by_const_ref([[maybe_unused]] const Probe& probe) { co_yield Probe::alive; }
std::generator<int> by_value([[maybe_unused]] Probe probe) { co_yield Probe::alive; }

void report(const char* label, int at_first_resume) {
  std::println("{}: live probes at the first resume = {}, in the loop body = {}", label, at_first_resume,
               Probe::alive);
}

int main() {
#ifdef __cpp_range_based_for
  std::println("__cpp_range_based_for = {}", __cpp_range_based_for);
#endif

  for (int seen : by_const_ref(Probe{})) report("const&, temporary in the range-for", seen);
  auto stored_ref = by_const_ref(Probe{});
  for (int seen : stored_ref) report("const&, generator stored first", seen);

  for (int seen : by_value(Probe{})) report("by value, temporary in the range-for", seen);
  auto stored_value = by_value(Probe{});
  for (int seen : stored_value) report("by value, generator stored first", seen);
}

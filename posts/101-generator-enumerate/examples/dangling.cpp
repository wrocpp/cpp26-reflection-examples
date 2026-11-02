// A coroutine's reference parameter points at the caller's object, and the coroutine frame outlives the
// call. Taking the container by value puts a copy in the frame. The two failing cases are behind macros
// so the default build runs clean:
//   -DSHOW_REF_IN_RANGE_FOR   the temporary is written in the range-for initializer
//   -DSHOW_REF_STORED         the generator is stored first and iterated later
// Build with -fsanitize=address -g to see the report. Needs <generator>, so -std=c++23 or later.
#include <generator>
#include <print>
#include <vector>

std::generator<int> by_value(std::vector<int> values) {
  for (int value : values) co_yield value;
}

std::generator<int> by_const_ref(const std::vector<int>& values) {
  for (int value : values) co_yield value;
}

std::vector<int> make_values() { return {1, 2, 3}; }

int main() {
  std::print("by value, in a range-for:");
  for (int value : by_value(make_values())) std::print(" {}", value);
  std::println();

  std::print("by value, generator stored first:");
  auto stored = by_value(make_values());
  for (int value : stored) std::print(" {}", value);
  std::println();

#ifdef SHOW_REF_IN_RANGE_FOR
  std::print("by const&, in a range-for:");
  for (int value : by_const_ref(make_values())) std::print(" {}", value);
  std::println();
#endif

#ifdef SHOW_REF_STORED
  std::print("by const&, generator stored first:");
  auto stored_ref = by_const_ref(make_values());
  for (int value : stored_ref) std::print(" {}", value);
  std::println();
#endif
}

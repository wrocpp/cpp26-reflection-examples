// The same dangling call G1's dangling.cpp makes, with nothing but the code: a coroutine takes a
// const& and the caller passes a temporary. Compile it with warnings on and read what the compiler says.
//   -DSHOW_RANGE_FOR   the temporary is written in the range-for initializer
//   -DSHOW_STORED      the generator is stored first and iterated later
// The default build contains neither and runs clean. Needs <generator>, so -std=c++23 or later.
#include <generator>
#include <print>
#include <vector>

std::generator<int> numbers(const std::vector<int>& values) {
  for (int value : values) co_yield value;
}

std::vector<int> make_values() { return {10, 20, 30}; }

int main() {
  std::print("named vector, for reference:");
  const std::vector<int> values = make_values();
  for (int value : numbers(values)) std::print(" {}", value);
  std::println();

#ifdef SHOW_RANGE_FOR
  std::print("temporary in the range-for:");
  for (int value : numbers(make_values())) std::print(" {}", value);
  std::println();
#endif

#ifdef SHOW_STORED
  std::print("stored first:");
  auto stored = numbers(make_values());
  for (int value : stored) std::print(" {}", value);
  std::println();
#endif
}

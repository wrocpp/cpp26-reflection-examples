// std::ranges::elements_of with a vector, inside a generator.
// -DSHOW_BARE_ELEMENTS_OF  elements_of over an lvalue vector: does not compile
#include <generator>
#include <print>
#include <ranges>
#include <vector>

std::generator<int> flatten(std::vector<int> first, std::vector<int> second) {
#ifdef SHOW_BARE_ELEMENTS_OF
  co_yield std::ranges::elements_of(first);
#else
  co_yield std::ranges::elements_of(first | std::views::as_rvalue);
#endif
  co_yield std::ranges::elements_of(second | std::views::as_rvalue);
}

int main() {
  for (int value : flatten({1, 2}, {3})) std::println("{}", value);
}

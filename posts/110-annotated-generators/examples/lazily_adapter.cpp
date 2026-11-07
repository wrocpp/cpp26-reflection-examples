// The negative result. lazily<^^Catalog::evens>(catalog, n) wraps an ordinary method that returns a vector in
// a std::generator. The wrapper is lazy, the method behind it is not: it builds the whole vector first.
// The same method written as a coroutine builds only what the consumer takes.
#include <generator>
#include <meta>
#include <print>
#include <ranges>
#include <vector>

inline constexpr int kRange = 1000;
inline constexpr int kTaken = 3;

struct Catalog {
  int built = 0;

  std::vector<int> evens(int count) {  // an ordinary method: builds everything, then returns
    std::vector<int> out;
    for (int i = 0; i < count; ++i) {
      ++built;
      if (i % 2 == 0) out.push_back(i);
    }
    return out;
  }

  std::generator<int> evens_lazy(int count) {  // the same method written as a coroutine
    for (int i = 0; i < count; ++i) {
      ++built;
      if (i % 2 == 0) co_yield i;
    }
  }
};

template <std::meta::info Method, class Object, class... Args>
auto lazily(Object& object, Args... args)
    -> std::generator<std::ranges::range_value_t<decltype(object.[:Method:](args...))>> {
  for (auto element : object.[:Method:](args...)) co_yield element;
}

int main() {
  Catalog wrapped;
  for (int value : lazily<^^Catalog::evens>(wrapped, kRange) | std::views::take(kTaken)) std::print("{} ", value);
  std::println("| adapter: {} elements built for {} consumed", wrapped.built, kTaken);
  Catalog coroutine;
  for (int value : coroutine.evens_lazy(kRange) | std::views::take(kTaken)) std::print("{} ", value);
  std::println("| coroutine: {} elements built for {} consumed", coroutine.built, kTaken);
}

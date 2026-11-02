// The shape of Alex Dathskovsky's std::generator enumerate: the container comes in by value and
// each step yields an (index, value) pair. Prints the four lines of his example.
#include <array>
#include <generator>
#include <print>
#include <tuple>

std::generator<const std::tuple<int, int>> enumerate(auto container) {
  for (int index = 0; auto&& value : container) {
    co_yield std::tuple{index, value};
    ++index;
  }
}

int main() {
  for (auto [index, value] : enumerate(std::array{4, 3, 2, 1})) {
    std::println("the value in idx {} is {}", index, value);
  }
}

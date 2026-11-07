// Per-element time of four ways to number the elements of a vector of a million ints. Each variant runs
// kPasses times and the best pass is reported. Change SALT (-DSALT=n) between runs: it alters the data,
// so a cached result from a service that stores outputs is not mistaken for a new measurement.
// The output is not deterministic, so this is a program to run several times, not a single figure.
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <generator>
#include <iostream>
#include <limits>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

#ifndef SALT
#define SALT 0
#endif

constexpr int kSalt = SALT;
constexpr std::size_t kElements = 1'000'000;
constexpr std::size_t kStride = 7;
constexpr std::size_t kValueRange = 1000;
constexpr int kPasses = 5;

using Clock = std::chrono::steady_clock;

std::generator<const std::tuple<int, int>> enumerate_by_value(auto container) {
  for (int index = 0; auto&& value : container) {
    co_yield std::tuple{index, value};
    ++index;
  }
}

template <std::ranges::input_range Range>
using Numbered = std::pair<std::size_t, std::ranges::range_reference_t<Range>>;

template <std::ranges::input_range Range>
std::generator<Numbered<Range>> enumerate_generic(Range range) {
  for (std::size_t index = 0; auto&& element : range) {
    co_yield Numbered<Range>{index, std::forward<decltype(element)>(element)};
    ++index;
  }
}

long sum_weighted(auto&& numbered_range) {
  long sum = 0;
  for (auto [index, value] : numbered_range) sum += static_cast<long>(index) * value;
  return sum;
}

long sum_weighted_by_index(const std::vector<int>& values) {
  long sum = 0;
  for (std::size_t index = 0; index < values.size(); ++index) sum += static_cast<long>(index) * values[index];
  return sum;
}

struct Timing {
  double best_ns_per_element;
  long checksum;
};

// Changes one element before each pass and restores it after, so the compiler cannot reuse the previous
// pass's result and every variant ends with the same data.
Timing time_passes(std::vector<int>& values, auto&& pass) {
  Timing timing{.best_ns_per_element = std::numeric_limits<double>::max(), .checksum = 0};
  for (int repetition = 0; repetition < kPasses; ++repetition) {
    int& touched = values[static_cast<std::size_t>(repetition)];
    ++touched;
    const auto start = Clock::now();
    timing.checksum = pass();
    const std::chrono::duration<double, std::nano> elapsed = Clock::now() - start;
    --touched;
    timing.best_ns_per_element = std::min(timing.best_ns_per_element, elapsed.count() / kElements);
  }
  return timing;
}

int main() {
  std::vector<int> values = std::views::iota(std::size_t{0}, kElements) |
                            std::views::transform([](std::size_t i) {
                              return static_cast<int>((i * kStride + kSalt) % kValueRange);
                            }) |
                            std::ranges::to<std::vector>();

  const Timing alex = time_passes(values, [&] { return sum_weighted(enumerate_by_value(values)); });
  const Timing generic =
      time_passes(values, [&] { return sum_weighted(enumerate_generic(std::views::all(values))); });
  const Timing library = time_passes(values, [&] { return sum_weighted(std::views::enumerate(values)); });
  const Timing plain = time_passes(values, [&] { return sum_weighted_by_index(values); });

  std::cout << "ns per element, best of " << kPasses << " passes, salt " << kSalt << "\n"
            << "generator by value (Alex's shape): " << alex.best_ns_per_element << "\n"
            << "generic generator over views::all: " << generic.best_ns_per_element << "\n"
            << "views::enumerate: " << library.best_ns_per_element << "\n"
            << "index loop: " << plain.best_ns_per_element << "\n";
  return alex.checksum == generic.checksum && generic.checksum == library.checksum &&
                 library.checksum == plain.checksum
             ? 0
             : 1;
}

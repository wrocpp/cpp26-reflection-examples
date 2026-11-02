// Counts the heap allocations behind six ways of numbering the elements of a range, on a std::array<int, 4>
// and on a std::vector<int> of 1000. A replaced global operator new records every call and its bytes.
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <generator>
#include <iostream>
#include <new>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

namespace counting {
struct Tally {
  std::size_t calls;
  std::size_t bytes;
};

std::atomic<std::size_t> calls{0};
std::atomic<std::size_t> bytes{0};

// Read through a function that is never inlined: with GCC 15 and later a plain read of a counter can
// move across a replaced operator new (-fassume-sane-operators-new-delete).
[[gnu::noinline]] Tally snapshot() { return {.calls = calls.load(), .bytes = bytes.load()}; }
}  // namespace counting

void* operator new(std::size_t size) {
  counting::calls.fetch_add(1);
  counting::bytes.fetch_add(size);
  if (void* block = std::malloc(size)) return block;
  throw std::bad_alloc{};
}
void operator delete(void* block) noexcept { std::free(block); }
void operator delete(void* block, std::size_t) noexcept { std::free(block); }

constexpr std::size_t kSmallCount = 4;    // the size of the array in Alex's example
constexpr std::size_t kLargeCount = 1000; // our harness: a vector of this many ints

// 1. Alex's shape: the container is a by-value parameter, so the coroutine frame owns a copy.
std::generator<const std::tuple<int, int>> enumerate_by_value(auto container) {
  for (int index = 0; auto&& value : container) {
    co_yield std::tuple{index, value};
    ++index;
  }
}

// 2. The same body with a const reference parameter. It dangles when the argument is a temporary
//    that is gone before the generator is iterated. It is here only to be counted.
std::generator<const std::tuple<int, int>> enumerate_by_const_ref(const auto& container) {
  for (int index = 0; auto&& value : container) {
    co_yield std::tuple{index, value};
    ++index;
  }
}

// 3. Generic: yields the index and the range's own reference type. By value, so a view is cheap.
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

struct Result {
  const char* label;
  counting::Tally tally;
  long checksum;
};

Result measure(const char* label, auto&& body) {
  const counting::Tally before = counting::snapshot();
  const long checksum = body();
  const counting::Tally after = counting::snapshot();
  return {.label = label,
          .tally = {.calls = after.calls - before.calls, .bytes = after.bytes - before.bytes},
          .checksum = checksum};
}

bool checksums_agree(const std::vector<Result>& results) {
  return std::ranges::all_of(results, [&](const Result& r) { return r.checksum == results.front().checksum; });
}

void print(const std::vector<Result>& results) {
  for (const Result& result : results)
    std::cout << result.label << ": " << result.tally.calls << " allocations, " << result.tally.bytes << " bytes\n";
}

int main() {
  const std::array<int, kSmallCount> small{4, 3, 2, 1};
  const std::vector<int> large =
      std::views::iota(0, static_cast<int>(kLargeCount)) | std::ranges::to<std::vector>();

  const std::vector<Result> small_results{
      measure("generator by value, std::array<int, 4>", [&] { return sum_weighted(enumerate_by_value(small)); }),
      measure("views::enumerate, std::array<int, 4>", [&] { return sum_weighted(std::views::enumerate(small)); }),
  };
  const std::vector<Result> large_results{
      measure("generator by value, vector of 1000", [&] { return sum_weighted(enumerate_by_value(large)); }),
      measure("generator by const&, vector of 1000", [&] { return sum_weighted(enumerate_by_const_ref(large)); }),
      measure("generic generator, vector copied in", [&] { return sum_weighted(enumerate_generic(large)); }),
      measure("generic generator, views::all(vector)",
              [&] { return sum_weighted(enumerate_generic(std::views::all(large))); }),
      measure("views::enumerate, vector of 1000", [&] { return sum_weighted(std::views::enumerate(large)); }),
      measure("index loop, vector of 1000", [&] { return sum_weighted_by_index(large); }),
  };

  print(small_results);
  print(large_results);
  return checksums_agree(small_results) && checksums_agree(large_results) ? 0 : 1;
}

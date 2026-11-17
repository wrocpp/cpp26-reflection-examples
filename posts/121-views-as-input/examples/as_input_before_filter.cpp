// verify: gcc-only
// as_input_before_filter.cpp - views::as_input in front of filter turns a vector into an input-only range, so a
// following reverse no longer compiles.
//
// Build: g++ -std=c++26 as_input_before_filter.cpp   (needs <ranges> with views::as_input, __cpp_lib_ranges_as_input)
// Run:   ./a.out          prints the long names moved out of the vector in order. Exits 0.
//        ./a.out unsafe   runs the pipeline with filter then reverse then as_rvalue, which is undefined behavior
//                         (a heap-buffer-overflow under -fsanitize=address). Selected by argv so that the default run is clean.
// Limits: the unsafe mode is only a demonstration. Its result depends on the strings and the predicate. A library
//         without views::as_input (libc++ trunk on 2026-11-17) rejects the file.
#include <cstddef>
#include <cstdio>
#include <generator>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t kLongName = 5;
constexpr std::string_view kUnsafeArgument = "unsafe";

struct IsLong {
  bool operator()(const std::string& name) const { return name.size() > kLongName; }
};

template <class R>
concept can_reverse = requires(R range) { std::views::reverse(std::forward<R>(range)); };

using Names = std::vector<std::string>;
using PlainFilter = decltype(std::declval<Names&>() | std::views::filter(IsLong{}));
using SafeFilter = decltype(std::declval<Names&>() | std::views::as_input | std::views::filter(IsLong{}));
using GeneratorFilter = decltype(std::declval<std::generator<std::string>>() | std::views::filter(IsLong{}));

// A filter over a vector is bidirectional, so reverse accepts it.
static_assert(std::ranges::bidirectional_range<PlainFilter>);
static_assert(can_reverse<PlainFilter>);

// The same filter behind as_input is input-only, like a filter over a generator, so reverse is refused.
static_assert(std::ranges::input_range<SafeFilter>);
static_assert(!std::ranges::forward_range<SafeFilter>);
static_assert(!can_reverse<SafeFilter>);
static_assert(!std::ranges::forward_range<GeneratorFilter>);
static_assert(!can_reverse<GeneratorFilter>);

Names make_names() { return {"Amsterdam", "Berlin", "Cologne", "LA"}; }

void print(std::string_view label, const Names& names) {
  std::printf("%.*s:", static_cast<int>(label.size()), label.data());
  for (const auto& name : names) std::printf(" %s", name.c_str());
  std::printf("\n");
}

Names move_long_names_in_order(Names& names) {
  return names | std::views::as_input | std::views::filter(IsLong{}) | std::views::as_rvalue | std::ranges::to<Names>();
}

Names move_long_names_reversed(Names& names) {
  return names | std::views::filter(IsLong{}) | std::views::reverse | std::views::as_rvalue | std::ranges::to<Names>();
}

}  // namespace

int main(int argc, char** argv) {
  auto names = make_names();
  if (argc > 1 && std::string_view{argv[1]} == kUnsafeArgument) {
    print("reversed", move_long_names_reversed(names));
    return 0;
  }
  print("moved in order", move_long_names_in_order(names));
  return 0;
}

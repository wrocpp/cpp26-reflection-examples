// Fields of a struct with nested aggregates, yielded lazily with a dotted path. One helper coroutine per
// member, and std::ranges::elements_of to hand the inner generator's values to the outer one.
// Prints the values, then the heap allocations of one full walk from a replaced operator new.
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <generator>
#include <meta>
#include <new>
#include <print>
#include <ranges>
#include <string>
#include <type_traits>
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

template <class T>
constexpr auto members = std::define_static_array(
    std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()));

template <class T>
concept Aggregate = std::is_aggregate_v<T> && !std::is_array_v<T>;

using Field = std::pair<std::string, std::string>;

template <class T>
std::generator<Field> walk(const T& object, std::string prefix);

template <class T, std::size_t Index>
std::generator<Field> member_fields(const T& object, std::string prefix) {
  using Member = [:std::meta::type_of(members<T>[Index]):];
  const auto path = prefix + std::string{std::meta::identifier_of(members<T>[Index])};
  const auto& value = object.[:members<T>[Index]:];
  if constexpr (Aggregate<Member>) {
    co_yield std::ranges::elements_of(walk(value, path + "."));
  } else {
    co_yield {path, std::format("{}", value)};
  }
}

template <class T, std::size_t... Indices>
std::generator<Field> walk_impl(const T& object, std::string prefix, std::index_sequence<Indices...>) {
  (co_yield std::ranges::elements_of(member_fields<T, Indices>(object, prefix)), ...);
}

template <class T>
std::generator<Field> walk(const T& object, std::string prefix) {
  return walk_impl(object, std::move(prefix), std::make_index_sequence<members<T>.size()>{});
}

struct Inner {
  int x = 1;
  int y = 2;
};
struct Outer {
  Inner inner;
  double d = 3.5;
  std::string name = "o";
  Inner other{.x = 4, .y = 5};
};

int main() {
  const Outer outer;
  for (const auto& [path, value] : walk(outer, "")) std::println("{} = {}", path, value);
  const auto before = counting::snapshot();
  std::size_t count = 0;
  for (const auto& field : walk(outer, "")) count += field.first.size() != 0;
  const auto after = counting::snapshot();
  std::println("{} fields, {} allocations, {} bytes", count, after.calls - before.calls, after.bytes - before.bytes);
}

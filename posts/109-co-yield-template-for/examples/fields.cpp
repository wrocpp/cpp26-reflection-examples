// A generator over the non-static data members of a struct. The members are reflected into a constexpr
// array outside the coroutine, and an index_sequence fold yields one pair per member. The last line is the
// heap allocations of one full walk, from a replaced operator new.
// -DSHOW_TEMPLATE_FOR      co_yield inside `template for (constexpr auto member : members)`: does not compile
// -DSHOW_TEMPLATE_FOR_INDEX  the same over a size_t index: does not compile either
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <generator>
#include <meta>
#include <new>
#include <print>
#include <string>
#include <string_view>
#include <utility>

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

using Field = std::pair<std::string_view, std::string>;

template <class T>
constexpr auto members = std::define_static_array(
    std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()));

#if defined(SHOW_TEMPLATE_FOR)

template <class T>
std::generator<Field> fields(const T& object) {
  template for (constexpr auto member : members<T>)
    co_yield {std::meta::identifier_of(member), std::format("{}", object.[:member:])};
}

#elif defined(SHOW_TEMPLATE_FOR_INDEX)

template <class T>
std::generator<Field> fields(const T& object) {
  template for (constexpr std::size_t index : std::views::iota(0uz, members<T>.size()))
    co_yield {std::meta::identifier_of(members<T>[index]), std::format("{}", object.[:members<T>[index]:])};
}

#else

template <class T, std::size_t... Indices>
std::generator<Field> fields_impl(const T& object, std::index_sequence<Indices...>) {
  ((co_yield {std::meta::identifier_of(members<T>[Indices]), std::format("{}", object.[:members<T>[Indices]:])}), ...);
}

template <class T>
std::generator<Field> fields(const T& object) {
  return fields_impl(object, std::make_index_sequence<members<T>.size()>{});
}

#endif

struct Point {
  int x = 1;
  double y = 2.5;
  std::string name = "p";
};

int main() {
  for (const auto& [name, value] : fields(Point{})) std::println("{} = {}", name, value);
  const auto before = counting::snapshot();
  std::size_t count = 0;
  for (const auto& field : fields(Point{})) count += field.first.size() != 0;
  const auto after = counting::snapshot();
  std::println("{} fields, {} allocations, {} bytes", count, after.calls - before.calls, after.bytes - before.bytes);
}

// verify: gcc-only
// Two enumerate adaptors over a std::vector<int>: zip(iota, r), as in a C++23 range_adaptor_closure example, and the
// standard std::views::enumerate (P2164), plus an adaptor that bounds the iota by the size of the range. The program
// prints which range properties each one has, asserts them with static_assert, and counts the allocations of
// std::ranges::to<std::vector> with a replaced operator new, and the iterator steps it takes over a range that counts them.
// Build: g++ -std=c++26 -O1   (Compiler Explorer: g162; no -Wall, because GCC 16.2 reports -Wmismatched-new-delete on the
// replaced operators although the pair matches). The program has no macros and exits 0 on GCC 16.2.
// Limits: needs std::views::enumerate, so libc++ 22.1 rejects it with "no member named 'enumerate' in namespace
// 'std::ranges::views'". The index type of the hand-written adaptor is std::size_t, the alias the outside post declares (`using Index = std::size_t;`).
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <print>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace counters {
inline volatile std::size_t allocations = 0;  // volatile: GCC may otherwise assume a replaced operator new changes nothing
inline volatile std::size_t steps = 0;  // increments of the counting range's iterator
inline const void* volatile escaped = nullptr;  // publishing the buffer keeps the optimiser from dropping the allocation
}  // namespace counters

void* operator new(std::size_t size) {
  counters::allocations = counters::allocations + 1;
  if (void* p = std::malloc(size)) return p;
  throw std::bad_alloc{};
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

namespace r = std::ranges;
namespace v = std::views;

constexpr std::size_t kCount = 1000;  // elements in the vector whose allocations are counted
constexpr int kFirst = 10;

// A sized, common, random-access view over ints that counts how often its iterator steps forward.
struct Counted : r::view_interface<Counted> {
  int* first = nullptr;
  int* last = nullptr;

  struct It {
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    int* p = nullptr;
    int& operator*() const { return *p; }
    It& operator++() {
      counters::steps = counters::steps + 1;
      ++p;
      return *this;
    }
    It operator++(int) {
      It before = *this;
      ++*this;
      return before;
    }
    It& operator--() {
      --p;
      return *this;
    }
    It operator--(int) {
      It before = *this;
      --*this;
      return before;
    }
    It& operator+=(difference_type n) {
      counters::steps = counters::steps + 1;
      p += n;
      return *this;
    }
    It& operator-=(difference_type n) {
      p -= n;
      return *this;
    }
    friend It operator+(It i, difference_type n) { return i += n; }
    friend It operator+(difference_type n, It i) { return i += n; }
    friend It operator-(It i, difference_type n) { return i -= n; }
    friend difference_type operator-(const It& a, const It& b) { return a.p - b.p; }
    int& operator[](difference_type n) const { return p[n]; }
    friend bool operator==(const It& a, const It& b) { return a.p == b.p; }
    friend auto operator<=>(const It& a, const It& b) { return a.p <=> b.p; }
  };
  It begin() const { return {first}; }
  It end() const { return {last}; }
};
static_assert(r::view<Counted> && r::random_access_range<Counted> && r::sized_range<Counted> && r::common_range<Counted>);

template <class R>
concept Iterable = r::input_range<R> && r::viewable_range<R>;

struct Enumerate : r::range_adaptor_closure<Enumerate> {
  constexpr auto operator()(Iterable auto&& c) const {
    return v::zip(v::iota(std::size_t{0}), std::forward<decltype(c)>(c));
  }
};
inline constexpr Enumerate enumerate{};

// Bounds the iota by the size of the range, and counts with the range's difference type like views::enumerate.
struct EnumerateSized : r::range_adaptor_closure<EnumerateSized> {
  template <r::viewable_range R>
    requires r::sized_range<R>
  constexpr auto operator()(R&& c) const {
    using Index = r::range_difference_t<R>;
    return v::zip(v::iota(Index{0}, static_cast<Index>(r::size(c))), std::forward<R>(c));
  }
};
inline constexpr EnumerateSized enumerate_sized{};

using Data = std::vector<int>;
using Unbounded = decltype(std::declval<Data&>() | enumerate);
using Bounded = decltype(std::declval<Data&>() | enumerate_sized);
using Standard = decltype(std::declval<Data&>() | v::enumerate);

template <class V>
using IndexOf = std::remove_cvref_t<std::tuple_element_t<0, r::range_reference_t<V>>>;

// What the hand-written adaptor loses.
static_assert(!r::sized_range<Unbounded>);
static_assert(!r::common_range<Unbounded>);
static_assert(r::random_access_range<Unbounded>);
static_assert(r::borrowed_range<Unbounded>);
static_assert(std::is_same_v<IndexOf<Unbounded>, std::size_t>);
// What the standard adaptor has.
static_assert(r::sized_range<Standard>);
static_assert(r::common_range<Standard>);
static_assert(r::random_access_range<Standard>);
static_assert(r::borrowed_range<Standard>);
static_assert(std::is_same_v<IndexOf<Standard>, std::ptrdiff_t>);
// What the bounded adaptor gets back.
static_assert(r::sized_range<Bounded>);
static_assert(r::common_range<Bounded>);
static_assert(r::random_access_range<Bounded>);
static_assert(r::borrowed_range<Bounded>);
static_assert(std::is_same_v<IndexOf<Bounded>, std::ptrdiff_t>);
// What each can be asked.
template <class V>
concept HasSize = requires(V& view) { view.size(); };
template <class V>
concept IteratorPair = requires(V& view) { std::vector<std::tuple<int, int>>(view.begin(), view.end()); };
static_assert(!HasSize<Unbounded> && HasSize<Standard> && HasSize<Bounded>);
static_assert(!IteratorPair<Unbounded> && IteratorPair<Standard> && IteratorPair<Bounded>);

template <class V>
void describe(const char* name) {
  std::println("{:18} sized={} common={} random_access={} borrowed={} index_signed={}", name, r::sized_range<V>,
               r::common_range<V>, r::random_access_range<V>, r::borrowed_range<V>, std::is_signed_v<IndexOf<V>>);
}

struct Cost {
  std::size_t allocations;
  std::size_t steps;
};

template <class Adaptor, class Pair>
Cost cost_of_to(Counted& source, const Adaptor& adaptor) {
  counters::allocations = 0;
  counters::steps = 0;
  auto made = source | adaptor | r::to<std::vector<Pair>>();
  const Cost cost{.allocations = counters::allocations, .steps = counters::steps};
  counters::escaped = made.data();
  return cost;
}

int main() {
  Data three{kFirst, kFirst * 2, kFirst * 3};
  describe<Unbounded>("zip(iota, r)");
  describe<Standard>("views::enumerate");
  describe<Bounded>("zip(bounded iota)");
  for (auto&& [i, e] : three | enumerate) std::print("{}:{} ", i, e);
  for (auto&& [i, e] : three | v::enumerate) std::print("{}:{} ", i, e);
  for (auto&& [i, e] : three | enumerate_sized) std::print("{}:{} ", i, e);
  std::println("");
  std::println("size of views::enumerate = {}, of the bounded adaptor = {}", (three | v::enumerate).size(),
               (three | enumerate_sized).size());

  Data many(kCount, kFirst);
  Counted source{.first = many.data(), .last = many.data() + many.size()};
  const Cost unbounded = cost_of_to<Enumerate, std::tuple<std::size_t, int>>(source, enumerate);
  const Cost standard = cost_of_to<decltype(v::enumerate), std::tuple<std::ptrdiff_t, int>>(source, v::enumerate);
  const Cost bounded = cost_of_to<EnumerateSized, std::tuple<std::ptrdiff_t, int>>(source, enumerate_sized);
  std::println("ranges::to<vector> over {} elements, allocations and iterator steps:", kCount);
  std::println("  zip(iota, r)      allocations {}, steps {}", unbounded.allocations, unbounded.steps);
  std::println("  views::enumerate  allocations {}, steps {}", standard.allocations, standard.steps);
  std::println("  zip(bounded iota) allocations {}, steps {}", bounded.allocations, bounded.steps);
}

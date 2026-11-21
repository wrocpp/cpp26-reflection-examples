// verify: gcc-only
// A sketch of the defaults of any_view<Widget> from P3411R7: a type-erased view of Widget that is an input range,
// move-only, not const-iterable and never a common range. The example from the paper: a class returns a filtered
// view of its map without naming the pipeline type. This is not an implementation of the paper (it has no
// options, no size, no allocator support); each default it shares is a static_assert below.
// Build: g++ -std=c++26 -O1   (Compiler Explorer: g162). None of the library builds I tried on 2026-11-21 has
// std::ranges::any_view: see the README for the compilers and flags.
// Prints the widgets in stock. Exits 0 when the output is as expected. The program has no macros.
#include <cstddef>
#include <cstdio>
#include <iterator>
#include <map>
#include <memory>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>

template <class T>
class any_input_view : public std::ranges::view_interface<any_input_view<T>> {
  struct Base {
    virtual ~Base() = default;
    virtual bool done() = 0;
    virtual T& get() = 0;
    virtual void next() = 0;
  };
  template <std::ranges::input_range R>
  struct Model final : Base {
    R range;
    std::ranges::iterator_t<R> it = std::ranges::begin(range);
    explicit Model(R r) : range(std::move(r)) {}
    bool done() override { return it == std::ranges::end(range); }
    T& get() override { return *it; }
    void next() override { ++it; }
  };
  std::unique_ptr<Base> impl_;

 public:
  struct iterator {
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using iterator_concept = std::input_iterator_tag;
    Base* impl;
    T& operator*() const { return impl->get(); }
    iterator& operator++() { impl->next(); return *this; }
    void operator++(int) { impl->next(); }
    bool operator==(std::default_sentinel_t) const { return impl->done(); }
  };
  template <std::ranges::view R>
  any_input_view(R r) : impl_(std::make_unique<Model<R>>(std::move(r))) {}
  iterator begin() { return {impl_.get()}; }
  std::default_sentinel_t end() { return {}; }
};

constexpr int kBoltStock = 12;
constexpr int kNutStock = 3;

struct Widget {
  std::string name;
  int stock;
};

using InStock = any_input_view<Widget>;

static_assert(std::ranges::view<InStock>);
static_assert(std::ranges::input_range<InStock>);
static_assert(!std::ranges::forward_range<InStock>);
static_assert(!std::ranges::common_range<InStock>);
static_assert(!std::ranges::range<const InStock>);
static_assert(!std::is_copy_constructible_v<InStock>);

class Inventory {
  std::map<std::string, Widget> widgets_{
      {"bolt", {.name = "bolt", .stock = kBoltStock}},
      {"gear", {.name = "gear", .stock = 0}},
      {"nut", {.name = "nut", .stock = kNutStock}}};

 public:
  InStock in_stock() {
    return widgets_ | std::views::values | std::views::filter([](const Widget& w) { return w.stock > 0; });
  }
};

int main() {
  Inventory inventory;
  std::string names;
  for (const Widget& w : inventory.in_stock()) names += w.name + " ";
  std::printf("in stock: %s\n", names.c_str());
  return names == "bolt nut " ? 0 : 1;
}

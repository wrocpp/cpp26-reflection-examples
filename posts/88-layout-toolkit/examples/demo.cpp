// demo.cpp - report, reordered, budgets, conversions, split in one file (single-file form: make_single_files.sh writes demo_single.cpp)
#include "layout.hpp"
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <utility>

struct OrderBad { std::uint64_t price; char side; std::uint64_t quantity; bool active; };
struct OrderGood { std::uint64_t price; std::uint64_t quantity; char side; bool active; };

// (1) report: 32 bytes with 14 padding bytes, 24 bytes with 6
static_assert(sizeof(OrderBad) == 32 && layout::padding_bytes<OrderBad> == 14);
static_assert(sizeof(OrderGood) == 24 && layout::padding_bytes<OrderGood> == 6);
static_assert(layout::tail_padding<OrderBad> == 7 && layout::tail_padding<OrderGood> == 6);

// (2) reordered: sorted by descending alignment, predicted by hand from size_of / alignment_of
using Reordered = layout::reordered<OrderBad>;
static_assert(layout::predicted_reordered_size<OrderBad> == 24);
static_assert(sizeof(Reordered) == layout::predicted_reordered_size<OrderBad>);
static_assert(sizeof(Reordered) == 24 && layout::padding_bytes<Reordered> == 6);

// (4) budgets
static_assert(layout::padding_bytes<OrderGood> <= 6, layout::padding_report<OrderGood, 6>);
struct Cached {
  [[=layout::hot]] std::uint64_t price;
  [[=layout::cold]] char side;
  [[=layout::hot]] std::uint64_t quantity;
  [[=layout::hot]] bool active;
};
static_assert(layout::group<Cached, layout::hot_t>.span <= layout::constructive_size,
              layout::group_report<Cached, layout::hot_t, layout::constructive_size>);

// (5) split
using Split = layout::split<Cached>;
static_assert(sizeof(Split::hot) == 24 && sizeof(Split::cold) == 1);

// (3) conversions with move-only and heap members
struct Job { std::string name; char prio; std::unique_ptr<int> payload; std::uint64_t id; bool done; };

int main() {
  layout::print<OrderBad>();
  layout::print<OrderGood>();
  layout::print<Reordered>();
  std::printf("padding_bytes: OrderBad %zu, OrderGood %zu, reordered<OrderBad> %zu\n", layout::padding_bytes<OrderBad>,
              layout::padding_bytes<OrderGood>, layout::padding_bytes<Reordered>);
  std::printf("interference sizes: destructive %zu, constructive %zu\n", layout::destructive_size, layout::constructive_size);
  auto g = layout::group<Cached, layout::hot_t>;
  std::printf("hot group of Cached: %zu members, span %zu [%zu,%zu), payload %zu\n", g.count, g.span, g.first, g.end, g.payload);
  std::printf("split<Cached>: hot %zu bytes, cold %zu bytes\n", sizeof(Split::hot), sizeof(Split::cold));

  layout::split_table<Cached> tab;
  layout::push(tab, Cached{7, 'b', 3, true});
  layout::push(tab, Cached{9, 's', 4, false});
  Cached back = layout::join(tab, 1);
  std::printf("split round trip: hot[0]=(%llu,%llu,%d) cold[0]=%c join(1)=(%llu,%c,%llu,%d)\n", (unsigned long long)tab.hot[0].price,
              (unsigned long long)tab.hot[0].quantity, (int)tab.hot[0].active, tab.cold[0].side, (unsigned long long)back.price, back.side,
              (unsigned long long)back.quantity, (int)back.active);

  Job j{"a name long enough to live on the heap, not in the SSO buffer", 'h', std::make_unique<int>(42), 77, true};
  const char* heap_before = j.name.data();
  int* payload_before = j.payload.get();
  auto r = layout::to_reordered(std::move(j));
  std::printf("to_reordered(Job): sizeof Job %zu, reordered %zu; string buffer moved: %d, unique_ptr moved: %d, source name empty: %d\n",
              sizeof(Job), sizeof(r), r.name.data() == heap_before, r.payload.get() == payload_before, (int)j.name.empty());
  Job k = layout::from_reordered(std::move(r));
  std::printf("from_reordered: name ok %d, prio %c, payload %d, id %llu, done %d, same buffer %d\n",
              k.name == "a name long enough to live on the heap, not in the SSO buffer", k.prio, *k.payload, (unsigned long long)k.id,
              (int)k.done, k.name.data() == heap_before);
  OrderBad ob{1, 'x', 2, true};
  auto rb = layout::to_reordered(ob);
  OrderBad ob2 = layout::from_reordered(rb);  // copies (lvalue source)
  std::printf("copy round trip OrderBad: %d\n", ob.price == ob2.price && ob.side == ob2.side && ob.quantity == ob2.quantity && ob.active == ob2.active);
}

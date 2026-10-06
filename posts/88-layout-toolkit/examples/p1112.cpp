// p1112.cpp - how layout::reordered<T> DIFFERS from T. P1112R5 ("Reflection will not solve this") says a reordered copy is a different
// type; this prints every difference the language lets us observe.
#include "layout.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <compare>
#include <type_traits>

struct OrderBad { std::uint64_t price; char side; std::uint64_t quantity; bool active; };
struct OrderCmp {
  std::uint64_t price; char side; std::uint64_t quantity; bool active;
  auto operator<=>(OrderCmp const&) const = default;  // lexicographic: price, side, quantity, active
};
using R = layout::reordered<OrderBad>;
using RC = layout::reordered<OrderCmp>;

// lexicographic comparison in the type's OWN member order (what a defaulted <=> does), written with reflection so it can run on both
template <class T>
constexpr int compare_in_member_order(T const& a, T const& b) {
  int result = 0;
  template for (constexpr auto m : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, layout::ctx))) {
    if (result == 0) result = a.[:m:] < b.[:m:] ? -1 : (b.[:m:] < a.[:m:] ? 1 : 0);
  }
  return result;
}
static_assert(compare_in_member_order(OrderCmp{1, 'a', 9, true}, OrderCmp{1, 'b', 3, true}) == -1);  // same answer as the defaulted <=>
static_assert((OrderCmp{1, 'a', 9, true} <=> OrderCmp{1, 'b', 3, true}) < 0);
static_assert(!std::is_same_v<OrderBad, R> && !std::is_layout_compatible_v<OrderBad, R>);
static_assert(std::is_aggregate_v<R> && std::is_standard_layout_v<R> && std::is_trivially_copyable_v<R>);

template <class A> constexpr bool has_eq = requires(A a, A b) { a == b; };
static void hex(char const* label, void const* p, std::size_t n) {
  std::printf("  %-18s", label);
  for (std::size_t i = 0; i < n; ++i) std::printf("%02x%s", static_cast<unsigned char const*>(p)[i], i % 8 == 7 ? " " : "");
  std::printf("\n");
}

int main() {
  OrderBad x{.price = 100, .side = 'B', .quantity = 7, .active = true};
  R r = layout::to_reordered(x);
  std::printf("1. size and offsets: sizeof OrderBad %zu, sizeof reordered %zu; alignof %zu / %zu\n", sizeof(OrderBad), sizeof(R), alignof(OrderBad), alignof(R));
  for (auto const& row : layout::rows<OrderBad>) std::printf("   OrderBad.%-9s offset %zu\n", row.name, row.offset);
  for (auto const& row : layout::rows<R>) std::printf("   reordered.%-8s offset %zu\n", row.name, row.offset);
  std::printf("2. designated initialisers: a designated-initialiser list must follow the member order of the type it initialises\n");
  R r2{.price = 1, .quantity = 2, .side = 's', .active = true};  // reordered order compiles on R; fail_desig.cpp shows OrderBad's order is an error on R
  std::printf("   reordered{.price,.quantity,.side,.active} compiles; the original's order does not (see fail_desig.cpp)\n");
  OrderBad p1{100, 'B', 7, true};
  R p2{100, 'B', 7, true};  // positional: same tokens, compiles, silently different meaning
  std::printf("3. positional init {100,'B',7,true}: OrderBad price=%llu side=%c quantity=%llu active=%d | reordered price=%llu quantity=%llu side=%d active=%d\n",
              (unsigned long long)p1.price, p1.side, (unsigned long long)p1.quantity, (int)p1.active, (unsigned long long)p2.price,
              (unsigned long long)p2.quantity, (int)p2.side, (int)p2.active);
  auto [a1, b1, c1, d1] = x;
  auto [a2, b2, c2, d2] = r;
  std::printf("4. structured bindings auto [a,b,c,d]: OrderBad b=%c c=%llu | reordered b=%llu c=%d (same names, other members)\n", b1,
              (unsigned long long)c1, (unsigned long long)b2, (int)c2);
  (void)a1; (void)d1; (void)a2; (void)d2; (void)r2;
  OrderCmp lo{1, 'a', 9, true}, hi{1, 'b', 3, true};
  RC rlo = layout::to_reordered(lo), rhi = layout::to_reordered(hi);
  std::printf("5. ordering: defaulted <=> on OrderCmp says lo<hi: %d; same rule written by reflection on reordered says lo<hi: %d (it compares quantity before side)\n",
              (lo <=> hi) < 0, compare_in_member_order(rlo, rhi) < 0);
  constexpr bool original_has_eq = has_eq<OrderCmp>;
  constexpr bool reordered_has_eq = has_eq<RC>;
  std::printf("   operator== / <=> exist on OrderCmp: %d, on reordered<OrderCmp>: %d (define_aggregate injects no functions)\n", original_has_eq, reordered_has_eq);
  alignas(8) unsigned char bx[sizeof(OrderBad)], br[sizeof(R)];
  std::memset(bx, 0, sizeof bx); std::memset(br, 0, sizeof br);
  std::memcpy(bx, &x, sizeof x); std::memcpy(br, &r, sizeof r);  // padding copied as is
  std::printf("6. object representation (same values, bytes shown after zeroing the buffers first):\n");
  hex("OrderBad", bx, sizeof bx);
  hex("reordered", br, sizeof br);
  std::printf("   memcmp over the first %zu bytes: %d; has_unique_object_representations: OrderBad %d, reordered %d (both have padding)\n", sizeof(R),
              std::memcmp(bx, br, sizeof(R)), (int)std::has_unique_object_representations_v<OrderBad>, (int)std::has_unique_object_representations_v<R>);
  std::printf("7. type relations: is_same %d, is_layout_compatible %d, convertible R->OrderBad %d, constructible OrderBad from R %d (conversion is layout::from_reordered)\n",
              (int)std::is_same_v<OrderBad, R>, (int)std::is_layout_compatible_v<OrderBad, R>, (int)std::is_convertible_v<R, OrderBad>,
              (int)std::is_constructible_v<OrderBad, R>);
}

// limits.cpp - one example per member kind: what layout::reordered does (supports it, or refuses with a message)
#include "layout.hpp"
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>

consteval bool contains(std::string_view hay, std::string_view needle) { return hay.find(needle) != std::string_view::npos; }

// supported: over-aligned member (alignas on the member): sorted by ITS alignment, alignment copied
struct Over { char a; alignas(16) int b; short c; char d; };
using OverR = layout::reordered<Over>;
static_assert(sizeof(Over) == 48 || sizeof(Over) == 32);  // recorded below, printed at run time
static_assert(alignof(OverR) == 16 && sizeof(OverR) == layout::predicted_reordered_size<Over>);
static_assert(layout::padding_bytes<OverR> <= layout::padding_bytes<Over>);

// supported: reference and const members (aggregate init, no assignment needed)
struct RefConst { char a; int& r; const int c; bool b; };
using RefConstR = layout::reordered<RefConst>;
static_assert(sizeof(RefConstR) == layout::predicted_reordered_size<RefConst>);

// supported: a nested struct moves as one opaque member (its inside is not reordered)
struct Inner { char x; std::uint64_t y; char z; };
struct Outer { char a; Inner in; char b; std::uint32_t c; };
using OuterR = layout::reordered<Outer>;
static_assert(sizeof(OuterR) == layout::predicted_reordered_size<Outer>);
static_assert(sizeof(Inner) == 24);

// supported: a std::string member, an empty struct member (not no_unique_address)
struct Empty {};
struct WithStr { char a; std::string s; Empty e; short b; };
using WithStrR = layout::reordered<WithStr>;
static_assert(sizeof(WithStrR) == layout::predicted_reordered_size<WithStr>);

// refused: each with its message
struct BitF { std::uint8_t a; unsigned b : 3; std::uint64_t c; };
static_assert(!layout::reorder_diag<BitF>.empty() && contains(layout::reorder_diag<BitF>, "bit-field"));
struct Dmi { char a; std::uint64_t b = 7; };
static_assert(contains(layout::reorder_diag<Dmi>, "default member initializer"));
struct Base { int x; };
struct Derived : Base { char a; std::uint64_t b; };
static_assert(contains(layout::reorder_diag<Derived>, "base class"));
struct Nua { char a; [[no_unique_address]] Empty e; std::uint64_t b; };
static_assert(contains(layout::reorder_diag<Nua>, "overlap"));
struct Priv { private: char a; std::uint64_t b; };
static_assert(contains(layout::reorder_diag<Priv>, "not public"));
struct alignas(64) TypeAl { char a; std::uint64_t b; };
static_assert(contains(layout::reorder_diag<TypeAl>, "alignas"));
struct Mut { char a; mutable std::uint64_t b; };
static_assert(contains(layout::reorder_diag<Mut>, "mutable"));
union U { int a; char b; };
static_assert(contains(layout::reorder_diag<U>, "only a struct"));
static_assert(layout::reorder_diag<Over>.empty() && layout::reorder_diag<RefConst>.empty() && layout::reorder_diag<Outer>.empty() &&
              layout::reorder_diag<WithStr>.empty());

// define_aggregate itself accepts bit_width, alignment, reference, const and no_unique_address specs (the tool refuses bit-fields by choice)
template <class T> struct raw { struct type; consteval { std::meta::define_aggregate(^^type, {
  std::meta::data_member_spec(^^char, {.name = "a"}),
  std::meta::data_member_spec(^^unsigned, {.name = "b", .bit_width = 3}),
  std::meta::data_member_spec(^^int, {.name = "c", .alignment = 16}),
  std::meta::data_member_spec(^^char, {.name = "d", .no_unique_address = true})}); } };
static_assert(sizeof(raw<int>::type) == 16 || sizeof(raw<int>::type) == 32);

int main() {
  std::printf("Over: sizeof %zu -> reordered %zu (padding %zu -> %zu)\n", sizeof(Over), sizeof(OverR), layout::padding_bytes<Over>, layout::padding_bytes<OverR>);
  std::printf("RefConst: sizeof %zu -> reordered %zu\n", sizeof(RefConst), sizeof(RefConstR));
  std::printf("Outer: sizeof %zu -> reordered %zu (Inner stays %zu)\n", sizeof(Outer), sizeof(OuterR), sizeof(Inner));
  std::printf("WithStr: sizeof %zu -> reordered %zu\n", sizeof(WithStr), sizeof(WithStrR));
  std::printf("raw define_aggregate with bit_width/alignment/no_unique_address: sizeof %zu\n", sizeof(raw<int>::type));
  int v = 5;
  RefConst rc{'x', v, 9, true};
  auto r = layout::to_reordered(rc);
  RefConst back = layout::from_reordered(r);
  std::printf("RefConst round trip: a=%c r aliases v=%d c=%d b=%d\n", back.a, &back.r == &v, back.c, (int)back.b);
  Outer o{'p', {'x', 1, 'z'}, 'q', 7};
  Outer ob = layout::from_reordered(layout::to_reordered(o));
  std::printf("Outer round trip: %d\n", ob.a == 'p' && ob.in.y == 1 && ob.b == 'q' && ob.c == 7);
  for (auto d : {layout::reorder_diag<BitF>, layout::reorder_diag<Dmi>, layout::reorder_diag<Derived>, layout::reorder_diag<Nua>,
                 layout::reorder_diag<Priv>, layout::reorder_diag<TypeAl>, layout::reorder_diag<Mut>, layout::reorder_diag<U>})
    std::fputs(d.data(), stdout);
}

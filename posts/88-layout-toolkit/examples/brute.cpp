// brute.cpp - is "descending alignment" minimal for padding? Brute force over permutations.
//  A: consteval, all 24 orderings of sizes 1,2,4,8 and all 120 orderings with a 16-byte aligned member; every ordering is ALSO built as a
//     real struct (define_aggregate) and its sizeof must equal the ABI arithmetic; the sorted result must equal the minimum.
//  B: run time, every multiset of up to 5 members from a catalogue, with and without over-aligned members (size < alignment).
#include "layout.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <numeric>

using layout::field;

template <std::size_t N>
consteval std::array<field, N> nth_permutation(std::array<field, N> base, std::size_t k) {
  std::array<std::size_t, N> idx{};
  std::iota(idx.begin(), idx.end(), 0);
  for (std::size_t i = 0; i < k; ++i) std::next_permutation(idx.begin(), idx.end());
  std::array<field, N> out{};
  for (std::size_t i = 0; i < N; ++i) out[i] = base[idx[i]];
  return out;
}
template <std::size_t N>
constexpr std::array<field, N> sorted_desc(std::array<field, N> v) {  // stable, by alignment
  std::stable_sort(v.begin(), v.end(), [](field a, field b) { return a.align > b.align; });
  return v;
}
template <std::size_t N>
consteval std::size_t minimum_size(std::array<field, N> base) {  // base is in ascending lexicographic index order
  std::array<std::size_t, N> idx{};
  std::iota(idx.begin(), idx.end(), 0);
  std::size_t best = ~std::size_t{0};
  do {
    std::array<field, N> p{};
    for (std::size_t i = 0; i < N; ++i) p[i] = base[idx[i]];
    best = std::min(best, layout::size_in_order(p));
  } while (std::next_permutation(idx.begin(), idx.end()));
  return best;
}
template <std::size_t N>
consteval std::size_t count_minimal(std::array<field, N> base) {
  std::array<std::size_t, N> idx{};
  std::iota(idx.begin(), idx.end(), 0);
  std::size_t best = minimum_size(base), n = 0;
  do {
    std::array<field, N> p{};
    for (std::size_t i = 0; i < N; ++i) p[i] = base[idx[i]];
    n += layout::size_in_order(p) == best;
  } while (std::next_permutation(idx.begin(), idx.end()));
  return n;
}

struct alignas(16) V16 { char b[16]; };
template <std::size_t N> inline constexpr std::array<std::meta::info, N> types_v{};
template <> inline constexpr std::array<std::meta::info, 4> types_v<4> = {^^char, ^^short, ^^int, ^^long long};
template <> inline constexpr std::array<std::meta::info, 5> types_v<5> = {^^char, ^^short, ^^int, ^^long long, ^^V16};
template <std::size_t N> inline constexpr std::array<field, N> fields_v{};
template <> inline constexpr std::array<field, 4> fields_v<4> = {{{1, 1}, {2, 2}, {4, 4}, {8, 8}}};
template <> inline constexpr std::array<field, 5> fields_v<5> = {{{1, 1}, {2, 2}, {4, 4}, {8, 8}, {16, 16}}};
constexpr std::size_t factorial(std::size_t n) { return n <= 1 ? 1 : n * factorial(n - 1); }

template <std::size_t N, std::size_t K>
consteval std::vector<std::meta::info> perm_specs() {
  std::array<std::size_t, N> idx{};
  std::iota(idx.begin(), idx.end(), 0);
  for (std::size_t i = 0; i < K; ++i) std::next_permutation(idx.begin(), idx.end());
  std::vector<std::meta::info> v;
  for (std::size_t i = 0; i < N; ++i)
    v.push_back(std::meta::data_member_spec(types_v<N>[idx[i]], {.name = std::define_static_string("m" + layout::detail::itos(i))}));
  return v;
}
template <std::size_t N, std::size_t K>
struct perm {
  struct type;
  consteval { std::meta::define_aggregate(^^type, perm_specs<N, K>()); }
};

// the ABI arithmetic equals the compiler's layout for every ordering, and the sorted ordering is a minimum, through the real tool
template <std::size_t N>
constexpr bool arithmetic_matches_compiler() {
  return []<std::size_t... K>(std::index_sequence<K...>) {
    return ((sizeof(typename perm<N, K>::type) == layout::size_in_order(nth_permutation(fields_v<N>, K))) && ...);
  }(std::make_index_sequence<factorial(N)>{});
}
template <std::size_t N>
constexpr bool reordered_is_minimal_for_every_input_order() {
  return []<std::size_t... K>(std::index_sequence<K...>) {
    return ((sizeof(layout::reordered<typename perm<N, K>::type>) == minimum_size(fields_v<N>)) && ...);
  }(std::make_index_sequence<factorial(N)>{});
}
static_assert(arithmetic_matches_compiler<4>(), "24 orderings of 1,2,4,8: ABI arithmetic equals sizeof");
static_assert(arithmetic_matches_compiler<5>(), "120 orderings with a 16-byte aligned member: ABI arithmetic equals sizeof");
static_assert(layout::size_in_order(sorted_desc(fields_v<4>)) == minimum_size(fields_v<4>));
static_assert(layout::size_in_order(sorted_desc(fields_v<5>)) == minimum_size(fields_v<5>));
static_assert(reordered_is_minimal_for_every_input_order<4>());
static_assert(reordered_is_minimal_for_every_input_order<5>());
static_assert(minimum_size(fields_v<4>) == 16 && minimum_size(fields_v<5>) == 32);

// B: a counterexample once size < alignment (alignas on a member): the gap after the 16-aligned int is free space for the char
struct Ce { alignas(16) int a; std::uint64_t b; char c; };
struct CeBest { alignas(16) int a; char c; std::uint64_t b; };
static_assert(sizeof(Ce) == 32 && sizeof(CeBest) == 16);
static_assert(sizeof(layout::reordered<Ce>) == 32);  // descending alignment gives a, b, c: not minimal here

struct item { std::size_t size, align; const char* what; };
static bool run_catalogue(std::vector<item> const& cat, const char* label, std::size_t max_items) {
  std::size_t tested = 0, bad = 0;
  std::vector<std::size_t> pick;
  item first_bad_items[8];
  std::size_t first_bad_n = 0, first_sorted = 0, first_best = 0;
  auto test = [&] {
    std::vector<std::size_t> idx(pick.size());
    std::iota(idx.begin(), idx.end(), 0);
    auto size_of_order = [&](std::vector<std::size_t> const& o) {
      std::size_t off = 0, ma = 1;
      for (auto i : o) { auto const& it = cat[pick[i]]; off = layout::round_up(off, it.align) + it.size; ma = std::max(ma, it.align); }
      return layout::round_up(off, ma);
    };
    std::size_t best = ~std::size_t{0};
    do best = std::min(best, size_of_order(idx)); while (std::next_permutation(idx.begin(), idx.end()));
    std::iota(idx.begin(), idx.end(), 0);
    std::stable_sort(idx.begin(), idx.end(), [&](std::size_t a, std::size_t b) { return cat[pick[a]].align > cat[pick[b]].align; });
    std::size_t sorted = size_of_order(idx);
    ++tested;
    if (sorted != best) {
      if (!bad) { first_bad_n = pick.size(); for (std::size_t i = 0; i < pick.size() && i < 8; ++i) first_bad_items[i] = cat[pick[i]]; first_sorted = sorted; first_best = best; }
      ++bad;
    }
  };
  auto rec = [&](auto&& self, std::size_t start, std::size_t left) -> void {
    if (pick.size() >= 2) test();
    if (!left) return;
    for (std::size_t i = start; i < cat.size(); ++i) { pick.push_back(i); self(self, i, left - 1); pick.pop_back(); }
  };
  rec(rec, 0, max_items);
  std::printf("%s: %zu multisets of 2..%zu members, %zu with sorted != minimum\n", label, tested, max_items, bad);
  if (bad) {
    std::printf("  first counterexample: ");
    for (std::size_t i = 0; i < first_bad_n; ++i) std::printf("%s(size %zu,align %zu) ", first_bad_items[i].what, first_bad_items[i].size, first_bad_items[i].align);
    std::printf("-> sorted %zu bytes, minimum %zu\n", first_sorted, first_best);
  }
  return bad == 0;
}

int main() {
  std::printf("A: sizes 1,2,4,8: minimum %zu bytes, reached by %zu of 24 orderings; sorted gives %zu\n", minimum_size(fields_v<4>), count_minimal(fields_v<4>),
              layout::size_in_order(sorted_desc(fields_v<4>)));
  std::printf("A: with a 16-byte aligned member: minimum %zu bytes, reached by %zu of 120 orderings; sorted gives %zu\n", minimum_size(fields_v<5>),
              count_minimal(fields_v<5>), layout::size_in_order(sorted_desc(fields_v<5>)));
  std::printf("A: all 24 + 120 orderings built as real structs: sizeof == ABI arithmetic (static_assert), reordered<each> == minimum (static_assert)\n");
  std::printf("B: counterexample type Ce {alignas(16) int a; uint64 b; char c}: sizeof %zu, reordered %zu, hand order (a,c,b) %zu\n", sizeof(Ce),
              sizeof(layout::reordered<Ce>), sizeof(CeBest));
  std::vector<item> multiple = {{1, 1, "char"}, {2, 2, "short"}, {3, 1, "char[3]"}, {4, 4, "int"}, {6, 2, "short[3]"}, {8, 8, "u64"},
                                {12, 4, "int[3]"}, {16, 8, "u64[2]"}, {16, 16, "V16"}, {24, 8, "u64[3]"}};
  bool ok1 = run_catalogue(multiple, "B1 sizes multiples of alignments", 5);
  std::vector<item> with_over = multiple;
  with_over.push_back({4, 16, "alignas16 int"});
  with_over.push_back({1, 8, "alignas8 char"});
  with_over.push_back({8, 16, "alignas16 u64"});
  with_over.push_back({2, 4, "alignas4 short"});
  bool ok2 = run_catalogue(with_over, "B2 with over-aligned members (size < alignment)", 5);
  return ok1 && !ok2 ? 0 : 1;  // recorded: B1 has no counterexample, B2 has some
}

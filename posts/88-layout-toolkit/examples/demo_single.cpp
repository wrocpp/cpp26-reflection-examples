// demo.cpp - report, reordered, budgets, conversions, split in one file (single-file form: make_single_files.sh writes demo_single.cpp)
// layout.hpp - a struct layout toolkit on C++26 reflection (GCC 16.2, -std=c++26 -freflection).
//   report:    layout::explain<T>, layout::padding_bytes<T>, layout::print<T>()
//   reorder:   layout::reordered<T> (members by descending alignment), layout::reorder_diag<T>
//   convert:   layout::to_reordered(t), layout::from_reordered(r)
//   budgets:   layout::group<T, hot_t>, layout::padding_report<T, N>, layout::group_report<T, Tag, N>
//   split:     layout::split<T>::hot / ::cold, layout::split_table<T>, free-function accessors
// Everything that decides is consteval; the only run-time code is printing and the member-wise moves.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <meta>
#include <new>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace layout {
using std::meta::info;
inline constexpr auto ctx = std::meta::access_context::unchecked();

struct hot_t {};   // annotation: [[=layout::hot]] member belongs to the hot group
inline constexpr hot_t hot{};
struct cold_t {};  // annotation: [[=layout::cold]] member belongs to the cold group
inline constexpr cold_t cold{};

// ---- plain ABI arithmetic, usable at compile time and run time (no reflection) ----
struct field {
  std::size_t size, align;
};
constexpr std::size_t round_up(std::size_t v, std::size_t a) { return (v + a - 1) / a * a; }
template <std::size_t N>
constexpr std::size_t size_in_order(std::array<field, N> const& fs) {  // offsets rounded up to alignment, total rounded to max alignment
  std::size_t off = 0, max_align = 1;
  for (auto const& f : fs) {
    off = round_up(off, f.align) + f.size;
    max_align = std::max(max_align, f.align);
  }
  return round_up(off, max_align);
}

namespace detail {
consteval std::string itos(std::size_t v) {  // std::to_string is not constexpr in libstdc++ 16.2
  std::string s = v ? "" : "0";
  for (; v; v /= 10) s.insert(s.begin(), char('0' + v % 10));
  return s;
}
consteval std::string pad_r(std::string s, std::size_t w) { while (s.size() < w) s += ' '; return s; }
consteval std::string pad_l(std::string s, std::size_t w) { while (s.size() < w) s.insert(s.begin(), ' '); return s; }
consteval std::string type_name(info t) { return std::string(std::meta::display_string_of(t)); }

consteval std::vector<info> members(info t) { return std::meta::nonstatic_data_members_of(t, ctx); }
consteval bool has_annotation(info m, info type) {
  for (info a : std::meta::annotations_of(m))
    if (std::meta::remove_cvref(std::meta::type_of(a)) == type) return true;
  return false;
}

// byte range a member covers: bit-fields cover the bytes their bits touch (size_of is not defined for them)
consteval std::size_t member_begin(info m) { return std::meta::offset_of(m).bytes; }
consteval std::size_t member_end(info m) {
  if (std::meta::is_bit_field(m)) {
    auto o = std::meta::offset_of(m);
    return o.bytes + (o.bits + std::meta::bit_size_of(m) + 7) / 8;
  }
  return member_begin(m) + std::meta::size_of(m);
}
consteval std::size_t member_align(info m) {  // the member's own alignment (alignas included); bit-fields: that of the type
  return std::meta::is_bit_field(m) ? std::meta::alignment_of(std::meta::type_of(m)) : std::meta::alignment_of(m);
}

constexpr char letter_for(std::size_t i) {
  constexpr std::string_view letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  return i < letters.size() ? letters[i] : '#';
}

struct row_data {
  const char* name;
  const char* type;
  std::size_t offset, size, align, pad_before;
  bool bit_field;
  char letter;
};
struct analysis {
  std::vector<row_data> rows;
  std::size_t total = 0, align = 1, tail = 0, padding = 0;
  std::string map;
};

consteval analysis analyse(info t) {
  analysis a;
  a.total = std::meta::size_of(t);
  a.align = std::meta::alignment_of(t);
  a.map = std::string(a.total, '.');
  std::size_t prev_end = 0, idx = 0;
  auto add = [&](std::string name, std::string type, std::size_t b, std::size_t e, std::size_t al, bool bf, char letter) {
    a.rows.push_back({std::define_static_string(name), std::define_static_string(type), b, e - b, al,
                      b > prev_end ? b - prev_end : 0, bf, letter});
    for (std::size_t i = b; i < e && i < a.total; ++i) a.map[i] = letter;
    prev_end = std::max(prev_end, e);
  };
  for (info base : std::meta::bases_of(t, ctx)) {
    info bt = std::meta::type_of(base);
    std::size_t b = std::meta::offset_of(base).bytes;
    add("<base " + type_name(bt) + ">", type_name(bt), b, b + std::meta::size_of(bt), std::meta::alignment_of(bt), false, '^');
  }
  for (info m : members(t))
    add(std::string(std::meta::identifier_of(m)), type_name(std::meta::type_of(m)), member_begin(m), member_end(m), member_align(m),
        std::meta::is_bit_field(m), letter_for(idx++));
  a.tail = a.total > prev_end ? a.total - prev_end : 0;
  for (char c : a.map) a.padding += (c == '.');
  return a;
}

consteval std::string explain_text(info t) {
  analysis a = analyse(t);
  std::size_t wn = 6, wt = 4;
  for (auto const& r : a.rows) {
    wn = std::max(wn, std::string_view(r.name).size());
    wt = std::max(wt, std::string_view(r.type).size());
  }
  std::string s = type_name(t) + ": " + itos(a.total) + " bytes, align " + itos(a.align) + ", " + itos(a.padding) + " padding bytes (" +
                  itos(a.tail) + " of them tail)\n";
  s += "  " + pad_r("member", wn) + "  " + pad_r("type", wt) + "  off size align pad_before\n";
  for (auto const& r : a.rows)
    s += "  " + pad_r(r.name, wn) + "  " + pad_r(r.type, wt) + "  " + pad_l(itos(r.offset), 3) + " " + pad_l(itos(r.size), 4) + " " +
         pad_l(itos(r.align), 5) + " " + pad_l(itos(r.pad_before), 10) + (r.bit_field ? "  (bit-field)" : "") + "\n";
  s += "  map (one char per byte, '.' = padding, a space every 8 bytes):\n  ";
  for (std::size_t i = 0; i < a.map.size(); ++i) {
    if (i && i % 8 == 0) s += ' ';
    s += a.map[i];
  }
  s += "\n  legend:";
  for (auto const& r : a.rows) s += std::string(" ") + r.letter + "=" + r.name;
  s += "\n";
  return s;
}
}  // namespace detail

// ---- (1) report ----
template <class T> inline constexpr std::span<const detail::row_data> rows = std::define_static_array(detail::analyse(^^T).rows);
template <class T> inline constexpr std::string_view byte_map = std::define_static_string(detail::analyse(^^T).map);
template <class T> inline constexpr std::size_t padding_bytes = detail::analyse(^^T).padding;
template <class T> inline constexpr std::size_t tail_padding = detail::analyse(^^T).tail;
template <class T> inline constexpr std::string_view explain = std::define_static_string(detail::explain_text(^^T));
template <class T> void print() { std::fputs(explain<T>.data(), stdout); }

// ---- (2) reordered ----
namespace detail {
consteval std::string unsupported(info t, std::string_view who) {  // empty = every member kind is supported
  std::string d;
  std::string pre = std::string(who) + "<" + type_name(t) + ">: ";
  auto bad = [&](std::string why) { d += pre + why + " (refused)\n"; };
  if (!std::meta::is_class_type(t) || std::meta::is_union_type(t)) { bad("only a struct or class can be rebuilt"); return d; }
  if (!std::meta::bases_of(t, ctx).empty()) bad("it has a base class and define_aggregate adds data members only");
  auto ms = members(t);
  std::size_t max_align = 1;
  for (info m : ms) {
    std::string n = "member '" + std::string(std::meta::identifier_of(m)) + "' ";
    if (!std::meta::is_public(m)) bad(n + "is not public and the rebuilt members are public");
    if (std::meta::is_mutable_member(m)) bad(n + "is mutable and a synthesised member cannot be");
    if (std::meta::is_bit_field(m)) { bad(n + "is a bit-field and moving it would change how the bits pack"); continue; }
    if (std::meta::has_default_member_initializer(m)) bad(n + "has a default member initializer and define_aggregate drops it");
    max_align = std::max<std::size_t>(max_align, std::meta::alignment_of(m));
  }
  bool any_bf = false;
  for (info m : ms) any_bf = any_bf || std::meta::is_bit_field(m);
  if (any_bf) return d;
  for (std::size_t i = 0; i < ms.size(); ++i)
    for (std::size_t j = i + 1; j < ms.size(); ++j)
      if (member_begin(ms[i]) < member_end(ms[j]) && member_begin(ms[j]) < member_end(ms[i]))
        bad("members '" + std::string(std::meta::identifier_of(ms[i])) + "' and '" + std::string(std::meta::identifier_of(ms[j])) +
            "' overlap ([[no_unique_address]]) and the attribute cannot be copied");
  if (!ms.empty() && std::meta::alignment_of(t) != max_align) bad("the type has its own alignas and a synthesised type cannot");
  return d;
}

consteval std::vector<info> by_descending_alignment(info t) {  // stable: ties keep declaration order
  std::vector<info> v = members(t);
  for (std::size_t i = 1; i < v.size(); ++i)
    for (std::size_t j = i; j > 0 && member_align(v[j - 1]) < member_align(v[j]); --j) std::swap(v[j - 1], v[j]);
  return v;
}

consteval info spec_of(info m) {  // the same name, type and (over-)alignment
  std::optional<int> al;
  if (!std::meta::is_reference_type(std::meta::type_of(m)) && member_align(m) > std::meta::alignment_of(std::meta::type_of(m)))
    al = static_cast<int>(member_align(m));
  return std::meta::data_member_spec(std::meta::type_of(m), {.name = std::define_static_string(std::meta::identifier_of(m)), .alignment = al});
}

consteval std::vector<info> reordered_specs(info t, bool ok) {
  std::vector<info> specs;
  if (ok)
    for (info m : by_descending_alignment(t)) specs.push_back(spec_of(m));
  return specs;
}

consteval std::size_t predicted_size(info t) {  // from size_of / alignment_of of the members only
  std::size_t off = 0, max_align = 1;
  for (info m : by_descending_alignment(t)) {
    off = round_up(off, member_align(m)) + std::meta::size_of(m);
    max_align = std::max(max_align, member_align(m));
  }
  return members(t).empty() ? 1 : round_up(off, max_align);
}
}  // namespace detail

template <class T> inline constexpr std::string_view reorder_diag = std::define_static_string(detail::unsupported(^^T, "layout::reordered"));
template <class T> inline constexpr std::size_t predicted_reordered_size = detail::predicted_size(^^T);

template <class T>
struct reordered_holder {
  static_assert(reorder_diag<T>.empty(), reorder_diag<T>);
  struct type;
  consteval { std::meta::define_aggregate(^^type, detail::reordered_specs(^^T, reorder_diag<T>.empty())); }
};
template <class T> using reordered = reordered_holder<T>::type;
template <class R> using origin_t = [:std::meta::template_arguments_of(std::meta::parent_of(std::meta::dealias(^^R)))[0]:];

// ---- (3) conversions: by name, moving when the source is an rvalue ----
namespace detail {
consteval info find_member(info t, std::string_view name) {
  for (info m : members(t))
    if (std::meta::identifier_of(m) == name) return m;
  return info{};
}
consteval std::vector<info> matching_sources(info to, info from) {  // for each member of `to`, in order, the member of `from` with that name
  std::vector<info> v;
  for (info m : members(to)) v.push_back(find_member(from, std::meta::identifier_of(m)));
  return v;
}
template <info M, class U, class S>
constexpr decltype(auto) take(S& s) {
  if constexpr (std::meta::is_reference_type(std::meta::type_of(M))) return (s.[:M:]);  // a reference member is rebound, not moved
  else return std::forward_like<U>(s.[:M:]);
}
template <class To, class F> inline constexpr auto sources = std::define_static_array(matching_sources(^^To, ^^F));
template <class To, class From>
constexpr To convert(From&& f) {
  using F = std::remove_cvref_t<From>;
  static_assert(std::ranges::none_of(sources<To, F>, [](info i) { return i == info{}; }), "layout::convert: a member of the target has no namesake in the source");
  return [&]<std::size_t... I>(std::index_sequence<I...>) { return To{take<sources<To, F>[I], From>(f)...}; }(std::make_index_sequence<sources<To, F>.size()>{});
}
}  // namespace detail

template <class U>
constexpr auto to_reordered(U&& u) { return detail::convert<reordered<std::remove_cvref_t<U>>>(std::forward<U>(u)); }
template <class R>
constexpr auto from_reordered(R&& r) { return detail::convert<origin_t<std::remove_cvref_t<R>>>(std::forward<R>(r)); }

// ---- (4) budgets ----
// GCC fires -Winterference-size at every use of these two constants that sits in a header; the pragma states that this use is deliberate
// (the values are reported and budgeted against, not baked into an ABI).
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winterference-size"
inline constexpr std::size_t constructive_size = std::hardware_constructive_interference_size;
inline constexpr std::size_t destructive_size = std::hardware_destructive_interference_size;
#pragma GCC diagnostic pop

struct group_stats {
  std::size_t count, first, end, span, payload;  // span = end - first; payload = sum of member sizes
};
namespace detail {
consteval group_stats group_of(info t, info tag) {
  group_stats g{0, 0, 0, 0, 0};
  bool any = false;
  for (info m : members(t)) {
    if (!has_annotation(m, tag)) continue;
    std::size_t b = member_begin(m), e = member_end(m);
    g.first = any ? std::min(g.first, b) : b;
    g.end = any ? std::max(g.end, e) : e;
    any = true;
    ++g.count;
    g.payload += e - b;
  }
  g.span = g.end - g.first;
  return g;
}
consteval std::string group_text(info t, info tag, std::size_t budget) {
  group_stats g = group_of(t, tag);
  std::string s = "layout budget: group '" + type_name(tag) + "' of " + type_name(t) + " spans " + itos(g.span) + " bytes [" + itos(g.first) +
                  ", " + itos(g.end) + ") with " + itos(g.payload) + " payload bytes in " + itos(g.count) + " members; budget " + itos(budget) + "\n";
  for (info m : members(t))
    if (has_annotation(m, tag))
      s += "  " + std::string(std::meta::identifier_of(m)) + " at " + itos(member_begin(m)) + ", " + itos(member_end(m) - member_begin(m)) + " bytes\n";
  return s;
}
consteval std::string padding_text(info t, std::size_t budget) {
  return "layout budget: " + type_name(t) + " has " + itos(analyse(t).padding) + " padding bytes, budget " + itos(budget) + "\n" + explain_text(t);
}
}  // namespace detail
template <class T, class Tag> inline constexpr group_stats group = detail::group_of(^^T, ^^Tag);
template <class T, class Tag, std::size_t Budget> inline constexpr std::string_view group_report = std::define_static_string(detail::group_text(^^T, ^^Tag, Budget));
template <class T, std::size_t Budget> inline constexpr std::string_view padding_report = std::define_static_string(detail::padding_text(^^T, Budget));

// ---- (5) hot/cold split: data members only, parallel arrays (no index field) ----
namespace detail {
consteval bool is_hot(info m) { return has_annotation(m, ^^hot_t) && !has_annotation(m, ^^cold_t); }  // unannotated members are cold
consteval std::string split_diag(info t) {
  std::string d = unsupported(t, "layout::split");
  if (!std::meta::is_class_type(t) || std::meta::is_union_type(t)) return d;
  for (info m : members(t))
    if (has_annotation(m, ^^hot_t) && has_annotation(m, ^^cold_t))
      d += "layout::split<" + type_name(t) + ">: member '" + std::string(std::meta::identifier_of(m)) + "' is annotated both hot and cold (refused)\n";
  return d;
}
consteval std::vector<info> part_specs(info t, bool want_hot, bool ok) {
  std::vector<info> specs;
  if (ok)
    for (info m : members(t))
      if (is_hot(m) == want_hot) specs.push_back(spec_of(m));
  return specs;
}
}  // namespace detail
template <class T> inline constexpr std::string_view split_diag = std::define_static_string(detail::split_diag(^^T));

template <class T>
struct split {
  static_assert(split_diag<T>.empty(), split_diag<T>);
  struct hot;
  struct cold;
  consteval { std::meta::define_aggregate(^^hot, detail::part_specs(^^T, true, split_diag<T>.empty())); }
  consteval { std::meta::define_aggregate(^^cold, detail::part_specs(^^T, false, split_diag<T>.empty())); }
};

template <class T>
struct split_table {  // element i is hot[i] together with cold[i]
  std::vector<typename split<T>::hot> hot;
  std::vector<typename split<T>::cold> cold;
};

template <class T> inline constexpr auto members_v = std::define_static_array(detail::members(^^T));
namespace detail {
template <info M, class H, class C, class Table>
constexpr decltype(auto) join_one(Table const& t, std::size_t i) {
  if constexpr (is_hot(M)) return (t.hot[i].[:find_member(^^H, std::meta::identifier_of(M)):]);
  else return (t.cold[i].[:find_member(^^C, std::meta::identifier_of(M)):]);
}
}  // namespace detail

template <class T, class U>
void push(split_table<T>& t, U&& u) {
  t.hot.push_back(detail::convert<typename split<T>::hot>(u));    // by name; copies from an lvalue
  t.cold.push_back(detail::convert<typename split<T>::cold>(std::forward<U>(u)));
}
template <class T> auto& hot_of(split_table<T>& t, std::size_t i) { return t.hot[i]; }
template <class T> auto& cold_of(split_table<T>& t, std::size_t i) { return t.cold[i]; }
template <class T> T join(split_table<T> const& t, std::size_t i) {  // rebuild the original element (copies)
  using H = typename split<T>::hot;
  using C = typename split<T>::cold;
  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return T{detail::join_one<members_v<T>[I], H, C>(t, i)...};
  }(std::make_index_sequence<members_v<T>.size()>{});
}
}  // namespace layout
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

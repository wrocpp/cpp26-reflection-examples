// Structured bindings in a range-for: which loop forms copy the element, and what a
// copy does and does not protect when the loop body changes the container.
// Build: -std=c++23. The default program only runs forms that are safe, and exits 0.
// -DMATRIX_CASE selects one form that can be undefined (see the README).
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <new>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#if defined(__SANITIZE_ADDRESS__)
extern "C" const char* __asan_default_options() { return "exitcode=0"; }
#endif

namespace counters {
inline std::size_t payload_copies = 0;
inline std::size_t allocations = 0;
inline std::size_t bytes = 0;
inline void reset() { payload_copies = 0; allocations = 0; bytes = 0; }
}  // namespace counters

// Counting replacement. GCC 16 at -O2 reports free() against new here (-Wmismatched-new-delete),
// although the pair is matched, so the warning is switched off for these three definitions.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmismatched-new-delete"
#endif
void* operator new(std::size_t size) {
  ++counters::allocations;
  counters::bytes += size;
  if (void* p = std::malloc(size)) return p;
  throw std::bad_alloc{};
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

constexpr std::array<int, 3> kInitialIds{1, 2, 3};
constexpr int kPayloadInts = 4;
constexpr int kVictimId = 2;         // the element the callback removes
constexpr int kTriggerNext = 1;      // the element during which it removes the next one
constexpr int kInsertBase = 100;     // first key the callback inserts
constexpr int kInsertCount = 64;     // enough to force a rehash or a reallocation

struct Payload {
  std::vector<int> data;
  int tag;
  Payload(std::vector<int> d, int t) : data{std::move(d)}, tag{t} {}
  Payload(const Payload& o) : data{o.data}, tag{o.tag} { ++counters::payload_copies; }
  Payload(Payload&&) noexcept = default;
  Payload& operator=(const Payload&) = delete;
  Payload& operator=(Payload&&) noexcept = default;
};

Payload make_payload(int tag) { return Payload{std::vector<int>(kPayloadInts, tag), tag}; }

using OrderedMap = std::map<int, Payload>;
using HashedMap = std::unordered_map<int, Payload>;
using PairVector = std::vector<std::pair<int, Payload>>;

template <class C>
C make_registry() {
  C c;
  if constexpr (std::is_same_v<C, PairVector>) c.reserve(kInitialIds.size());  // capacity equals size
  for (int id : kInitialIds) {
    if constexpr (std::is_same_v<C, PairVector>) c.emplace_back(id, make_payload(id));
    else c.emplace(id, make_payload(id));
  }
  return c;
}

// ---- part 1: which loop forms copy -------------------------------------------------
template <class C> std::size_t copies_auto_binding(C& c) {
  counters::reset(); for (auto [id, p] : c) { (void)id; (void)p; } return counters::payload_copies; }
template <class C> std::size_t copies_const_auto_binding(C& c) {
  counters::reset(); for (const auto [id, p] : c) { (void)id; (void)p; } return counters::payload_copies; }
template <class C> std::size_t copies_const_ref_binding(C& c) {
  counters::reset(); for (const auto& [id, p] : c) { (void)id; (void)p; } return counters::payload_copies; }
template <class C> std::size_t copies_auto_entry(C& c) {
  counters::reset(); for (auto entry : c) { (void)entry; } return counters::payload_copies; }
template <class C> std::size_t copies_auto_ref(C& c) {
  counters::reset(); for (auto& entry : c) { (void)entry; } return counters::payload_copies; }
template <class C> std::size_t copies_forwarding_ref(C& c) {
  counters::reset(); for (auto&& entry : c) { (void)entry; } return counters::payload_copies; }
template <class C> std::size_t copies_values_by_value(C& c) {
  counters::reset(); for (auto p : c | std::views::values) { (void)p; } return counters::payload_copies; }
template <class C> std::size_t copies_values_const_ref(C& c) {
  counters::reset(); for (const auto& p : c | std::views::values) { (void)p; } return counters::payload_copies; }

template <class C>
void print_copy_counts(const char* name) {
  C c = make_registry<C>();
  std::printf("%-12s auto[id,p]=%zu const auto[id,p]=%zu const auto&[id,p]=%zu auto entry=%zu auto&=%zu auto&&=%zu"
              " values:auto=%zu values:const auto&=%zu\n",
              name, copies_auto_binding(c), copies_const_auto_binding(c), copies_const_ref_binding(c),
              copies_auto_entry(c), copies_auto_ref(c), copies_forwarding_ref(c),
              copies_values_by_value(c), copies_values_const_ref(c));
}

// ---- part 2: const on a structured binding -----------------------------------------
void print_const_binding() {
  int errors = 3;
  const auto [count] = std::tie(errors);
  ++count;
  const auto [owned] = std::make_tuple(errors);
  std::printf("tie: errors=%d decltype(count)=%s; make_tuple: decltype(owned)=%s\n", errors,
              std::is_same_v<decltype(count), int&> ? "int&" : "other",
              std::is_same_v<decltype(owned), const int> ? "const int" : "other");
}

// ---- part 2b: auto&& ----------------------------------------------------------------
std::size_t copies_forwarding_binding_const(const OrderedMap& c) {
  counters::reset(); for (auto&& [id, p] : c) { (void)id; (void)p; } return counters::payload_copies; }

constexpr auto make_pair_copy = [](const auto& e) { return std::pair<int, Payload>{e.first, e.second}; };

template <bool UseForwarding>
std::size_t copies_over_prvalue_pairs(OrderedMap& c) {
  counters::reset();
  auto view = c | std::views::transform(make_pair_copy);
  if constexpr (UseForwarding) { for (auto&& [id, p] : view) { (void)id; (void)p; } }
  else { for (auto [id, p] : view) { (void)id; (void)p; } }
  return counters::payload_copies;
}

void print_forwarding_ref() {
  OrderedMap c = make_registry<OrderedMap>();
  const std::size_t const_copies = copies_forwarding_binding_const(c);
  counters::reset();
  const int tag_before = c.at(1).tag;
  for (auto&& [id, p] : c) { if (id == 1) p.tag += 1; }
  const std::size_t mutable_copies = counters::payload_copies;
  std::printf("auto&& [id,p]: const map copies=%zu (p const: %s); non-const map copies=%zu, tag of key 1 went %d -> %d\n",
              const_copies, std::is_const_v<std::remove_reference_t<decltype(*std::declval<const OrderedMap&>().begin())>> ? "yes" : "no",
              mutable_copies, tag_before, c.at(1).tag);
  const std::size_t via_forwarding = copies_over_prvalue_pairs<true>(c);
  const std::size_t via_value = copies_over_prvalue_pairs<false>(c);
  std::printf("transform yielding pairs by value: auto&& [id,p]=%zu copies, auto [id,p]=%zu copies (all inside the lambda)\n",
              via_forwarding, via_value);
  std::vector<bool> flags(kInitialIds.size());
  for (auto&& f : flags) f = true;
  std::printf("vector<bool> with auto&& f = true: %zu of %zu set\n", static_cast<std::size_t>(std::ranges::count(flags, true)), flags.size());
}

// ---- part 3: the container changes while the loop runs ------------------------------
enum class Mode { EraseCurrent, EraseNext, Insert };

struct Op {
  enum class Kind { Erase, Insert } kind;
  int key;
};

std::vector<Op> plan_for(Mode mode, int id) {
  std::vector<Op> ops;
  if (mode == Mode::EraseCurrent && id == kVictimId) ops.push_back({.kind = Op::Kind::Erase, .key = kVictimId});
  if (mode == Mode::EraseNext && id == kTriggerNext) ops.push_back({.kind = Op::Kind::Erase, .key = kVictimId});
  if (mode == Mode::Insert && id == kTriggerNext)
    for (int i = 0; i < kInsertCount; ++i) ops.push_back({.kind = Op::Kind::Insert, .key = kInsertBase + i});
  return ops;
}

template <class C>
void apply_one(C& c, const Op& op) {
  if constexpr (std::is_same_v<C, PairVector>) {
    if (op.kind == Op::Kind::Erase) std::erase_if(c, [&](const auto& e) { return e.first == op.key; });
    else c.emplace_back(op.key, make_payload(op.key));
  } else {
    if (op.kind == Op::Kind::Erase) c.erase(op.key);
    else c.emplace(op.key, make_payload(op.key));
  }
}

template <class C>
void apply_ops(C& c, const std::vector<Op>& ops) {
  for (const Op& op : ops) apply_one(c, op);
}

struct Stats {
  int visited = 0;
  long checksum = 0;
};

void touch(Stats& s, int id, const Payload& p) {
  ++s.visited;
  s.checksum += id + p.tag + static_cast<long>(p.data.size()) + p.data.front();
}

template <class C> const Payload* find_in(const C& c, int key) {
  if constexpr (std::is_same_v<C, PairVector>) {
    auto it = std::ranges::find(c, key, &std::pair<int, Payload>::first);
    return it == c.end() ? nullptr : &it->second;
  } else {
    auto it = c.find(key);
    return it == c.end() ? nullptr : &it->second;
  }
}

// Forms that can be undefined when the body changes the container.
template <class C> Stats loop_by_value(C& c, Mode m) {
  Stats s;
  for (auto [id, p] : c) { apply_ops(c, plan_for(m, id)); touch(s, id, p); }
  return s;
}
template <class C> Stats loop_by_ref(C& c, Mode m) {
  Stats s;
  for (const auto& [id, p] : c) { apply_ops(c, plan_for(m, id)); touch(s, id, p); }
  return s;
}
template <class C> Stats loop_forwarding(C& c, Mode m) {
  Stats s;
  for (auto&& [id, p] : c) { apply_ops(c, plan_for(m, id)); touch(s, id, p); }
  return s;
}
// Held pointer from a key lookup, used after the callback.
template <class C> Stats loop_keys_held(C& c, Mode m) {
  Stats s;
  for (int key : std::ranges::to<std::vector>(c | std::views::keys)) {
    const Payload* p = find_in(c, key);
    if (p == nullptr) continue;
    apply_ops(c, plan_for(m, key));
    touch(s, key, *p);
  }
  return s;
}

// Forms that stay defined in every case.
template <class C> Stats loop_snapshot(C& c, Mode m) {
  Stats s;
  const C snapshot = c;
  for (const auto& [id, p] : snapshot) { apply_ops(c, plan_for(m, id)); touch(s, id, p); }
  return s;
}
template <class C> Stats loop_keys_copy(C& c, Mode m) {
  Stats s;
  for (int key : std::ranges::to<std::vector>(c | std::views::keys)) {
    const Payload* found = find_in(c, key);
    if (found == nullptr) continue;
    const Payload p{*found};
    apply_ops(c, plan_for(m, key));
    touch(s, key, p);
  }
  return s;
}
template <class C> Stats loop_deferred(C& c, Mode m) {
  Stats s;
  std::vector<Op> pending;
  for (const auto& [id, p] : c) {
    touch(s, id, p);
    const std::vector<Op> ops = plan_for(m, id);
    pending.insert(pending.end(), ops.begin(), ops.end());
  }
  apply_ops(c, pending);
  return s;
}
template <class C> Stats loop_erase_if(C& c, Mode m) {
  Stats s;
  std::erase_if(c, [&](const auto& e) {
    touch(s, e.first, e.second);
    return !plan_for(m, e.first).empty() && m == Mode::EraseCurrent;
  });
  return s;
}

struct Outcome {
  int visited;
  long checksum;
  std::size_t left;
};

template <class C, class Loop>
Outcome run(Loop loop, Mode m) {
  C c = make_registry<C>();
  Stats s = loop(c, m);
  return {s.visited, s.checksum, c.size()};
}

const char* mode_name(Mode m) {
  switch (m) {
    case Mode::EraseCurrent: return "erase-current";
    case Mode::EraseNext: return "erase-next";
    case Mode::Insert: return "insert";
  }
  return "?";
}

template <class C, class Loop>
void print_outcome(const char* container, const char* form, Loop loop, Mode m) {
  const Outcome o = run<C>(loop, m);
  std::printf("%-12s %-13s %-12s visited=%d checksum=%ld size_after=%zu\n", container, mode_name(m), form,
              o.visited, o.checksum, o.left);
}

template <class C>
void print_safe_forms(const char* container) {
  for (Mode m : {Mode::EraseCurrent, Mode::EraseNext, Mode::Insert}) {
    print_outcome<C>(container, "snapshot", [](C& c, Mode mm) { return loop_snapshot(c, mm); }, m);
    print_outcome<C>(container, "keys+copy", [](C& c, Mode mm) { return loop_keys_copy(c, mm); }, m);
    print_outcome<C>(container, "deferred", [](C& c, Mode mm) { return loop_deferred(c, mm); }, m);
  }
  print_outcome<C>(container, "erase_if", [](C& c, Mode mm) { return loop_erase_if(c, mm); }, Mode::EraseCurrent);
}

// ---- part 4: what each strategy allocates ------------------------------------------
const void* volatile g_sink = nullptr;

// Keeps the compiler from eliding the allocation being measured (new-expressions may be elided).
inline void escape(const void* p) {
#if defined(__GNUC__)
  asm volatile("" : : "g"(p) : "memory");
#else
  g_sink = p;
#endif
}

template <class C>
void print_costs(const char* container) {
  C c = make_registry<C>();
  counters::reset();
  { const C snapshot = c; escape(&snapshot); }
  const auto snap_allocs = counters::allocations, snap_bytes = counters::bytes;
  counters::reset();
  { auto keys = std::ranges::to<std::vector>(c | std::views::keys); escape(keys.data()); }
  std::printf("%-12s snapshot of container: %zu allocations, %zu bytes; vector of keys: %zu allocations, %zu bytes\n",
              container, snap_allocs, snap_bytes, counters::allocations, counters::bytes);
}

void print_shared_payload() {
  std::map<int, std::shared_ptr<const Payload>> shared;
  for (int id : kInitialIds) shared.emplace(id, std::make_shared<const Payload>(make_payload(id)));
  counters::reset();
  long use_count_in_loop = 0;
  for (auto [id, p] : shared) { (void)id; use_count_in_loop = p.use_count(); }
  std::printf("shared_ptr map, auto[id,p]: payload copies=%zu allocations=%zu use_count inside the loop=%ld (outside: %ld)\n",
              counters::payload_copies, counters::allocations, use_count_in_loop, shared.begin()->second.use_count());
}

// Shared payloads: a by-value binding copies a pointer. The map iterator is still the loop's cursor.
Stats loop_shared_by_value(Mode m) {
  std::map<int, std::shared_ptr<const Payload>> c;
  for (int id : kInitialIds) c.emplace(id, std::make_shared<const Payload>(make_payload(id)));
  Stats s;
  for (auto [id, p] : c) {
    for (const Op& op : plan_for(m, id)) {
      if (op.kind == Op::Kind::Erase) c.erase(op.key);
      else c.emplace(op.key, std::make_shared<const Payload>(make_payload(op.key)));
    }
    touch(s, id, *p);
  }
  return s;
}

void print_rehash() {
  HashedMap c = make_registry<HashedMap>();
  const auto before = c.bucket_count();
  apply_ops(c, plan_for(Mode::Insert, kTriggerNext));
  std::printf("unordered_map: bucket_count %zu before the %d inserts, %zu after (a rehash)\n", before, kInsertCount,
              c.bucket_count());
}

#if defined(SHOW_ERROR)
// A payload that cannot be copied turns the accidental copy into a compile error.
struct MoveOnlyPayload {
  std::vector<int> data;
  explicit MoveOnlyPayload(std::vector<int> d) : data{std::move(d)} {}
  MoveOnlyPayload(MoveOnlyPayload&&) noexcept = default;
};
void by_value_over_move_only(const std::map<int, MoveOnlyPayload>& registry) {
  for (auto [id, p] : registry) { (void)id; (void)p; }
}
#endif

#if defined(MATRIX_CASE)
// -DMATRIX_CASE with CONTAINER (0 map, 1 unordered_map, 2 vector), MODE (0,1,2), FORM (0..8; 8 is auto&& [id, p]).
template <class C> int matrix_case(Mode m, int form) {
  C c = make_registry<C>();
  Stats s;
  switch (form) {
    case 0: s = loop_by_value(c, m); break;
    case 1: s = loop_by_ref(c, m); break;
    case 2: s = loop_keys_held(c, m); break;
    case 3: s = loop_snapshot(c, m); break;
    case 4: s = loop_keys_copy(c, m); break;
    case 5: s = loop_deferred(c, m); break;
    case 6: s = loop_erase_if(c, m); break;
    case 8: s = loop_forwarding(c, m); break;
    default: s = loop_by_value(c, m); break;
  }
  std::printf("visited=%d checksum=%ld size_after=%zu\n", s.visited, s.checksum, c.size());
  return 0;
}
int main() {
  const Mode m = static_cast<Mode>(MODE);
  if (FORM == 7) {
    const Stats s = loop_shared_by_value(m);
    std::printf("visited=%d checksum=%ld\n", s.visited, s.checksum);
    return 0;
  }
  if (CONTAINER == 0) return matrix_case<OrderedMap>(m, FORM);
  if (CONTAINER == 1) return matrix_case<HashedMap>(m, FORM);
  return matrix_case<PairVector>(m, FORM);
}
#else
int main() {
  std::puts("copies per loop (3 elements)");
  print_copy_counts<OrderedMap>("map");
  print_copy_counts<HashedMap>("unordered_map");
  print_copy_counts<PairVector>("vector<pair>");
  print_const_binding();
  print_forwarding_ref();
  std::puts("loops that stay defined when the body changes the container");
  print_safe_forms<OrderedMap>("map");
  print_safe_forms<HashedMap>("unordered_map");
  print_safe_forms<PairVector>("vector<pair>");
  print_rehash();
  std::puts("allocation counts");
  print_costs<OrderedMap>("map");
  print_costs<HashedMap>("unordered_map");
  print_costs<PairVector>("vector<pair>");
  print_shared_payload();
  return 0;
}
#endif

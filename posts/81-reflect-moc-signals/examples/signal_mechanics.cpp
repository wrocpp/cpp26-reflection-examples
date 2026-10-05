// verify: gcc-only
// How a data-member signal knows who owns it, and what each design costs. No Qt.
// Build: g++-16 -std=c++26 -freflection ep27_signal_mechanics.cpp   (Compiler Explorer: g162)
#include <meta>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <source_location>
#include <type_traits>
#include <vector>

namespace meta = std::meta;

// ---------------------------------------------------------------------------------------------
// Attempt 1: identity from std::source_location in a default template argument.
// A default template argument is evaluated where the template is declared, so every member gets
// the same type. (std::source_location itself is rejected as a template argument on GCC 16.2: it
// is not a structural type. .line() is.)
template <unsigned Line = std::source_location::current().line()>
struct sig_by_line {};
struct ByLine {
  sig_by_line<> a;
  sig_by_line<> b;
};
static_assert(std::is_same_v<decltype(ByLine::a), decltype(ByLine::b)>, "one type: no identity");

// Attempt 2: a closure in a default template argument gives each member its own type.
// A closure type has internal linkage, which matters as soon as the class is in a header.
template <class Tag = decltype([] {})>
struct sig_by_closure {};
struct ByClosure {
  sig_by_closure<> a;
  sig_by_closure<> b;
  sig_by_closure<> c;
};
static_assert(!std::is_same_v<decltype(ByClosure::a), decltype(ByClosure::b)>, "distinct types");

// ---------------------------------------------------------------------------------------------
// Attempt 3: a member that takes no space. Two empty members of different types can share an
// address. Qt compares the raw bytes of the pointer to data member, so both would look the same.
template <int Id>
struct empty_signal {};
struct ZeroSize {
  [[no_unique_address]] empty_signal<1> first;
  [[no_unique_address]] empty_signal<2> second;
};
static_assert(sizeof(ZeroSize) == 1);
static_assert(meta::offset_of(^^ZeroSize::first).bytes == meta::offset_of(^^ZeroSize::second).bytes,
              "both signals sit at the same offset");

// ---------------------------------------------------------------------------------------------
// The design that passed: a hidden first member (the owner anchor) publishes the owner; each signal
// keeps it and finds its identity from its offset in a table that reflection builds.
namespace anchored {

template <class Sig, meta::info Owner = meta::current_class()>
struct signal;

struct published_owner_slot {
  void* object = nullptr;
  void const* tag = nullptr;
};
inline thread_local published_owner_slot published_owner;

template <meta::info Owner>
inline constexpr char owner_tag = 0;

template <meta::info Owner, class Self>
consteval meta::info member_of_type() {
  for (auto m : meta::nonstatic_data_members_of(Owner, meta::access_context::unchecked()))
    if (meta::remove_cvref(meta::type_of(m)) == ^^Self) return m;
  throw meta::exception("member not found in its owner", Owner);
}

template <meta::info Owner = meta::current_class()>
struct owner_anchor {
  owner_anchor() {
    constexpr meta::info self = member_of_type<Owner, owner_anchor>();
    constexpr std::size_t offset = meta::offset_of(self).bytes;
    published_owner = {const_cast<char*>(reinterpret_cast<char const*>(this)) - offset, &owner_tag<Owner>};
  }
  owner_anchor(owner_anchor const&) = delete;
  owner_anchor& operator=(owner_anchor const&) = delete;
};

struct entry {
  std::size_t offset;
  char const* name;
};

consteval std::vector<entry> make_table(meta::info cls) {
  std::vector<entry> out;
  for (auto m : meta::nonstatic_data_members_of(cls, meta::access_context::unchecked())) {
    auto const t = meta::remove_cvref(meta::type_of(m));
    if (meta::has_template_arguments(t) && meta::template_of(t) == ^^signal)
      out.push_back({static_cast<std::size_t>(meta::offset_of(m).bytes), std::define_static_string(meta::identifier_of(m))});
  }
  return out;
}

template <meta::info Owner>
inline constexpr auto table = std::define_static_array(make_table(Owner));

template <meta::info Owner>
inline constexpr char const* class_name = std::define_static_string(meta::identifier_of(Owner));

template <class... A, meta::info Owner>
struct signal<void(A...), Owner> {
  signal() : owner_(take()) {}
  signal(signal const&) = delete;
  signal& operator=(signal const&) = delete;

  void operator()(A...) const {
    auto const offset = static_cast<std::size_t>(reinterpret_cast<char const*>(this) - reinterpret_cast<char const*>(owner_));
    for (auto const& e : table<Owner>)
      if (e.offset == offset) {
        std::printf("non-static: emit %s::%s (offset %zu)\n", class_name<Owner>, e.name, e.offset);
        return;
      }
    std::abort();
  }

 private:
  static void* take() {
    if (published_owner.tag != &owner_tag<Owner>) {
      std::puts("signal built with no owner: put the anchor first");
      std::abort();
    }
    return published_owner.object;
  }
  void* owner_;
};

}  // namespace anchored

#define OWNER_ANCHOR ::anchored::owner_anchor<> anchor_;

struct Counter {
  OWNER_ANCHOR
  anchored::signal<void(int)> valueChanged;
  anchored::signal<void(int)> alsoChanged;  // same signature, still a different signal
  anchored::signal<void()> done;
};

// ---------------------------------------------------------------------------------------------
// The opt-in form: a static member, identified by its address. It takes no space in the object, so
// firing it needs the owner passed in.
namespace statics {

template <class Sig, meta::info Owner = meta::current_class()>
struct static_signal;

template <meta::info Owner>
inline constexpr char const* class_name = std::define_static_string(meta::identifier_of(Owner));

// Outside the class on purpose: inside it, the name static_signal is the injected class name, which
// reflects the specialization and not the template.
consteval bool is_static_signal(meta::info member) {
  if (!meta::is_variable(member)) return false;  // constructors and functions have no type_of
  auto const t = meta::remove_cvref(meta::type_of(member));
  return meta::has_template_arguments(t) && meta::template_of(t) == ^^static_signal;
}

template <class... A, meta::info Owner>
struct static_signal<void(A...), Owner> {
  // A real library returns a pending object so that `emit s(v)` can supply the owner. Here the owner
  // is passed by hand to keep the mechanics visible.
  void fire(typename[:Owner:]*, A...) const {
    int index = 0;
    bool found = false;
    template for (constexpr auto m : std::define_static_array(meta::members_of(Owner, meta::access_context::unchecked()))) {
      if constexpr (is_static_signal(m)) {
        if (static_cast<void const*>(&[:m:]) == static_cast<void const*>(this)) {
          std::printf("static:     emit %s::%s (index %d, found by address)\n", class_name<Owner>,
                      std::define_static_string(meta::identifier_of(m)), index);
          found = true;
        }
        ++index;
      }
    }
    if (!found) {
      std::puts("static signal not found among the static signals of its owner");
      std::abort();
    }
  }
};

}  // namespace statics

struct Sensor {
  static inline statics::static_signal<void(int)> levelChanged{};
  static inline statics::static_signal<void(int)> alarm{};
  int level = 0;
};

int main() {
  std::setvbuf(stdout, nullptr, _IONBF, 0);
  Counter c;
  c.valueChanged(1);
  c.alsoChanged(2);
  c.done();
  static_assert(std::is_same_v<decltype(Counter::valueChanged), decltype(Counter::alsoChanged)>,
                "one type for one signature, so it has external linkage");

  Sensor s;
  Sensor::levelChanged.fire(&s, 5);
  Sensor::alarm.fire(&s, 1);

  std::printf("sizeof: ByClosure %zu, ZeroSize %zu, Counter (anchor + 3 signals) %zu, Sensor (2 static signals + an int) %zu\n",
              sizeof(ByClosure), sizeof(ZeroSize), sizeof(Counter), sizeof(Sensor));
  std::printf("ZeroSize offsets: first %zu, second %zu\n", static_cast<std::size_t>(meta::offset_of(^^ZeroSize::first).bytes),
              static_cast<std::size_t>(meta::offset_of(^^ZeroSize::second).bytes));
  auto const p1 = &ZeroSize::first;
  auto const p2 = &ZeroSize::second;
  std::printf("raw bytes of &ZeroSize::first and &ZeroSize::second equal: %s\n",
              std::memcmp(&p1, &p2, sizeof p1) == 0 ? "yes" : "no");
}

// verify: gcc-only
// Which initialisation forms zero a class, and what choosing {} over = default does to its traits. Three classes have a
// start pointer, a length and an end pointer and differ only in how the empty constructor is written.
// "Zeroed" is decided by the compiler during constant evaluation: if a form leaves the length indeterminate, reading it is
// not a constant expression, so the form is not zeroed. No indeterminate value is read at run time.
// Build: g++ -std=c++26 -O1 -Wall -Wno-deprecated-declarations zeroing_by_form.cpp   (Compiler Explorer: g162, clang2210, gsnapshot)
// The program has no macros and exits 0. It prints one row per class: the three forms that can leave the length alone,
// then the Initialised, WithInitialiser, std::span, make_unique and designated-initializer cases. -Wno-deprecated-declarations hides the
// C++26 deprecation warning on std::is_trivial_v, which the file uses on purpose.
#include <cstdio>
#include <memory>
#include <span>
#include <type_traits>

constexpr int kSeven = 7;

struct Defaulted {
  const char* start;
  int length;
  const char* end;
  constexpr Defaulted() = default;
  constexpr Defaulted(const char* s, int n) : start{s}, length{n}, end{s + n} {}
};

struct Braced {
  const char* start;
  int length;
  const char* end;
  constexpr Braced() {}
  constexpr Braced(const char* s, int n) : start{s}, length{n}, end{s + n} {}
};

struct OutOfLine {
  const char* start;
  int length;
  const char* end;
  constexpr OutOfLine();
  constexpr OutOfLine(const char* s, int n) : start{s}, length{n}, end{s + n} {}
};
constexpr OutOfLine::OutOfLine() = default;

struct WithInitialiser {
  int length;
  int extra = kSeven;
};

struct Plain {
  int length;
  int extra;
};

// The recommended shape: every member has a default member initialiser, so T t; is as safe as T{}.
struct Initialised {
  const char* start = nullptr;
  int length = 0;
  const char* end = nullptr;
};

// Is the length 0 in a constant expression? A form that leaves it indeterminate makes the read non-constant.
template <class T>
constexpr bool zeroed_by_default_init = requires { typename std::bool_constant<([] { T t; return t.length == 0; }())>; };
template <class T>
constexpr bool zeroed_by_parens = requires { typename std::bool_constant<(T().length == 0)>; };
template <class T>
constexpr bool zeroed_by_braces = requires { typename std::bool_constant<(T{}.length == 0)>; };

// Defaulted on its first declaration stays trivial and is zeroed by () and {}. Braced and OutOfLine run their constructor.
static_assert(std::is_trivial_v<Defaulted> && std::is_trivially_default_constructible_v<Defaulted>);
static_assert(!zeroed_by_default_init<Defaulted> && zeroed_by_parens<Defaulted> && zeroed_by_braces<Defaulted>);
static_assert(!std::is_trivial_v<Braced> && !std::is_trivially_default_constructible_v<Braced>);
static_assert(!zeroed_by_default_init<Braced> && !zeroed_by_parens<Braced> && !zeroed_by_braces<Braced>);
static_assert(!std::is_trivial_v<OutOfLine> && !std::is_trivially_default_constructible_v<OutOfLine>);
static_assert(!zeroed_by_default_init<OutOfLine> && !zeroed_by_parens<OutOfLine> && !zeroed_by_braces<OutOfLine>);
// A default member initialiser makes the implicit constructor non-trivial. The run-time lines in main show that () and {}
// still zero the other members. GCC 16.2 rejects T() for it in a constant expression ("accessing uninitialized member").
static_assert(!std::is_trivially_default_constructible_v<WithInitialiser>);
static_assert(std::is_trivial_v<Plain> && !zeroed_by_default_init<Plain> && zeroed_by_parens<Plain> && zeroed_by_braces<Plain>);
// Initialised is not trivial either (its members have initialisers), and it is still an aggregate.
static_assert(!std::is_trivially_default_constructible_v<Initialised> && std::is_aggregate_v<Initialised>);
// A constructor that is not trivial does not stop the class from being copied with memcpy.
static_assert(std::is_trivially_copyable_v<Braced> && std::is_trivially_copyable_v<OutOfLine>);
// Only the aggregates among them can be initialised with a list of members.
static_assert(!std::is_aggregate_v<Defaulted> && !std::is_aggregate_v<Braced>);
static_assert(std::is_aggregate_v<WithInitialiser> && std::is_aggregate_v<Plain>);
// std::span has a user-provided default constructor, so it sits on the Braced side.
static_assert(!std::is_trivially_default_constructible_v<std::span<int>> && !std::is_aggregate_v<std::span<int>>);

constexpr const char* yes_no(bool value) { return value ? "zeroed" : "left alone"; }

template <class T>
void row(const char* name) {
  std::printf("%-16s trivial %d | T t;: %-10s | T(): %-10s | T{}: %-10s\n", name, static_cast<int>(std::is_trivial_v<T>),
              yes_no(zeroed_by_default_init<T>), yes_no(zeroed_by_parens<T>), yes_no(zeroed_by_braces<T>));
}

int main() {
  row<Defaulted>("Defaulted");
  row<Braced>("Braced");
  row<OutOfLine>("OutOfLine");
  row<Plain>("Plain");

  const std::span<int> view{};
  std::printf("std::span<int> view{}: data() is %s, size() is %zu\n", view.data() == nullptr ? "null" : "not null",
              view.size());

  const auto owned = std::make_unique<Defaulted>();
  std::printf("make_unique<Defaulted>() value-initialises: length %d, start %s\n", owned->length,
              owned->start == nullptr ? "null" : "not null");
  const Plain half{.length = kSeven};
  std::printf("Plain{.length = 7}: length %d, extra %d\n", half.length, half.extra);
  const Initialised plain;
  std::printf("Initialised plain; : length %d, start %s\n", plain.length, plain.start == nullptr ? "null" : "not null");
  const WithInitialiser parens = WithInitialiser();
  const WithInitialiser braces{};
  std::printf("WithInitialiser() : length %d, extra %d\n", parens.length, parens.extra);
  std::printf("WithInitialiser{} : length %d, extra %d\n", braces.length, braces.extra);
  return 0;
}

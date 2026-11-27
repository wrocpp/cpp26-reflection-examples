// verify: gcc-only
// Three class templates that want a std::optional<int>, a std::string_view and a std::span<const int> as a template
// argument. P3861R1 (adopted for the C++26 draft at Buzios, November 2026) makes those standard types structural. No
// compiler I ran implements it on 2026-11-28, so each template here takes a small structural struct of our own instead:
// public, non-mutable members of structural types, which the language already accepts.
// The program also asks the compiler that built it whether the standard types are accepted yet. No assertion pins the
// "no" answers, so a compiler that implements the paper prints "yes" and the program still exits 0.
// Build: g++ -std=c++26   (Compiler Explorer: g162, gsnapshot, clang_trunk at -std=c++26). No macros. Exits 0.
// Prints:
//   Bounded<{}>::describe()          no limit
//   Bounded<{kLimit}>::describe()    limit 8
//   Named<"retries">::name           retries (7 characters)
//   Named<"retries"> twice           same type: yes
//   Lookup<kPrimes>::sum             17 over 4 values
//   Lookup over a copy of kPrimes    same type: no
//   accepted as a template argument today:
//     std::optional<int>               no
//     std::string_view                 no
//     std::span<const int>             no
// Limits: the text wrapper compares characters, so two equal texts give one type. The span wrapper compares the address
// and the length, so equal contents in two arrays give two types. R0 of P3861 gives std::span that address rule, and
// std::string_view too; I did not read R1, so I do not claim what the adopted text says.
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <iterator>
#include <numeric>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>

constexpr int kLimit = 8;
constexpr int kNameWidth = 32;
constexpr const char* kYes = "yes";
constexpr const char* kNo = "no";

// --- std::optional<int> as an argument: an engaged flag and a value -----------------------------------------------
struct optional_int {
  bool engaged{};
  int value{};

  constexpr optional_int() = default;
  constexpr optional_int(int v) : engaged{true}, value{v} {}
  constexpr std::optional<int> get() const { return engaged ? std::optional<int>{value} : std::nullopt; }
};

template <optional_int Limit>
struct Bounded {
  static void describe(const char* label) {
    constexpr std::optional<int> limit = Limit.get();
    if constexpr (limit.has_value()) {
      std::printf("%-*s limit %d\n", kNameWidth, label, *limit);
    } else {
      std::printf("%-*s no limit\n", kNameWidth, label);
    }
  }
};

// --- std::string_view as an argument: the characters by value ----------------------------------------------------
template <std::size_t Capacity>
struct fixed_text {
  char chars[Capacity]{};

  consteval fixed_text(const char (&literal)[Capacity]) { std::copy_n(literal, Capacity, chars); }
  constexpr std::string_view view() const { return {chars, Capacity - 1}; }
};

template <fixed_text Text>
struct Named {
  static constexpr std::string_view name = Text.view();
};

// --- std::span<const int> as an argument: an address and a length ------------------------------------------------
struct int_span {
  const int* data{};
  std::size_t size{};

  constexpr std::span<const int> view() const { return {data, size}; }
};

template <int_span Table>
struct Lookup {
  static constexpr int sum = std::accumulate(Table.view().begin(), Table.view().end(), 0);
  static constexpr std::size_t count = Table.view().size();
};

constexpr int kPrimes[] = {2, 3, 5, 7};
constexpr int kPrimesCopy[] = {2, 3, 5, 7};
constexpr int_span kPrimesTable{.data = kPrimes, .size = std::size(kPrimes)};
constexpr int_span kPrimesCopyTable{.data = kPrimesCopy, .size = std::size(kPrimesCopy)};

// --- does the compiler that built this accept the standard types yet? --------------------------------------------
template <auto Value>
struct Probe {};

template <class T>
concept accepted_as_argument = requires { typename Probe<T{}>; };

template <class T>
void report(const char* name) {
  std::printf("  %-*s %s\n", kNameWidth, name, accepted_as_argument<T> ? kYes : kNo);
}

int main() {
  Bounded<optional_int{}>::describe("Bounded<{}>::describe()");
  Bounded<optional_int{kLimit}>::describe("Bounded<{kLimit}>::describe()");

  using Retries = Named<"retries">;
  std::printf("%-*s %.*s (%zu characters)\n", kNameWidth, "Named<\"retries\">::name",
              static_cast<int>(Retries::name.size()), Retries::name.data(), Retries::name.size());
  std::printf("%-*s same type: %s\n", kNameWidth, "Named<\"retries\"> twice",
              std::is_same_v<Retries, Named<"retries">> ? kYes : kNo);

  using Primes = Lookup<kPrimesTable>;
  std::printf("%-*s %d over %zu values\n", kNameWidth, "Lookup<kPrimes>::sum", Primes::sum, Primes::count);
  std::printf("%-*s same type: %s\n", kNameWidth, "Lookup over a copy of kPrimes",
              std::is_same_v<Primes, Lookup<kPrimesCopyTable>> ? kYes : kNo);

  std::printf("accepted as a template argument today:\n");
  report<std::optional<int>>("std::optional<int>");
  report<std::string_view>("std::string_view");
  report<std::span<const int>>("std::span<const int>");
}

// verify: gcc-only
// Which types are accepted as a non-type template parameter today, and what "the same argument" means for a pointer.
// This is the limit that P3861 (adopted at Buzios, revision R1 not public on 2026-11-21) is about: its R0 makes
// std::string_view, std::span and std::tuple of structural types structural. A requires-expression over Probe<T{}>
// asks each compiler, so the program compiles everywhere and prints the answer of the compiler that built it. No
// assertion pins the "no" rows, so a compiler that implements the paper prints "yes" for them and still exits 0.
// Build: g++ -std=c++26   (Compiler Explorer: g162, gsnapshot, clang2310, clang_trunk; clang_bb_p2996 with
//        -std=c++26 -freflection-latest -stdlib=libc++). The program has no macros and exits 0 on each of them.
#include <cstddef>
#include <cstdio>
#include <optional>
#include <span>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <variant>

constexpr std::size_t kTextCapacity = 4;
constexpr int kNameWidth = 24;
constexpr const char* kYes = "yes";
constexpr const char* kNo = "no";

template <auto Value>
struct Probe {};

// True when a value-initialised T is accepted as a template argument, which is what structural means in practice.
template <class T>
concept accepted_as_argument = requires { typename Probe<T{}>; };

struct plain_text {  // public, non-mutable members of structural types: structural by the language's own rule
  char text[kTextCapacity]{};
};

constexpr char first[] = "Hello";
constexpr char second[] = "Hello";

template <const char* Address>
struct ByAddress {};

template <class T>
void report(const char* name) {
  std::printf("%-*s %s\n", kNameWidth, name, accepted_as_argument<T> ? kYes : kNo);
}

int main() {
  report<int>("int");
  report<const char*>("const char*");
  report<plain_text>("plain_text");
  report<std::string_view>("std::string_view");
  report<std::span<const int>>("std::span<const int>");
  report<std::tuple<int, char>>("std::tuple<int, char>");
  report<std::optional<int>>("std::optional<int>");
  report<std::variant<int>>("std::variant<int>");
  std::printf("ByAddress<first> and ByAddress<second> are the same type: %s\n",
              std::is_same_v<ByAddress<first>, ByAddress<second>> ? kYes : kNo);
}

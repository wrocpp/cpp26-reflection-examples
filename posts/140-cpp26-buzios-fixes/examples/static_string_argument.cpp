// verify: gcc-only
// Two ways to put a text into a template argument with C++26 today, since std::string_view is not structural (see
// structural_today.cpp). ByValue keeps the characters in the argument: a class with a char array member filled by a
// consteval constructor, the same trick as the label of the annotate post. ByAddress keeps a pointer: std::define_static_string
// returns a pointer to a static array. The program prints what each route treats as "the same argument".
// Build: g++ -std=c++26 -freflection   (Compiler Explorer: g162, gsnapshot; clang_bb_p2996 with
//        -std=c++26 -freflection-latest -stdlib=libc++). The program has no macros and exits 0 on each of them.
// Not shown: a std::string_view argument built from such a pointer. That is my reading of what R0 of P3861 would allow,
// and no compiler checked implements it.
#include <meta>

#include <cstddef>
#include <cstdio>
#include <string_view>
#include <type_traits>

constexpr const char* kYes = "yes";
constexpr const char* kNo = "no";

template <std::size_t Size>
struct fixed_string {
  char text[Size]{};
  consteval fixed_string(const char (&source)[Size]) {
    for (std::size_t i = 0; i < Size; ++i) text[i] = source[i];
  }
};

template <fixed_string Text>
struct ByValue {
  static constexpr std::string_view text() { return Text.text; }
};

template <const char* Address>
struct ByAddress {
  static constexpr std::string_view text() { return Address; }
};

constexpr const char* abc = std::define_static_string("abc");
constexpr const char* abc_again = std::define_static_string(std::string_view{"abc"});
constexpr const char* abd = std::define_static_string("abd");

int main() {
  std::printf("by value, \"abc\" twice: same type %s, reads back \"%s\"\n",
              std::is_same_v<ByValue<"abc">, ByValue<"abc">> ? kYes : kNo, ByValue<"abc">::text().data());
  std::printf("by value, \"abc\" and \"abd\": same type %s\n",
              std::is_same_v<ByValue<"abc">, ByValue<"abd">> ? kYes : kNo);
  std::printf("by address, define_static_string twice: same pointer %s, same type %s, reads back \"%s\"\n",
              abc == abc_again ? kYes : kNo, std::is_same_v<ByAddress<abc>, ByAddress<abc_again>> ? kYes : kNo,
              ByAddress<abc>::text().data());
  std::printf("by address, \"abc\" and \"abd\": same type %s\n",
              std::is_same_v<ByAddress<abc>, ByAddress<abd>> ? kYes : kNo);
}

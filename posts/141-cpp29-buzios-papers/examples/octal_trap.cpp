// verify: gcc-only
// The table from P0085R5 ("Padding"): a programmer pads numbers with a leading zero to align the columns, and the
// literal 042 is octal, so it is 34 and not 42. The table is {100, 042} and {107, 000}. Nothing in the language
// diagnoses it today, and the paper deprecates that spelling (unprefixed octal literals other than 0).
// Build: g++ -std=c++26 -O1 -Wall -Wextra -pedantic   (Compiler Explorer: g162). No diagnostic on g162 and gsnapshot
//        with these flags, and none on clang2310 and clang_trunk with
//        -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic.
// Prints the four cells. Exits 0 when the padded cell holds 34, the octal value, and not 42. The program has no macros.
#include <array>
#include <cstdio>

constexpr int kIntended = 42;
constexpr int kActual = 34;

using Table = std::array<std::array<int, 2>, 2>;

constexpr Table kTable{{{100, 042}, {107, 000}}};

static_assert(kTable[0][1] == kActual);

int main() {
  for (const auto& row : kTable) std::printf("%d %d\n", row[0], row[1]);
  return kTable[0][1] == kActual && kTable[0][1] != kIntended ? 0 : 1;
}

// verify: ce-only
// The 0o prefix of P0085R5 in C++ mode. Clang accepts 0o and 0O in C++ mode and calls it an extension under
// -pedantic, so 0o755 is the number 493, the same as 0755, and it is the permission mask rwxr-xr-x. P0085R5 rewrites
// the std::filesystem::perms table in that spelling, and a static_assert checks that 0o755 equals the or-combination
// of the named permissions.
// Build: clang++ -std=c++26 -O1   (Compiler Explorer: clang2310 and clang_trunk). With -pedantic Clang warns
//        "octal integer literals are a Clang extension [-Woctal-prefix-extension]"; with -pedantic-errors it is an
//        error.
// GCC 16.2 and GCC trunk reject 0o17 with "unable to find numeric literal operator 'operator""o17'"; MSVC gives C3688.
// Prints the values. Exits 0 when the prefixed and unprefixed spellings agree. The program has no macros.
#include <cstdio>
#include <filesystem>

constexpr int kDecimalFifteen = 15;
constexpr int kDecimalMask = 493;

using std::filesystem::perms;

constexpr perms kExpectedMask =
    perms::owner_all | perms::group_read | perms::group_exec | perms::others_read | perms::others_exec;

static_assert(0o17 == kDecimalFifteen);
static_assert(0O17 == kDecimalFifteen);
static_assert(0o755 == kDecimalMask);
static_assert(static_cast<perms>(0o755) == kExpectedMask);

int main() {
  std::printf("0o17 = %d, 0O17 = %d, 017 = %d\n", 0o17, 0O17, 017);
  std::printf("0o755 = %d\n", 0o755);
  return 0o17 == 017 && 0o755 == 0755 ? 0 : 1;
}

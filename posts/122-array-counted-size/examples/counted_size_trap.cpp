// verify: gcc-only
// The trap behind the check: a hand-counted std::array<T, N> accepts a list that is too short.
//
// Build: g++ -std=c++23 -O1 counted_size_trap.cpp -o counted_size_trap && ./counted_size_trap
// Prints: the table with its forgotten entry, and which list lengths the compiler accepts.
// Limits: a list that is too long is rejected, and that is the only direction the compiler guards.
#include <array>
#include <cstdio>
#include <string_view>
#include <utility>

namespace {

enum class Level : unsigned { Debug, Info, Warning, Error, Fatal, Count };

constexpr auto kLevelCount = std::to_underlying(Level::Count);
constexpr auto kFatalIndex = std::to_underlying(Level::Fatal);

// Fatal has no name: the list has four entries for five slots, and the build is green.
constexpr std::array<std::string_view, kLevelCount> kNames{"debug", "info", "warning", "error"};

// A list that fits is one the array accepts when it is built from the pack.
template <class... Names>
concept FitsLevels = requires(Names... names) { std::array<std::string_view, kLevelCount>{names...}; };

using Name = const char*;

static_assert(FitsLevels<Name, Name, Name, Name, Name>);
static_assert(FitsLevels<Name, Name, Name, Name>, "a list that is too short compiles");
static_assert(!FitsLevels<Name, Name, Name, Name, Name, Name>, "a list that is too long does not");
static_assert(kNames[kFatalIndex].empty(), "the forgotten entry is an empty string");

}  // namespace

int main() {
  const std::string_view fatal = kNames[kFatalIndex];
  std::printf("%zu names declared, name of Fatal: '%.*s' (%zu characters)\n", kNames.size(),
              static_cast<int>(fatal.size()), fatal.data(), fatal.size());
  return 0;
}

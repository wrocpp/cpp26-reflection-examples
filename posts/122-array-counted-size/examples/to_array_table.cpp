// verify: gcc-only
// The fix the check proposes: std::to_array deduces the size, and one assertion states the count.
//
// Build: g++ -std=c++23 -O1 to_array_table.cpp -o to_array_table && ./to_array_table
// Prints: the size the table has and the size the enum expects, for a complete and a short list.
// Limits: to_array does not know how many entries are wanted. The static_assert is what states it.
#include <array>
#include <cstdio>
#include <string_view>
#include <utility>

namespace {

enum class Level : unsigned { Debug, Info, Warning, Error, Fatal, Count };

constexpr auto kLevelCount = std::to_underlying(Level::Count);

constexpr auto kNames = std::to_array<std::string_view>({"debug", "info", "warning", "error", "fatal"});
constexpr auto kShortNames = std::to_array<std::string_view>({"debug", "info", "warning", "error"});

// The count is checked where it is declared. A forgotten entry now stops the build.
static_assert(kNames.size() == kLevelCount);
static_assert(kShortNames.size() != kLevelCount, "this list is one name short; asserting == would fail");

}  // namespace

int main() {
  std::printf("complete: %zu of %u entries\n", kNames.size(), kLevelCount);
  std::printf("short:    %zu of %u entries\n", kShortNames.size(), kLevelCount);
  return 0;
}

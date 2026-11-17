// verify: gcc-only
// main.cpp - uses events_after.hpp, the header that passes the policy.
//
// Build: g++ -std=c++26 main.cpp
// Run:   ./a.out   prints one line per name and exits 0.
// Limits: the header comments are the subject, not the program.
#include <array>
#include <cstdio>
#include <string_view>

#include "events_after.hpp"

namespace {

constexpr std::array<std::string_view, 3> kNames{"debug", "error", "bogus"};

static_assert(events::level_count() == events::kLevelCount);
static_assert(events::is_severe(events::parse_level("error")));
static_assert(!events::is_severe(events::default_level()));

}  // namespace

int main() {
  for (const std::string_view name : kNames) {
    const events::Level level = events::parse_level(name);
    std::printf("%.*s -> %.*s severe=%d\n", static_cast<int>(name.size()), name.data(),
                static_cast<int>(events::level_name(level).size()), events::level_name(level).data(),
                events::is_severe(level) ? 1 : 0);
  }
  return 0;
}

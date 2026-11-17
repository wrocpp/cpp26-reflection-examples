// verify: gcc-only
// events_after.hpp - events_before.hpp with the three flagged comments rewritten as caller-facing contracts. The
// wrocpp-header-comment-policy check reports nothing on this file. The history those comments carried belongs in
// the commit message.
//
// Build: used by main.cpp, g++ -std=c++26 main.cpp
// Limits: header only, no state.
#include <string_view>

namespace events {

constexpr unsigned kLevelCount = 4;

enum class Level : unsigned { Debug, Info, Warning, Error };

/// Returns the level for a name, and Info when the name is not one of the four.
constexpr Level parse_level(std::string_view name) {
  if (name == "debug") return Level::Debug;
  if (name == "warning") return Level::Warning;
  if (name == "error") return Level::Error;
  return Level::Info;
}

/// Returns the lower-case name of a level. The view refers to a string literal and never dangles.
constexpr std::string_view level_name(Level level) {
  switch (level) {
    case Level::Debug: return "debug";
    case Level::Warning: return "warning";
    case Level::Error: return "error";
    default: return "info";
  }
}

/// True for Warning and Error.
constexpr bool is_severe(Level level) { return level >= Level::Warning; }

// spec: SWR-EVT-0004
constexpr Level default_level() { return Level::Info; }

// TODO(#42): accept a span of names.
constexpr unsigned level_count() { return kLevelCount; }

}  // namespace events

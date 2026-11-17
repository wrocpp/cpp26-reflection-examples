// verify: gcc-only
// events_before.hpp - the header the hook sees right after an agent edited it. Three comments are the mistake the
// wrocpp-header-comment-policy check reports: one points at git, two narrate history. The rest must stay clean.
//
// Check: syntax only, g++ -std=c++26 -fsyntax-only -x c++ events_before.hpp
// Limits: the file is the INPUT of the check, so it is meant to be flagged. It is not included by main.cpp.
#include <string_view>

namespace events {

enum class Level : unsigned { Debug, Info, Warning, Error };

// See commit 4f9c2ab for why this takes a string_view.
constexpr Level parse_level(std::string_view name) {
  if (name == "debug") return Level::Debug;
  if (name == "warning") return Level::Warning;
  if (name == "error") return Level::Error;
  return Level::Info;
}

// This used to return a raw pointer.
constexpr std::string_view level_name(Level level) {
  switch (level) {
    case Level::Debug: return "debug";
    case Level::Warning: return "warning";
    case Level::Error: return "error";
    default: return "info";
  }
}

// An earlier version cached the last name.
constexpr bool is_severe(Level level) { return level >= Level::Warning; }

/// Returns the level for a name, and Info when the name is not one of the four.
constexpr Level fallback_level() { return Level::Info; }

// spec: SWR-EVT-0004
constexpr Level default_level() { return fallback_level(); }

// TODO(#42): accept a span of names.
constexpr unsigned level_count() { return 4; }

}  // namespace events

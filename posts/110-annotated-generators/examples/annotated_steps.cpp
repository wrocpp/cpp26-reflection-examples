// Annotate member functions, run them lazily. [[=step{"parse"}]] marks a function as a step and names it.
// run_steps(object) yields (title, result) one step at a time, in declaration order. A step that the
// consumer never asks for is never called.
#include <cstddef>
#include <format>
#include <generator>
#include <meta>
#include <print>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

inline constexpr std::size_t kTitleCapacity = 24;

struct step {
  char title[kTitleCapacity]{};
  consteval step() = default;
  consteval step(std::string_view source) {
    for (std::size_t i = 0; i < source.size(); ++i) title[i] = source[i];
  }
};

consteval bool is_step(std::meta::info member) {
  if (!std::meta::is_function(member)) return false;
  for (auto annotation : std::meta::annotations_of(member))
    if (std::meta::remove_cvref(std::meta::type_of(annotation)) == ^^step) return true;
  return false;
}

consteval std::string_view title_of(std::meta::info member) {
  for (auto annotation : std::meta::annotations_of(member)) {
    if (std::meta::remove_cvref(std::meta::type_of(annotation)) != ^^step) continue;
    const step value = std::meta::extract<step>(annotation);  // a local, so the view is not into a temporary
    const std::string_view title{value.title};
    return title.empty() ? std::meta::identifier_of(member) : std::string_view{std::define_static_string(title)};
  }
  return {};
}

template <class T>
consteval std::vector<std::meta::info> step_functions() {
  std::vector<std::meta::info> found;
  for (auto member : std::meta::members_of(^^T, std::meta::access_context::unchecked()))
    if (is_step(member)) found.push_back(member);
  return found;
}

template <class T>
constexpr auto steps = std::define_static_array(step_functions<T>());

using Outcome = std::pair<std::string_view, std::string>;

template <class T, std::size_t... Indices>
std::generator<Outcome> run_impl(T& object, std::index_sequence<Indices...>) {
  ((co_yield {title_of(steps<T>[Indices]), std::format("{}", object.[:steps<T>[Indices]:]())}), ...);
}

template <class T>
std::generator<Outcome> run_steps(T& object) {
  return run_impl(object, std::make_index_sequence<steps<T>.size()>{});
}

struct Pipeline {
  int executed = 0;
  [[=step{}]] int load() { ++executed; return 3; }
  [[=step{"parse"}]] std::string parse() { ++executed; return "ok"; }
  [[=step{"validate"}]] bool validate() { ++executed; return true; }
  void helper() {}
};

int main() {
  Pipeline pipeline;
  for (const auto& [title, result] : run_steps(pipeline)) {
    std::println("{} -> {}", title, result);
    if (title == "parse") break;
  }
  std::println("executed {} of 3 steps", pipeline.executed);
}

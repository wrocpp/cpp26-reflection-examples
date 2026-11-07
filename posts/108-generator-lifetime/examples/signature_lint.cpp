// A reflection lint: a function that returns std::generator and takes a reference parameter fails a
// static_assert whose message names the function. It reads declarations in one namespace, so it sees only
// functions declared before the assert. It looks at reference parameters only: a std::string_view, a
// std::span or a pointer parameter dangles the same way and is not flagged. Needs -std=c++26 -freflection.
//   -DSHOW_ERROR   adds a generator function with a const& parameter, and the assert fires
#include <generator>
#include <meta>
#include <string>
#include <string_view>
#include <vector>

namespace api {
std::generator<int> by_value(std::vector<int> values) {
  for (int value : values) co_yield value;
}
int count(const std::vector<int>& values) { return static_cast<int>(values.size()); }
#ifdef SHOW_ERROR
std::generator<int> by_reference(const std::vector<int>& values) {
  for (int value : values) co_yield value;
}
#endif
}  // namespace api

consteval bool returns_generator(std::meta::info function) {
  const auto result = std::meta::return_type_of(function);
  return std::meta::has_template_arguments(result) && std::meta::template_of(result) == ^^std::generator;
}

consteval bool takes_reference(std::meta::info function) {
  for (auto parameter : std::meta::parameters_of(function))
    if (std::meta::is_reference_type(std::meta::type_of(parameter))) return true;
  return false;
}

consteval std::string_view first_offender(std::meta::info scope) {
  for (auto member : std::meta::members_of(scope, std::meta::access_context::unchecked()))
    if (std::meta::is_function(member) && returns_generator(member) && takes_reference(member))
      return std::define_static_string(std::string{"generator function takes a reference parameter: "} +
                                       std::string{std::meta::identifier_of(member)});
  return {};
}

constexpr std::string_view offender = first_offender(^^api);
static_assert(offender.empty(), offender);

int main() {}

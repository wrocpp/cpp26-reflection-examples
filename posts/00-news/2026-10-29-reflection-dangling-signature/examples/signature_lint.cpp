// verify: gcc-only
// Manual mint, not the pipeline: GCC 16.2 or later with -std=c++26 -freflection.
// g++ -std=c++26 -freflection
#include <meta>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace api {
std::string_view first_word(std::string s);                             // a view of a by-value owner
std::span<const int> head(std::vector<int> v);                          // a span of a by-value owner
const std::string& longer(const std::string& a, const std::string& b);  // the std::max shape
std::string_view first_word_ok(std::string_view s);                     // a view of a view
std::string longer_ok(const std::string& a, const std::string& b);      // returns by value
}  // namespace api

// ^^std::string names an alias, so compare dealiased types on both sides.
consteval std::meta::info canon(std::meta::info t) {
  return std::meta::dealias(std::meta::remove_cvref(t));
}
consteval bool is_view(std::meta::info t) {
  auto c = canon(t);
  return std::meta::has_template_arguments(c) &&
         (std::meta::template_of(c) == ^^std::basic_string_view ||
          std::meta::template_of(c) == ^^std::span);
}
consteval bool is_owner(std::meta::info t) {
  auto c = canon(t);
  return c == std::meta::dealias(^^std::string) ||
         (std::meta::has_template_arguments(c) && std::meta::template_of(c) == ^^std::vector);
}

int main() {
  constexpr auto ctx = std::meta::access_context::unchecked();
  template for (constexpr auto f : std::define_static_array(std::meta::members_of(^^api, ctx))) {
    if constexpr (std::meta::is_function(f)) {
      constexpr auto ret = std::meta::return_type_of(f);
      bool owner_by_value = false;
      bool same_type_ref = false;
      template for (constexpr auto p : std::define_static_array(std::meta::parameters_of(f))) {
        constexpr auto pt = std::meta::type_of(p);
        if constexpr (!std::meta::is_reference_type(pt) && is_owner(pt)) owner_by_value = true;
        if constexpr (std::meta::is_reference_type(ret) && std::meta::is_reference_type(pt) &&
                      canon(ret) == canon(pt))
          same_type_ref = true;
      }
      const char* verdict = "ok";
      if constexpr (is_view(ret))
        verdict = owner_by_value ? "RETURNS A VIEW OF A BY-VALUE OWNER" : "returns a view (check the source)";
      else if constexpr (std::meta::is_reference_type(ret))
        verdict = same_type_ref ? "RETURNS A REFERENCE THAT MAY BIND A TEMPORARY ARGUMENT"
                                : "returns a reference (check the source)";
      std::println("{:14} {}", std::meta::identifier_of(f), verdict);
    }
  }
}

// Numbering the fields of a struct with std::views::enumerate, over the pair-yielding generator.
// -DSHOW_TUPLE_ENUMERATE  a by-value generator that yields const std::tuple<int, int>, as in the enumerate
//                         of the earlier short, applied to the same fields(): does not compile
#include <cstddef>
#include <format>
#include <generator>
#include <meta>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

using Field = std::pair<std::string_view, std::string>;

template <class T>
constexpr auto members = std::define_static_array(
    std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()));

template <class T, std::size_t... Indices>
std::generator<Field> fields_impl(const T& object, std::index_sequence<Indices...>) {
  ((co_yield {std::meta::identifier_of(members<T>[Indices]), std::format("{}", object.[:members<T>[Indices]:])}), ...);
}

template <class T>
std::generator<Field> fields(const T& object) {
  return fields_impl(object, std::make_index_sequence<members<T>.size()>{});
}

#ifdef SHOW_TUPLE_ENUMERATE
std::generator<const std::tuple<int, int>> enumerate(auto container) {
  for (int index = 0; auto&& value : container) {
    co_yield std::tuple{index, value};
    ++index;
  }
}
#endif

struct Point {
  int x = 1;
  double y = 2.5;
  std::string name = "p";
};

int main() {
  const Point point;
  for (const auto& [index, field] : std::views::enumerate(fields(point)))
    std::println("{}: {} = {}", index, field.first, field.second);
#ifdef SHOW_TUPLE_ENUMERATE
  for (const auto& [index, field] : enumerate(fields(point))) std::println("{}", index);
#endif
}

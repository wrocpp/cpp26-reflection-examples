// Annotate data members, get a generator. [[=skip{}]] hides a member and [[=label{"..."}]] renames it.
// The annotations are read at compile time into a constexpr array outside the coroutine, and an
// index_sequence fold yields one pair per kept member (a reflection value cannot live in the frame).
// -DSHOW_STRING_VIEW_LABEL  makes the annotation payload a std::string_view: does not compile
#include <cstddef>
#include <format>
#include <generator>
#include <meta>
#include <print>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

inline constexpr std::size_t kLabelCapacity = 24;

struct skip {};

#if defined(SHOW_STRING_VIEW_LABEL)
struct label {
  std::string_view text;
};
#else
// An annotation value must be a structural type, so the text is copied into an array member.
struct label {
  char text[kLabelCapacity]{};
  consteval label(std::string_view source) {
    for (std::size_t i = 0; i < source.size(); ++i) text[i] = source[i];
  }
};
#endif

consteval bool has_annotation(std::meta::info entity, std::meta::info tag) {
  for (auto annotation : std::meta::annotations_of(entity))
    if (std::meta::remove_cvref(std::meta::type_of(annotation)) == tag) return true;
  return false;
}

consteval std::string_view label_of(std::meta::info member) {
  for (auto annotation : std::meta::annotations_of(member))
    if (std::meta::remove_cvref(std::meta::type_of(annotation)) == ^^label)
      return std::define_static_string(std::string_view{std::meta::extract<label>(annotation).text});
  return std::meta::identifier_of(member);
}

template <class T>
consteval std::vector<std::meta::info> kept_members() {
  std::vector<std::meta::info> kept;
  for (auto member : std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()))
    if (!has_annotation(member, ^^skip)) kept.push_back(member);
  return kept;
}

template <class T>
constexpr auto members = std::define_static_array(kept_members<T>());

using Field = std::pair<std::string_view, std::string>;

template <class T, std::size_t... Indices>
std::generator<Field> fields_impl(const T& object, std::index_sequence<Indices...>) {
  ((co_yield {label_of(members<T>[Indices]), std::format("{}", object.[:members<T>[Indices]:])}), ...);
}

template <class T>
std::generator<Field> fields(const T& object) {
  return fields_impl(object, std::make_index_sequence<members<T>.size()>{});
}

struct User {
  int id = 7;
  [[=skip{}]] std::string password = "hunter2";
  [[=label{"user_name"}]] std::string login = "ada";
  double score = 2.5;
};

int main() {
  for (const auto& [name, value] : fields(User{})) std::println("{} = {}", name, value);
}

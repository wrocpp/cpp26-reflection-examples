// verify: gcc-only
// The SQL name of a column sits on its enumerator as an annotation, a consteval check reports the first enumerator that
// breaks the rule, and a reflection is passed as a template argument. Prints the names it looked up.
// Build: g++ -std=c++26 -freflection   (Compiler Explorer: g162)
// No macros; the program exits 0. The default build has no failing case: the README lists the variants that stop the build.
#include <meta>

#include <cstddef>
#include <cstdio>
#include <string>
#include <string_view>

constexpr std::size_t kMaxSqlName = 24;

// Structural: public members only, so it can be an annotation and a template argument.
struct sql {
  char text[kMaxSqlName]{};
  constexpr sql(const char* s) {
    for (std::size_t i = 0; s[i] != '\0'; ++i) text[i] = s[i];
  }
};

enum class Column {
  Name [[=sql{"name"}]],
  Wavelength [[=sql{"wavelength_nm"}]],
  Intensity [[=sql{"intensity"}]],
};

// A second enum that forgets one name and one that repeats one: only used with inverted assertions.
enum class Draft { Name [[=sql{"name"}]], Raw };
enum class Twice { A [[=sql{"x"}]], B [[=sql{"x"}]] };

namespace sqlnames {

consteval std::string_view keep(std::string_view s) { return std::define_static_string(s); }

consteval std::string_view text_of(std::meta::info enumerator) {
  return keep(std::string_view{std::meta::extract<sql>(std::meta::annotations_of_with_type(enumerator, ^^sql)[0]).text});
}

consteval bool has_name(std::meta::info enumerator) {
  return !std::meta::annotations_of_with_type(enumerator, ^^sql).empty();
}

// The name for an enumerator reflection: no value, so aliases cannot hide which enumerator was meant.
template <std::meta::info E>
constexpr std::string_view of_enumerator = text_of(E);

// The name for a value. A value that is no enumerator is a hard error that names it.
template <auto V>
constexpr std::string_view of_value = [] consteval {
  for (auto e : std::meta::enumerators_of(^^decltype(V))) {
    if (std::meta::extract<decltype(V)>(e) == V) return text_of(e);
  }
  throw "this value is not an enumerator";
}();

// Empty when every enumerator of E has a distinct name, else a sentence that names the first offender.
template <typename E>
consteval std::string_view problem() {
  std::string seen;
  for (auto e : std::meta::enumerators_of(^^E)) {
    std::string who = std::string{std::meta::identifier_of(^^E)} + "::" + std::string{std::meta::identifier_of(e)};
    if (!has_name(e)) return keep(who + " has no [[=sql{...}]] annotation");
    std::string name = std::string{text_of(e)};
    if (seen.find("|" + name + "|") != std::string::npos) return keep(who + " repeats the SQL name '" + name + "'");
    seen += "|" + name + "|";
  }
  return {};
}

}  // namespace sqlnames

static_assert(sqlnames::of_value<Column::Name> == "name");
static_assert(sqlnames::of_value<Column::Wavelength> == "wavelength_nm");
static_assert(sqlnames::of_enumerator<^^Column::Intensity> == "intensity");
static_assert(sqlnames::problem<Column>().empty(), sqlnames::problem<Column>());

// The inverted cases: the same function reports what is wrong.
static_assert(sqlnames::problem<Draft>() == "Draft::Raw has no [[=sql{...}]] annotation");
static_assert(sqlnames::problem<Twice>() == "Twice::B repeats the SQL name 'x'");

// A reflection is itself a structural type, so it can be a template argument next to a value.
template <std::meta::info R>
struct named {
  static constexpr std::string_view name = std::meta::identifier_of(R);
};
static_assert(named<^^Column::Wavelength>::name == "Wavelength");
static_assert(named<^^Column>::name == "Column");

int main() {
  std::printf("%s %s %s\n", sqlnames::of_value<Column::Name>.data(), sqlnames::of_value<Column::Wavelength>.data(),
              sqlnames::of_enumerator<^^Column::Intensity>.data());
  std::printf("problem(Draft) = %s\n", sqlnames::problem<Draft>().data());
  std::printf("problem(Twice) = %s\n", sqlnames::problem<Twice>().data());
  return 0;
}

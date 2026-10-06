// verify: gcc-only
// A class that finds itself, and Q_PROPERTY text parsed at compile time. No Qt.
// Build: g++-16 -std=c++26 -freflection -Wall -Wextra object_reduction.cpp   (Compiler Explorer: g162)
// -DBAD adds a property that uses a keyword the parser does not know; the build then stops.
#include <meta>

#include <cstddef>
#include <print>
#include <string_view>
#include <vector>

namespace meta = std::meta;

struct prop_decl {
  std::string_view type, name, read, write, notify;
};

constexpr bool is_keyword(std::string_view w) { return w == "READ" || w == "WRITE" || w == "NOTIFY"; }

// "int level READ level WRITE setLevel NOTIFY levelChanged": the type and name come before the first
// keyword, and each keyword is followed by one word. The real parser in reflect-moc handles more
// keywords (MEMBER, RESET, CONSTANT, FINAL, ...); this one is the same idea in 20 lines.
consteval prop_decl parse_property(std::string_view text) {
  std::vector<std::string_view> words;
  for (std::size_t i = 0; i < text.size();) {
    while (i < text.size() && text[i] == ' ') ++i;
    std::size_t const start = i;
    while (i < text.size() && text[i] != ' ') ++i;
    if (i > start) words.push_back(text.substr(start, i - start));
  }
  prop_decl out{};
  std::size_t first_keyword = words.size();
  for (std::size_t w = 0; w < words.size(); ++w)
    if (is_keyword(words[w])) {
      first_keyword = w;
      break;
    }
  if (first_keyword < 2) throw meta::exception("expected 'type name KEYWORD ...'", ^^parse_property);
  out.name = words[first_keyword - 1];
  out.type = text.substr(0, static_cast<std::size_t>(words[first_keyword - 1].data() - text.data()) - 1);
  for (std::size_t w = first_keyword; w + 1 < words.size(); w += 2) {
    if (words[w] == "READ") out.read = words[w + 1];
    else if (words[w] == "WRITE") out.write = words[w + 1];
    else if (words[w] == "NOTIFY") out.notify = words[w + 1];
    else throw meta::exception("unknown property keyword", ^^parse_property);
  }
  return out;
}

// The parser sees the macro argument as the text the programmer typed.
static_assert(parse_property("int level READ level WRITE setLevel").write == "setLevel");
static_assert(parse_property("unsigned long long ticks READ ticks").type == "unsigned long long");

#define CAT_(a, b) a##b
#define CAT(a, b) CAT_(a, b)

// The class being defined, found from inside one of its own member functions.
#define SELF (meta::parent_of(meta::current_function()))

// Two members, both defined in the class body, which is a complete-class context: reflection
// inside them sees every member of the class, including the properties declared after the macro.
#define MY_OBJECT                                                                                  \
 public:                                                                                           \
  static constexpr char const* class_name() {                                                      \
    return std::define_static_string(meta::identifier_of(SELF));                                   \
  }                                                                                                \
  static constexpr std::size_t property_count() {                                                  \
    std::size_t n = 0;                                                                             \
    template for (constexpr auto m : std::define_static_array(                                     \
                      meta::members_of(SELF, meta::access_context::unchecked())))                  \
      if constexpr (meta::is_variable(m) && meta::remove_cvref(meta::type_of(m)) == ^^prop_decl) ++n; \
    return n;                                                                                      \
  }

#define MY_PROPERTY(...) static constexpr prop_decl CAT(prop_, __LINE__) = parse_property(#__VA_ARGS__);

template <class T>
void describe() {
  std::println("class {}, property_count() from inside the class: {}", T::class_name(), T::property_count());
  template for (constexpr auto m : std::define_static_array(meta::members_of(^^T, meta::access_context::unchecked()))) {
    if constexpr (meta::is_variable(m) && meta::remove_cvref(meta::type_of(m)) == ^^prop_decl) {
      constexpr prop_decl const& p = [:m:];
      std::println("  property {} : {}  read={} write={} notify={}", p.name, p.type, p.read, p.write, p.notify);
    }
  }
}

class Sensor {
  MY_OBJECT
  MY_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
  MY_PROPERTY(unsigned long long ticks READ ticks)
#ifdef BAD
  MY_PROPERTY(int broken READ broken FLAVOUR sweet)
#endif
 public:
  int level() const { return level_; }
  void setLevel(int v) { level_ = v; }
  unsigned long long ticks() const { return 0; }

 private:
  int level_ = 0;
};

class Lamp {
  MY_OBJECT
  MY_PROPERTY(bool on READ on WRITE setOn)

 public:
  bool on() const { return on_; }
  void setOn(bool v) { on_ = v; }

 private:
  bool on_ = false;
};

int main() {
  describe<Sensor>();
  describe<Lamp>();
}

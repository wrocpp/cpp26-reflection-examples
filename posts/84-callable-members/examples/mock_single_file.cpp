// verify: gcc-only
// Callable data members, part 2: Mock<IClock> for code that is generic over its clock. IClock is only the source of
// names and signatures; Mock<IClock> does not derive from it, so it cannot be passed where an IClock& is required.
// Build: g++-16 -std=c++26 -freflection -Wall -Wextra -Werror mock_single_file.cpp   (Compiler Explorer: g162)
#include <meta>
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <variant>
#include <tuple>
#include <type_traits>
#include <vector>
#include <string_view>

namespace mock {
using std::meta::info;

consteval std::vector<info> methods_of(info api) {
  std::vector<info> out;
  for (info m : std::meta::members_of(api, std::meta::access_context::unchecked()))
    if (std::meta::is_function(m) && !std::meta::is_special_member_function(m) &&
        !std::meta::is_operator_function(m))
      out.push_back(m);
  return out;
}

// Unique per method (the reflection is a template argument). State lives in the member: no owner needed.
template <info M, class Sig = typename [:std::meta::type_of(M):]> struct method;
template <info M, class R, class... A> struct method<M, R(A...)> {
  using args_t = std::tuple<std::remove_cvref_t<A>...>;
  using value_t = std::conditional_t<std::is_void_v<R>, std::monostate, std::remove_cvref_t<R>>;
  std::optional<value_t> ret{};
  int calls = 0;
  std::vector<args_t> recorded{};

  template <class V> method& returns(V&& v) requires(!std::is_void_v<R>) { ret = std::forward<V>(v); return *this; }
  R operator()(A... a) {
    ++calls;
    recorded.emplace_back(a...);
    if constexpr (!std::is_void_v<R>) return ret ? *ret : value_t{};
  }
  void verify_called(int n) const {
    if (calls != n) {
      std::fprintf(stderr, "mock: %s expected %d call(s), got %d\n", std::string(std::meta::identifier_of(M)).c_str(), n, calls);
      std::abort();
    }
  }
};

namespace detail {
template <class I> struct gen {
  struct type;
  consteval {
    std::vector<info> specs;
    for (info m : methods_of(^^I))
      specs.push_back(std::meta::data_member_spec(
          std::meta::substitute(^^method, {std::meta::reflect_constant(m)}),
          {.name = std::meta::identifier_of(m)}));
    std::meta::define_aggregate(^^type, specs);
  }
};
}  // namespace detail
template <class I> using Mock = typename detail::gen<I>::type;
}  // namespace mock
#include <string>
struct IClock {                         // an ordinary abstract interface
  virtual ~IClock() = default;
  virtual int now() = 0;
  virtual void sleep_for(int ms) = 0;
  virtual std::string label(int id, bool upper) = 0;
};
struct RealClock { int now() { return 42; } void sleep_for(int) {} std::string label(int, bool) { return "real"; } };

template <class Clock> struct Sut {
  Clock& c;
  int twice() { return 2 * c.now(); }
#ifdef V_FORGOTTEN
  int broken() { return c.nowish(); }     // no such method in IClock
#endif
#ifdef V_MISTYPED
  int broken() { return c.now(5); }       // wrong arity
#endif
#ifdef V_MISTYPED2
  void broken() { c.sleep_for("soon"); }  // wrong type
#endif
  std::string tag() { c.sleep_for(10); return c.label(7, true); }
};

#ifdef V_VIRTUAL_SLOT
int use_interface(IClock& c) { return c.now(); }
#endif

int main() {
  RealClock real;
  Sut<RealClock> s1{real};
  mock::Mock<IClock> m;
  m.now.returns(21);
  m.label.returns(std::string("MOCK"));
  Sut<mock::Mock<IClock>> s2{m};
#if defined(V_FORGOTTEN) || defined(V_MISTYPED) || defined(V_MISTYPED2)
  s2.broken();
#endif
  int a = s1.twice(), b = s2.twice();
  std::string t = s2.tag();
  std::printf("real=%d mock=%d tag=%s now.calls=%d sleep.calls=%d sizeof(Mock<IClock>)=%zu\n", a, b, t.c_str(),
              m.now.calls, m.sleep_for.calls, sizeof(m));
  m.now.verify_called(1);
  m.sleep_for.verify_called(1);
  bool rec = std::get<0>(m.sleep_for.recorded[0]) == 10 && std::get<1>(m.label.recorded[0]) == true;
#ifdef V_VIRTUAL_SLOT
  return use_interface(m);
#endif
  return (a == 84 && b == 42 && t == "MOCK" && rec) ? 0 : 1;
}

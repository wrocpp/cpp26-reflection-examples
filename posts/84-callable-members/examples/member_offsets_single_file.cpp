// verify: gcc-only
// Callable data members, part 3: where the members sit. The same rpc::client<Api> is generated twice, with and without
// [[no_unique_address]] on the empty callable members. The owner is recovered as `this` minus offset_of(member), so
// calls work in both layouts, including the one where the members are not at offset 0.
// Build: g++-16 -std=c++26 -freflection -Wall -Wextra -Werror member_offsets_single_file.cpp   (Compiler Explorer: g162)
#include <meta>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#include <meta>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

namespace rpc {
using std::meta::info;

// ---- wire format: ints as "<n> ", strings as "<len>:<bytes>" ----
struct reader { std::string_view s; std::size_t pos = 0; };

template <class T> void put(std::string& out, T const& v) {
  if constexpr (std::is_integral_v<T>) out += std::to_string(v) + ' ';
  else { std::string_view sv = v; out += std::to_string(sv.size()) + ':'; out += sv; }
}
template <class T> T get(reader& r) {
  if constexpr (std::is_integral_v<T>) {
    std::size_t e = r.s.find(' ', r.pos);
    T v = static_cast<T>(std::stoll(std::string(r.s.substr(r.pos, e - r.pos))));
    r.pos = e + 1; return v;
  } else {
    std::size_t e = r.s.find(':', r.pos);
    std::size_t n = std::stoul(std::string(r.s.substr(r.pos, e - r.pos)));
    T v(r.s.substr(e + 1, n)); r.pos = e + 1 + n; return v;
  }
}

// ---- reflection helpers ----
consteval std::vector<info> methods_of(info api) {
  std::vector<info> out;
  for (info m : std::meta::members_of(api, std::meta::access_context::unchecked()))
    if (std::meta::is_function(m) && !std::meta::is_special_member_function(m) &&
        !std::meta::is_operator_function(m))
      out.push_back(m);
  return out;
}
consteval info member_named(info cls, std::string_view name) {
  for (info m : std::meta::nonstatic_data_members_of(cls, std::meta::access_context::unchecked()))
    if (std::meta::has_identifier(m) && std::meta::identifier_of(m) == name) return m;
  throw std::meta::exception(std::string("rpc: no member named '") + std::string(name) + "'", cls);
}
consteval info member_of_type(info cls, info type) {
  for (info m : std::meta::nonstatic_data_members_of(cls, std::meta::access_context::unchecked()))
    if (std::meta::type_of(m) == type) return m;
  throw std::meta::exception("rpc: member not found in its owner", cls);
}

struct transport {
  std::function<std::string(std::string_view name, std::string_view bytes)> send;
};

// ---- the callable member: unique per (Owner, M); empty ----
template <info Owner, info M, class Sig = typename [:std::meta::type_of(M):]> struct method;
template <info Owner, info M, class R, class... A> struct method<Owner, M, R(A...)> {
  R operator()(A... a) {
    using O = typename [:Owner:];
    constexpr std::size_t off = std::meta::offset_of(member_of_type(Owner, ^^method)).bytes;
    O& self = *reinterpret_cast<O*>(reinterpret_cast<char*>(this) - off);  // owner recovery
    std::string bytes;
    (put(bytes, a), ...);
    std::string reply = self.rpc_transport.send(std::meta::identifier_of(M), bytes);
    if constexpr (!std::is_void_v<R>) { reader r{reply}; return get<std::remove_cvref_t<R>>(r); }
  }
};

namespace detail {
template <class Api, bool Nua> struct client_gen {
  struct type;
  consteval {
    std::vector<info> specs;
    specs.push_back(std::meta::data_member_spec(^^transport, {.name = "rpc_transport"}));
    for (info m : methods_of(^^Api))
      specs.push_back(std::meta::data_member_spec(
          std::meta::substitute(^^method, {std::meta::reflect_constant(^^type),
                                           std::meta::reflect_constant(m)}),
          {.name = std::meta::identifier_of(m), .no_unique_address = Nua}));
    std::meta::define_aggregate(^^type, specs);
  }
};
}  // namespace detail
template <class Api, bool Nua = true> using client = typename detail::client_gen<Api, Nua>::type;

// ---- server side: dispatch by reflecting Api, call the real Impl ----
template <class Sig> struct sig;
template <class R, class... A> struct sig<R(A...)> {
  template <class F> static std::string call(F&& f, std::string_view bytes) {
    [[maybe_unused]] reader r{bytes};
    std::tuple<std::remove_cvref_t<A>...> args{get<std::remove_cvref_t<A>>(r)...};
    std::string out;
    if constexpr (std::is_void_v<R>) std::apply(f, args);
    else put(out, std::apply(f, args));
    return out;
  }
};
template <class Api, class Impl> struct server {
  Impl impl;
  std::string handle(std::string_view name, std::string_view bytes) {
    std::string out; bool found = false;
    template for (constexpr info m : std::define_static_array(methods_of(^^Api))) {
      if (!found && std::meta::identifier_of(m) == name) {
        found = true;
        constexpr info im = [] consteval {
          for (info c : std::meta::members_of(^^Impl, std::meta::access_context::unchecked()))
            if (std::meta::is_function(c) && std::meta::has_identifier(c) &&
                std::meta::identifier_of(c) == std::meta::identifier_of(m)) return c;
          throw std::meta::exception("rpc: Impl lacks a method of Api", ^^Impl);
        }();
        using Sig = typename [:std::meta::type_of(m):];
        out = sig<Sig>::call([&](auto&&... a) -> decltype(auto) { return impl.[:im:](a...); }, bytes);
      }
    }
    if (!found) throw std::runtime_error("unknown method");
    return out;
  }
};
}  // namespace rpc

struct Api { int add(int, int); std::string greet(std::string_view); void ping(); };
struct ApiImpl {
  int pings = 0;
  int add(int a, int b) { return a + b; }
  std::string greet(std::string_view n) { return "hello " + std::string(n); }
  void ping() { ++pings; }
};

template <class C> bool exercise(const char* label) {
  rpc::server<Api, ApiImpl> srv;
  C client{.rpc_transport = {[&](std::string_view n, std::string_view b) { return srv.handle(n, b); }}};
  constexpr auto ctx = std::meta::access_context::unchecked();
  std::printf("%s: sizeof=%zu", label, sizeof(C));
  template for (constexpr auto m : std::define_static_array(std::meta::nonstatic_data_members_of(^^C, ctx)))
    std::printf("  %s@%zu", std::string(std::meta::identifier_of(m)).c_str(), (std::size_t)std::meta::offset_of(m).bytes);
  int s = client.add(2, 3);
  client.ping();
  std::printf("  add=%d pings=%d\n", s, srv.impl.pings);
  return s == 5 && srv.impl.pings == 1;
}

int main() {
  bool a = exercise<rpc::client<Api, true>>("with [[no_unique_address]]   ");
  bool b = exercise<rpc::client<Api, false>>("without [[no_unique_address]]");
  return (a && b) ? 0 : 1;
}

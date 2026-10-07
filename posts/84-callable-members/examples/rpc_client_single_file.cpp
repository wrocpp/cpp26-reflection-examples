// verify: gcc-only
// Callable data members, part 1: an RPC client with no IDL. rpc::client<Api> is synthesized by define_aggregate
// with one empty [[no_unique_address]] callable data member per member function of Api, plus the transport.
// Build: g++-16 -std=c++26 -freflection -Wall -Wextra -Werror rpc_client_single_file.cpp   (Compiler Explorer: g162)
#include <meta>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#ifndef RPC_NUA
#define RPC_NUA true
#endif
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
template <class Api> struct client_gen {
  struct type;
  consteval {
    std::vector<info> specs;
    specs.push_back(std::meta::data_member_spec(^^transport, {.name = "rpc_transport"}));
    for (info m : methods_of(^^Api))
      specs.push_back(std::meta::data_member_spec(
          std::meta::substitute(^^method, {std::meta::reflect_constant(^^type),
                                           std::meta::reflect_constant(m)}),
          {.name = std::meta::identifier_of(m), .no_unique_address = RPC_NUA}));
    std::meta::define_aggregate(^^type, specs);
  }
};
}  // namespace detail
template <class Api> using client = typename detail::client_gen<Api>::type;

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
#include <cstdio>
#include <stdexcept>
struct Api { int add(int, int); std::string greet(std::string_view); void ping(); };
struct ApiImpl {
  int pings = 0;
  int add(int a, int b) { return a + b; }
  std::string greet(std::string_view n) { return "hello " + std::string(n); }
  void ping() { ++pings; }
};
int main() {
  rpc::server<Api, ApiImpl> srv;
  rpc::client<Api> client{.rpc_transport = {[&](std::string_view n, std::string_view b) { return srv.handle(n, b); }}};
#ifdef V_BADTYPE
  client.add("x", 3);
#elifdef V_UNKNOWN
  client.sub(2, 3);
#elifdef V_ARITY
  client.add(2);
#else
  int s = client.add(2, 3);
  std::string g = client.greet("wro");
  client.ping(); client.ping();
  std::printf("add=%d greet=%s pings=%d sizeof(client)=%zu sizeof(transport)=%zu\n", s, g.c_str(), srv.impl.pings,
              sizeof(client), sizeof(rpc::transport));
  return (s == 5 && g == "hello wro" && srv.impl.pings == 2) ? 0 : 1;
#endif
}

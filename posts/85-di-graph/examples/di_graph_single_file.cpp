// A dependency-injection graph resolved at compile time from constructor reflection.
// The whole graph is planned by one consteval function; the run-time work is the constructor calls.
// verify: gcc-only
// verify: gcc-options: -std=c++26 -freflection -Wall -Wextra -Werror
#include <memory>
#include <meta>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace di {
using std::meta::info;

struct singleton_t {};  // class annotation: one instance per graph (default: transient)
inline constexpr singleton_t singleton{};
struct inject_t {};     // constructor annotation: the constructor the graph must call
inline constexpr inject_t inject{};

inline constexpr std::size_t max_name_length = 24;  // qualifier names live in a structural char array
struct name_t {
  char v[max_name_length]{};
  friend constexpr bool operator==(name_t const&, name_t const&) = default;
};
consteval name_t named(std::string_view s) {  // [[=di::named("fixed")]] on a constructor parameter
  name_t n;
  for (std::size_t i = 0; i < s.size() && i + 1 < max_name_length; ++i) n.v[i] = s[i];
  return n;
}

struct binding {
  info iface, impl;
  name_t name{};
  consteval binding named(std::string_view s) const { auto b = *this; b.name = ::di::named(s); return b; }
};
template <class I, class C> inline constexpr binding bind{^^I, ^^C};

namespace detail {
consteval std::string itos(std::size_t v) {  // std::to_string is not constexpr in libstdc++ 16.2
  std::string s = v ? "" : "0";
  for (; v; v /= 10) s.insert(s.begin(), char('0' + v % 10));
  return s;
}
enum class form { ref, ptr, unique, shared };
struct arg { form f; std::size_t node; };
struct node { info type; bool stored; std::size_t first_arg, n_args; std::size_t slot; };
struct plan_t {  // everything the run-time part needs, promoted to static storage
  const node* nodes = nullptr; std::size_t n_nodes = 0;
  const arg* args = nullptr;
  std::size_t n_stored = 0;
  std::string_view error, text;
};

template <class T> union slot { T v; slot() {} ~slot() {} };  // raw storage: constructed in order by the graph

struct builder {
  struct bnode { info type; bool stored; std::vector<arg> args; };
  std::vector<binding> bs;
  std::vector<bnode> nodes;
  std::vector<info> path;
  std::string err;

  consteval std::string nm(info t) const {
    return std::string(has_identifier(t) ? identifier_of(t) : display_string_of(t));
  }
  consteval std::string path_text(std::size_t from, std::string_view sep = " -> ") const {
    std::string s;
    for (std::size_t i = from; i < path.size(); ++i) s += (i > from ? std::string(sep) : "") + nm(path[i]);
    return s;
  }
  consteval void fail([[maybe_unused]] info at, std::string msg) {
#ifdef DI_THROW
    throw std::meta::exception(std::u8string(msg.begin(), msg.end()), at);
#else
    if (err.empty()) err = std::move(msg);
#endif
  }
  consteval bool has_annotation(info r, info type) const {
    for (info a : annotations_of(r)) if (remove_cvref(type_of(a)) == type) return true;
    return false;
  }
  consteval name_t qualifier(info param) const {
    for (info a : annotations_of(param)) if (remove_cvref(type_of(a)) == ^^name_t) return extract<name_t>(a);
    return {};
  }
  // Constructor rule: the one annotated [[=di::inject]], else the only public user-declared one, else none (default).
  consteval info pick_ctor(info c) {
    std::vector<info> pub, inj; std::size_t declared = 0;
    for (info m : members_of(c, std::meta::access_context::unchecked())) {
      if (!is_constructor(m) || !is_user_declared(m) || is_copy_constructor(m) || is_move_constructor(m)) continue;
      ++declared;
      if (!is_public(m)) continue;
      pub.push_back(m);
      if (has_annotation(m, ^^inject_t)) inj.push_back(m);
    }
    if (inj.size() == 1) return inj[0];
    if (inj.size() > 1) fail(c, nm(c) + " has " + itos(inj.size()) + " constructors annotated [[=di::inject]]; keep one");
    else if (pub.size() == 1) return pub[0];
    else if (pub.size() > 1) fail(c, nm(c) + " has " + itos(pub.size()) +
                                     " public constructors and none is annotated [[=di::inject]] (path: " + path_text(0) + ")");
    else if (declared > 0) fail(c, nm(c) + " has no public constructor (path: " + path_text(0) + ")");
    return info{};
  }
  consteval std::size_t build(info c) {  // concrete type -> node index; nodes end up in dependency order
    for (std::size_t i = 0; i < path.size(); ++i)
      if (path[i] == c) { fail(c, "dependency cycle: " + path_text(i) + " -> " + nm(c)); return 0; }
    bool single = has_annotation(c, ^^singleton_t);
    if (single) for (std::size_t i = 0; i < nodes.size(); ++i) if (nodes[i].type == c) return i;
    path.push_back(c);
    bnode n{c, single, {}};
    if (info ctor = pick_ctor(c); ctor != info{}) {
      auto ps = parameters_of(ctor);
      for (std::size_t i = 0; i < ps.size() && err.empty(); ++i) n.args.push_back(resolve(c, ps[i], i));
    }
    path.pop_back();
    nodes.push_back(std::move(n));
    return nodes.size() - 1;
  }
  consteval arg resolve(info consumer, info param, std::size_t i) {
    info t = type_of(param), tgt = t; form f = form::ref;
    auto smart = [&](info tpl) { return has_template_arguments(t) && template_of(t) == tpl; };
    if (is_lvalue_reference_type(t)) tgt = remove_cv(remove_reference(t));
    else if (is_pointer_type(t)) { f = form::ptr; tgt = remove_cv(remove_pointer(t)); }
    else if (smart(^^std::unique_ptr)) { f = form::unique; tgt = template_arguments_of(t)[0]; }
    else if (smart(^^std::shared_ptr)) { f = form::shared; tgt = template_arguments_of(t)[0]; }
    else { fail(consumer, "unsupported parameter form " + std::string(display_string_of(t)) + " in " + nm(consumer)); return {f, 0}; }

    // Message text is built only on failure: building it per edge made a 300-type graph exceed the constexpr ops limit.
    auto who = [&] {
      return "parameter '" + (has_identifier(param) ? std::string(identifier_of(param)) : "#" + itos(i)) + "' of " + nm(consumer) + "'s constructor";
    };
    name_t q = qualifier(param);
    auto qs = [&] { return q == name_t{} ? std::string() : std::string(" named \"") + q.v + "\""; };
    std::size_t found = 0; info impl = tgt;
    for (auto const& b : bs) if (b.iface == tgt && b.name == q) { ++found; impl = b.impl; }
    if (found > 1) fail(consumer, "ambiguous binding: " + itos(found) + " bindings for " + nm(tgt) + qs() +
                                    " serve " + who() + " (path: " + path_text(0) + ")");
    else if (found == 0 && q != name_t{})
      fail(consumer, "qualifier has no binding: " + who() + " asks for " + nm(tgt) + qs() + ", but no di::bind<" + nm(tgt) +
                       ", ...>.named(\"" + q.v + "\") exists");
    else if (found == 0 && is_abstract_type(tgt))
      fail(consumer, "missing binding: " + nm(tgt) + " is an interface with no di::bind; required by " + who() +
                       " (path: " + path_text(0) + ")");
    if (!err.empty()) return {f, 0};
    std::size_t idx = build(impl);
    if (!err.empty()) return {f, 0};
    if (f == form::unique && nodes[idx].stored && has_annotation(impl, ^^singleton_t))
      fail(consumer, "unique_ptr<" + nm(impl) + "> requested by " + who() + ", but " + nm(impl) + " is a singleton");
    if (f == form::ref || f == form::ptr) nodes[idx].stored = true;  // transient by reference: the graph owns that instance
    return {f, idx};
  }
};

consteval plan_t compute(info root, std::vector<binding> bs) {
  builder b{.bs = std::move(bs), .nodes = {}, .path = {}, .err = {}};
  for (auto const& x : b.bs) if (x.iface != x.impl && !is_base_of_type(x.iface, x.impl))
    b.fail(root, "bind<" + b.nm(x.iface) + ", " + b.nm(x.impl) + ">: " + b.nm(x.impl) + " does not derive from " + b.nm(x.iface));
  std::size_t r = b.err.empty() ? b.build(root) : 0;
  if (!b.err.empty()) { plan_t p; p.error = std::string_view{std::define_static_string(b.err)}; return p; }
  b.nodes[r].stored = true;
  std::vector<node> ns; std::vector<arg> as; std::size_t stored = 0; std::string text;
  for (std::size_t i = 0; i < b.nodes.size(); ++i) {
    auto const& n = b.nodes[i];
    ns.push_back({n.type, n.stored, as.size(), n.args.size(), n.stored ? stored++ : std::size_t(-1)});
    text += "  n" + itos(i) + (n.stored ? " [" + itos(ns.back().slot) + "] " : " [heap] ") + b.nm(n.type) + "(";
    for (auto const& a : n.args) { as.push_back(a); text += "n" + itos(a.node) + (&a == &n.args.back() ? "" : ", "); }
    text += ")\n";
  }
  return {std::define_static_array(ns).data(), ns.size(), std::define_static_array(as).data(), stored,
          {}, std::string_view{std::define_static_string(text)}};
}

consteval std::vector<info> slot_specs(plan_t const& p) {
  std::vector<info> v;
  for (std::size_t i = 0; i < p.n_nodes; ++i) if (p.nodes[i].stored)
    v.push_back(data_member_spec(substitute(^^slot, {p.nodes[i].type}),
                                 {.name = std::define_static_string("n" + itos(i) + "_" + std::string(identifier_of(p.nodes[i].type)))}));
  return v;
}
}  // namespace detail

// The graph owns every singleton (and every transient that is injected by reference) in ONE synthesized aggregate.
template <class Root, auto... Bs>
class graph {
  static constexpr detail::plan_t P = detail::compute(^^Root, {Bs...});
  static_assert(P.error.empty(), P.error);
  struct slots;
  consteval { define_aggregate(^^slots, detail::slot_specs(P)); }
  slots s;
  static constexpr std::size_t N = P.n_nodes;

  template <std::size_t I> auto& at() {
    constexpr auto m = std::meta::nonstatic_data_members_of(^^slots, std::meta::access_context::unchecked())[P.nodes[I].slot];
    return s.[:m:].v;
  }
  template <std::size_t I> using type_of_node = [: P.nodes[I].type :];
  template <std::size_t I, class F> decltype(auto) with_args(F&& f) {
    return [&]<std::size_t... J>(std::index_sequence<J...>) -> decltype(auto) { return f(arg<I, J>()...); }
        (std::make_index_sequence<P.nodes[I].n_args>{});
  }
  template <std::size_t I, std::size_t J> decltype(auto) arg() {
    constexpr auto a = P.args[P.nodes[I].first_arg + J];
    using T = type_of_node<a.node>;
    if constexpr (a.f == detail::form::ref) return (at<a.node>());
    else if constexpr (a.f == detail::form::ptr) return &at<a.node>();
    else if constexpr (a.f == detail::form::unique)
      return with_args<a.node>([&](auto&&... x) { return std::make_unique<T>(std::forward<decltype(x)>(x)...); });
    else if constexpr (P.nodes[a.node].stored)  // shared singleton: non-owning alias, the graph keeps ownership
      return std::shared_ptr<T>(std::shared_ptr<void>{}, &at<a.node>());
    else return with_args<a.node>([&](auto&&... x) { return std::make_shared<T>(std::forward<decltype(x)>(x)...); });
  }
  template <std::size_t I> void build() {
    if constexpr (P.nodes[I].stored)
      with_args<I>([&](auto&&... x) {
        if constexpr (sizeof...(x) == 0) ::new (static_cast<void*>(&at<I>())) type_of_node<I>;  // default-init, as `T t;` does
        else std::construct_at(&at<I>(), std::forward<decltype(x)>(x)...);
      });
  }
  template <std::size_t I> void destroy() { if constexpr (P.nodes[I].stored) std::destroy_at(&at<I>()); }

public:
  graph() { [&]<std::size_t... I>(std::index_sequence<I...>) { (build<I>(), ...); }(std::make_index_sequence<N>{}); }
  ~graph() { [&]<std::size_t... I>(std::index_sequence<I...>) { (destroy<N - 1 - I>(), ...); }(std::make_index_sequence<N>{}); }
  graph(graph const&) = delete;
  graph& operator=(graph const&) = delete;
  Root& root() { return at<N - 1>(); }
  static constexpr std::string_view plan() { return P.text; }
  static constexpr std::size_t stored_instances = P.n_stored;
};

template <class Root, auto... Bs> auto make() { return graph<Root, Bs...>{}; }
}  // namespace di
// ---- the application: plain C++, DI shows only as annotations and one binding list ----
#include <cstdio>
struct [[=di::singleton]] Config { int port = 8080; };
struct ILogger { virtual ~ILogger() = default; virtual void log(std::string_view) = 0; };
struct [[=di::singleton]] ConsoleLogger final : ILogger {
  Config const& cfg; explicit ConsoleLogger(Config const& c) : cfg(c) {}
  void log(std::string_view m) override { std::printf("[port %d] %.*s\n", cfg.port, int(m.size()), m.data()); }
};
struct IClock { virtual ~IClock() = default; virtual long now() const = 0; };
struct [[=di::singleton]] SystemClock final : IClock { long now() const override { return 1700000000; } };
struct [[=di::singleton]] FixedClock final : IClock { long now() const override { return 42; } };
struct Cache { ILogger& log; explicit Cache(ILogger& l) : log(l) {} };            // transient
struct Validator { int rules = 3; };                                              // transient, via unique_ptr
struct Repo { ILogger& log; Cache& cache; Repo(ILogger& l, Cache& c) : log(l), cache(c) {} };
struct Billing {
  Repo& repo; IClock& clock; ILogger* log; std::unique_ptr<Validator> v;
  Billing(Repo& r, [[=di::named("fixed")]] IClock& c, ILogger* l, std::unique_ptr<Validator> val)
      : repo(r), clock(c), log(l), v(std::move(val)) {}
};
struct Orders { Repo& repo; IClock& clock; Orders(Repo& r, IClock& c) : repo(r), clock(c) {} };
struct Api { Billing& b; Orders& o; Api(Billing& x, Orders& y) : b(x), o(y) {} };

int main() {
  auto g = di::make<Api, di::bind<ILogger, ConsoleLogger>, di::bind<IClock, SystemClock>,
                    di::bind<IClock, FixedClock>.named("fixed")>();
  std::printf("plan:\n%s", std::string(decltype(g)::plan()).c_str());
  Api& a = g.root();
  a.b.log->log("hello from the graph");
  std::printf("billing clock %ld, orders clock %ld\n", a.b.clock.now(), a.o.clock.now());
  std::printf("same logger: %d, same repo cache: %d, validator rules: %d\n", &a.b.repo.log == &a.o.repo.log,
              &a.b.repo.cache == &a.o.repo.cache, a.b.v->rules);
}

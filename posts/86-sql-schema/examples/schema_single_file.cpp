// verify: gcc-only
// A SQL schema in a string literal becomes checked C++ types: a consteval DDL parser, row structs from define_aggregate
// (std::optional for nullable columns), queries checked against the schema, and a drift check for hand-written structs.
// The #embed variant of the same engine is schema_embed.cpp (it needs schema.sql next to it).
// Build: g++-16 -std=c++26 -freflection -Wall -Wextra -Werror schema_single_file.cpp   (Compiler Explorer: g162)
#include <cstddef>
#include <cstdint>
#include <meta>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace sqlschema {
using std::meta::info;

enum class ctype { integer, bigint, real, text, boolean };
// structural description: arrays live in static storage (define_static_array / define_static_string), so this is a valid NTTP.
struct column { char const* name; ctype type; bool pk; bool not_null; };
struct table { char const* name; column const* cols; std::size_t ncols; };
struct schema_t {
  table const* tables; std::size_t ntables; char const* error;
  constexpr bool ok() const { return error == nullptr; }
  constexpr std::string_view message() const { return error ? std::string_view{error} : std::string_view{}; }
};

template <std::size_t N> struct fixed_string {
  char v[N]{};
  constexpr fixed_string(char const (&s)[N]) { for (std::size_t i = 0; i < N; ++i) v[i] = s[i]; }
  constexpr std::string_view sv() const { return {v, N - 1}; }
};
template <std::size_t N> fixed_string(char const (&)[N]) -> fixed_string<N>;

namespace detail {
inline constexpr std::size_t npos = ~std::size_t{0};

consteval std::string itos(std::size_t v) {  // std::to_string is not constexpr in libstdc++ 16.2
  std::string s = v ? "" : "0";
  for (; v; v /= 10) s.insert(s.begin(), char('0' + v % 10));
  return s;
}
consteval std::string_view keep(std::string const& s) { return std::define_static_string(s); }
consteval char upper(char c) { return (c >= 'a' && c <= 'z') ? char(c - 'a' + 'A') : c; }
consteval bool id_start(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
consteval bool id_char(char c) { return id_start(c) || (c >= '0' && c <= '9'); }

consteval bool ieq(std::string_view a, std::string_view b) {  // SQL names are case-insensitive; C++ members keep the spelling
  if (a.size() != b.size()) return false;
  for (std::size_t i = 0; i < a.size(); ++i) if (upper(a[i]) != upper(b[i])) return false;
  return true;
}

struct cursor {  // a tiny lexer over DDL or SELECT text
  std::string_view s; std::size_t i = 0; std::string_view file;
  consteval bool eof() const { return i >= s.size(); }
  consteval void skip() {  // whitespace and `--` line comments
    while (i < s.size()) {
      char c = s[i];
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r') ++i;
      else if (c == '-' && i + 1 < s.size() && s[i + 1] == '-') { while (i < s.size() && s[i] != '\n') ++i; }
      else break;
    }
  }
  consteval std::string where() { skip(); return where_at(i); }  // file:line:col of the next token
  consteval std::string where_at(std::size_t pos) const {  // O(pos): called on errors only
    std::size_t line = 1, col = 1;
    for (std::size_t k = 0; k < pos; ++k) { if (s[k] == '\n') { ++line; col = 1; } else ++col; }
    return std::string(file) + ":" + itos(line) + ":" + itos(col);
  }
  consteval std::string found() {  // what is at the cursor, for "found '...'"
    skip(); if (eof()) return "end of input";
    std::size_t j = i; if (id_start(s[j])) { while (j < s.size() && id_char(s[j])) ++j; } else ++j;
    return "'" + std::string(s.substr(i, j - i)) + "'";
  }
  consteval bool eat(char c) { skip(); if (!eof() && s[i] == c) { ++i; return true; } return false; }
  consteval bool eat_kw(std::string_view kw) {  // case-insensitive whole word
    skip(); std::size_t j = i, k = 0;
    while (k < kw.size() && j < s.size() && upper(s[j]) == kw[k]) { ++j; ++k; }
    if (k != kw.size() || (j < s.size() && id_char(s[j]))) return false;
    i = j; return true;
  }
  consteval bool ident(std::string& out) {  // name or "quoted name"
    skip(); out.clear(); if (eof()) return false;
    if (s[i] == '"') {
      std::size_t j = i + 1; while (j < s.size() && s[j] != '"') ++j;
      if (j >= s.size()) return false;
      out = std::string(s.substr(i + 1, j - i - 1)); i = j + 1; return true;
    }
    if (!id_start(s[i])) return false;
    std::size_t j = i; while (j < s.size() && id_char(s[j])) ++j;
    out = std::string(s.substr(i, j - i)); i = j; return true;
  }
};

inline constexpr std::string_view cxx_keywords[] = {"alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t", "class", "compl", "concept", "const", "contract_assert", "consteval", "constexpr", "constinit", "const_cast", "continue", "co_await", "co_return", "co_yield", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected", "public", "register", "reinterpret_cast", "requires", "return", "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "template", "this", "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"};
consteval bool is_cxx_keyword(std::string_view n) { for (auto k : cxx_keywords) if (k == n) return true; return false; }
// what define_aggregate would refuse, said with the column in the message ("name is a keyword" names nothing)
consteval std::string member_name_problem(std::string_view n) {
  if (n.empty() || !id_start(n[0])) return "'" + std::string(n) + "' does not start with a letter or underscore, so it cannot name a C++ member";
  for (char ch : n) if (!id_char(ch)) return "'" + std::string(n) + "' contains a character that is not allowed in a C++ identifier";
  if (is_cxx_keyword(n)) return "'" + std::string(n) + "' is a C++ keyword and cannot name a struct member";
  return {};
}

struct col_b { std::string name; ctype ty; bool pk; bool nn; };
struct tab_b { std::string name; std::vector<col_b> cols; };

consteval schema_t parse_impl(std::string const& text, std::string_view file) {
  cursor c{text, 0, file}; std::vector<tab_b> ts; std::string err;
  auto fail = [&](std::string const& m) { err = c.where() + ": " + m; };
  while (err.empty()) {
    c.skip(); if (c.eof()) break;
    if (!(c.eat_kw("CREATE") && c.eat_kw("TABLE"))) { fail("expected CREATE TABLE, found " + c.found()); break; }
    std::string tn;
    if (!c.ident(tn)) { fail("expected a table name after CREATE TABLE, found " + c.found()); break; }
    for (auto const& t : ts) if (ieq(t.name, tn)) { fail("table '" + tn + "' is defined twice"); break; }
    if (!err.empty()) break;
    std::string tctx = "table '" + tn + "'";
    if (!c.eat('(')) { fail(tctx + ": expected '(' after the table name, found " + c.found()); break; }
    tab_b t{tn, {}};
    for (;;) {
      std::string cn; c.skip(); std::size_t at = c.i;
      if (!c.ident(cn)) { fail(tctx + ": expected a column name, found " + c.found()); break; }
      std::string cctx = tctx + ", column '" + cn + "'";
      if (auto why = member_name_problem(cn); !why.empty()) { err = c.where_at(at) + ": " + cctx + ": " + why; break; }
      for (auto const& o : t.cols) if (ieq(o.name, cn)) { fail(cctx + ": defined twice"); break; }
      if (!err.empty()) break;
      col_b col{cn, ctype::integer, false, false};
      if (c.eat_kw("INTEGER")) col.ty = ctype::integer;
      else if (c.eat_kw("BIGINT")) col.ty = ctype::bigint;
      else if (c.eat_kw("REAL")) col.ty = ctype::real;
      else if (c.eat_kw("TEXT")) col.ty = ctype::text;
      else if (c.eat_kw("BOOLEAN")) col.ty = ctype::boolean;
      else { fail(cctx + ": expected a type (INTEGER, BIGINT, REAL, TEXT or BOOLEAN), found " + c.found()); break; }
      for (;;) {
        if (c.eat_kw("NOT")) { if (!c.eat_kw("NULL")) { fail(cctx + ": expected NULL after NOT, found " + c.found()); break; } col.nn = true; }
        else if (c.eat_kw("PRIMARY")) { if (!c.eat_kw("KEY")) { fail(cctx + ": expected KEY after PRIMARY, found " + c.found()); break; } col.pk = true; }
        else break;
      }
      if (!err.empty()) break;
      t.cols.push_back(col);
      if (c.eat(',')) continue;
      if (c.eat(')')) break;
      fail(cctx + ": expected ',' or ')' after the column definition, found " + c.found()); break;
    }
    if (!err.empty()) break;
    if (!c.eat(';')) { fail(tctx + ": expected ';' after ')', found " + c.found()); break; }
    ts.push_back(std::move(t));
  }
  if (!err.empty()) return {nullptr, 0, std::define_static_string(err)};
  std::vector<table> out;
  for (auto const& t : ts) {
    std::vector<column> cs;
    for (auto const& k : t.cols) cs.push_back({std::define_static_string(k.name), k.ty, k.pk, k.nn || k.pk});
    out.push_back({std::define_static_string(t.name), std::define_static_array(cs).data(), cs.size()});
  }
  return {std::define_static_array(out).data(), out.size(), nullptr};
}

// ---- types ----
consteval info base_type(ctype t) {  // dealiased: type_of(member) compares equal to these
  switch (t) {
    case ctype::integer: case ctype::bigint: return std::meta::dealias(^^::int64_t);
    case ctype::real: return ^^double;
    case ctype::text: return std::meta::dealias(^^std::string);
    case ctype::boolean: return ^^bool;
  }
  return ^^void;
}
consteval info column_type(column const& c) { return c.not_null ? base_type(c.type) : std::meta::substitute(^^std::optional, {base_type(c.type)}); }
consteval std::string type_name(info t) {
  t = std::meta::dealias(t);
  if (t == std::meta::dealias(^^::int64_t)) return "int64_t";
  if (t == std::meta::dealias(^^std::string)) return "std::string";
  if (t == ^^double) return "double";
  if (t == ^^bool) return "bool";
  if (std::meta::has_template_arguments(t) && std::meta::template_of(t) == ^^std::optional)
    return "std::optional<" + type_name(std::meta::template_arguments_of(t)[0]) + ">";
  return std::string(std::meta::display_string_of(t));
}
consteval std::string_view sql_name(ctype t) {
  switch (t) {
    case ctype::integer: return "INTEGER"; case ctype::bigint: return "BIGINT"; case ctype::real: return "REAL";
    case ctype::text: return "TEXT"; case ctype::boolean: return "BOOLEAN";
  }
  return "?";
}
consteval std::size_t find_table(schema_t const& S, std::string_view n) {
  for (std::size_t i = 0; i < S.ntables; ++i) if (ieq(S.tables[i].name, n)) return i;
  return npos;
}
consteval std::size_t find_col(table const& t, std::string_view n) {
  for (std::size_t i = 0; i < t.ncols; ++i) if (ieq(t.cols[i].name, n)) return i;
  return npos;
}
consteval std::string list_tables(schema_t const& S) { std::string r; for (std::size_t i = 0; i < S.ntables; ++i) r += (i ? ", " : "") + std::string(S.tables[i].name); return r; }
consteval std::string list_cols(table const& t) { std::string r; for (std::size_t i = 0; i < t.ncols; ++i) r += (i ? ", " : "") + std::string(t.cols[i].name); return r; }

consteval std::vector<info> row_specs(schema_t const& S, std::size_t ti) {
  std::vector<info> v;
  if (ti == npos) return v;
  auto const& t = S.tables[ti];
  for (std::size_t j = 0; j < t.ncols; ++j) v.push_back(std::meta::data_member_spec(column_type(t.cols[j]), {.name = t.cols[j].name}));
  return v;
}

// ---- queries ----
struct qplan {
  std::string_view error;                // empty when the query checks
  std::size_t table = npos;
  std::size_t const* sel = nullptr; std::size_t nsel = 0;  // selected column indices
  std::size_t param_col = npos;          // column compared with `?`, or npos
  std::string_view op;
};
consteval qplan plan_query(schema_t const& S, std::string_view q) {
  cursor c{q, 0, ""}; std::string err; qplan p;
  auto fail = [&](std::string const& m) { if (err.empty()) err = m + " in query \"" + std::string(q) + "\""; };
  if (!c.eat_kw("SELECT")) fail("expected SELECT, found " + c.found());
  bool star = false; std::vector<std::string> names; std::string n;
  if (err.empty()) {
    if (c.eat('*')) star = true;
    else for (;;) {
      if (!c.ident(n)) { fail("expected a column name or *, found " + c.found()); break; }
      names.push_back(n);
      if (!c.eat(',')) break;
    }
  }
  std::string tn;
  if (err.empty() && !c.eat_kw("FROM")) fail("expected FROM, found " + c.found());
  if (err.empty() && !c.ident(tn)) fail("expected a table name after FROM, found " + c.found());
  if (err.empty()) {
    p.table = find_table(S, tn);
    if (p.table == npos) fail("unknown table '" + tn + "' (tables: " + list_tables(S) + ")");
  }
  std::vector<std::size_t> sel;
  if (err.empty()) {
    auto const& t = S.tables[p.table];
    if (star) for (std::size_t j = 0; j < t.ncols; ++j) sel.push_back(j);
    for (auto const& nm : names) {
      auto j = find_col(t, nm);
      if (j == npos) { fail("unknown column '" + nm + "' in table '" + tn + "' (columns: " + list_cols(t) + ")"); break; }
      sel.push_back(j);
    }
  }
  if (err.empty() && c.eat_kw("WHERE")) {
    auto const& t = S.tables[p.table];
    if (!c.ident(n)) fail("expected a column name after WHERE, found " + c.found());
    else if ((p.param_col = find_col(t, n)) == npos) fail("unknown column '" + n + "' in WHERE of table '" + tn + "' (columns: " + list_cols(t) + ")");
    if (err.empty()) {
      if (c.eat('<')) p.op = c.eat('=') ? "<=" : c.eat('>') ? "<>" : "<";
      else if (c.eat('>')) p.op = c.eat('=') ? ">=" : ">";
      else if (c.eat('=')) p.op = "=";
      else fail("expected a comparison operator (= < > <= >= <>) after column '" + n + "', found " + c.found());
    }
    if (err.empty() && !c.eat('?')) fail("expected '?' after the operator on column '" + n + "' (literals are not supported), found " + c.found());
  }
  if (err.empty() && c.eat_kw("ORDER")) {
    auto const& t = S.tables[p.table];
    if (!c.eat_kw("BY")) fail("expected BY after ORDER, found " + c.found());
    else if (!c.ident(n)) fail("expected a column name after ORDER BY, found " + c.found());
    else if (find_col(t, n) == npos) fail("unknown column '" + n + "' in ORDER BY of table '" + tn + "' (columns: " + list_cols(t) + ")");
    if (err.empty() && !c.eat_kw("ASC")) c.eat_kw("DESC");
  }
  if (err.empty()) { c.eat(';'); c.skip(); if (!c.eof()) fail("unexpected " + c.found() + " at the end of the query"); }
  if (!err.empty()) { p.error = keep(err); p.table = npos; p.param_col = npos; return p; }
  p.sel = std::define_static_array(sel).data(); p.nsel = sel.size();
  return p;
}
consteval std::vector<info> query_row_specs(schema_t const& S, qplan const& p) {
  std::vector<info> v;
  if (!p.error.empty()) return v;
  auto const& t = S.tables[p.table];
  for (std::size_t k = 0; k < p.nsel; ++k) {
    auto const& c = t.cols[p.sel[k]];
    v.push_back(std::meta::data_member_spec(column_type(c), {.name = c.name}));
  }
  return v;
}
// `?` takes the BASE type of the compared column, nullable or not: a comparison with NULL never matches in SQL.
consteval std::vector<info> param_types(schema_t const& S, qplan const& p) {
  std::vector<info> v;
  if (p.error.empty() && p.param_col != npos) v.push_back(base_type(S.tables[p.table].cols[p.param_col].type));
  return v;
}
consteval std::string param_msg(schema_t const& S, qplan const& p, std::string_view q, std::size_t idx, info got) {
  got = std::meta::dealias(got);
  auto const& t = S.tables[p.table]; auto const& c = t.cols[p.param_col];
  std::string m = "parameter " + itos(idx + 1) + " of the query is " + std::string(t.name) + "." + c.name + " " + std::string(p.op) + " ?";
  if (std::meta::has_template_arguments(got) && std::meta::template_of(got) == ^^std::optional)
    return m + ": got " + type_name(got) + "; a comparison with NULL never matches, pass the " + type_name(base_type(c.type)) + " itself (query \"" + std::string(q) + "\")";
  return m + ": expected " + type_name(base_type(c.type)) + ", got " + type_name(got) + " (query \"" + std::string(q) + "\")";
}
consteval std::string count_msg(schema_t const& S, qplan const& p, std::string_view q, std::size_t got) {
  std::string m = "the query takes " + itos(p.param_col == npos ? 0 : 1) + " parameter(s)";
  if (p.param_col != npos) { auto const& t = S.tables[p.table]; m += " (" + std::string(t.name) + "." + t.cols[p.param_col].name + ": " + type_name(base_type(t.cols[p.param_col].type)) + ")"; }
  return m + ", bind() got " + itos(got) + " (query \"" + std::string(q) + "\")";
}

// ---- drift ----
consteval std::string diff_struct(schema_t const& S, std::size_t ti, info T, std::string_view tname) {
  if (ti == npos) return "unknown table '" + std::string(tname) + "' (tables: " + list_tables(S) + ")";
  auto const& t = S.tables[ti];
  std::string sn(std::meta::identifier_of(T)), out, ord;  // ord: reported only when nothing else is wrong (a missing column shifts every later index)
  auto add = [&](std::string const& m) { out += (out.empty() ? "" : "; ") + m; };
  auto ms = std::meta::nonstatic_data_members_of(T, std::meta::access_context::unchecked());
  for (std::size_t j = 0; j < t.ncols; ++j) {
    auto const& c = t.cols[j]; std::size_t at = npos;
    for (std::size_t k = 0; k < ms.size(); ++k) if (std::meta::identifier_of(ms[k]) == std::string_view(c.name)) at = k;
    std::string col = std::string(t.name) + "." + c.name;
    if (at == npos) { add("missing: column " + col + " has no member in " + sn + " (expected " + type_name(column_type(c)) + " " + c.name + ")"); continue; }
    info have = std::meta::type_of(ms[at]), want = column_type(c);
    if (have != want) {
      bool opt_have = std::meta::has_template_arguments(have) && std::meta::template_of(have) == ^^std::optional;
      info have_base = opt_have ? std::meta::template_arguments_of(have)[0] : have;
      if (have_base == base_type(c.type))
        add("nullability: " + sn + "::" + c.name + " is " + type_name(have) + " but " + col + " is " + (c.not_null ? "NOT NULL" : "nullable") + " (expected " + type_name(want) + ")");
      else add("wrong type: " + sn + "::" + c.name + " is " + type_name(have) + " but " + col + " is " + std::string(sql_name(c.type)) + " (expected " + type_name(want) + ")");
    } else if (at != j) ord += (ord.empty() ? "" : "; ") + std::string("order: ") + sn + "::" + c.name + " is member " + itos(at + 1) + " but " + col + " is column " + itos(j + 1);
  }
  for (auto m : ms) if (find_col(t, std::meta::identifier_of(m)) == npos)
    add("extra: " + sn + "::" + std::string(std::meta::identifier_of(m)) + " (" + type_name(std::meta::type_of(m)) + ") is not a column of " + std::string(t.name));
  return out.empty() ? ord : out;
}
}  // namespace detail

inline consteval schema_t parse(std::string_view text, std::string_view file = "schema.sql") { return detail::parse_impl(std::string(text), file); }
inline consteval schema_t parse(std::span<unsigned char const> bytes, std::string_view file = "schema.sql") {
  std::string s; for (auto b : bytes) s += char(b);
  return detail::parse_impl(s, file);
}

// ---- row types ----
template <schema_t S, std::size_t Table> struct row_for {
  struct type;
  consteval { define_aggregate(^^type, detail::row_specs(S, Table)); }
};
template <schema_t S, fixed_string Name> struct row_named {
  static constexpr std::size_t idx = detail::find_table(S, Name.sv());
  static constexpr std::string_view error = idx == detail::npos
      ? detail::keep("unknown table '" + std::string(Name.sv()) + "' (tables: " + detail::list_tables(S) + ")") : std::string_view{};
  static_assert(error.empty(), error);
  using type = typename row_for<S, idx == detail::npos ? 0 : idx>::type;
};

template <schema_t S, fixed_string Q> struct query_t {
  static constexpr detail::qplan P = detail::plan_query(S, Q.sv());
  static_assert(P.error.empty(), P.error);
  struct row;
  consteval { define_aggregate(^^row, detail::query_row_specs(S, P)); }
  using params = [:std::meta::substitute(^^std::tuple, std::define_static_array(detail::param_types(S, P))):];
  static constexpr std::size_t nparams = std::tuple_size_v<params>;
  struct bound {
    using row_type = row; static constexpr std::string_view sql = Q.sv(); params args;
  };
  template <std::size_t I, class A> static constexpr bool arg_ok() {
    using P0 = std::tuple_element_t<I, params>;
    return requires { P0{std::declval<A>()}; };  // list-init: no narrowing
  }
  template <std::size_t I, class A> static constexpr void check_arg() {
    static_assert(arg_ok<I, A>(), std::string_view(detail::keep(detail::param_msg(S, P, Q.sv(), I, ^^std::decay_t<A>))));
  }
  template <class... A> static constexpr bool all_ok() {
    if constexpr (sizeof...(A) != nparams) return false;
    else return []<std::size_t... I>(std::index_sequence<I...>) { return (arg_ok<I, A>() && ...); }(std::index_sequence_for<A...>{});
  }
  template <class... A> static constexpr bound bind(A&&... a) {
    static_assert(sizeof...(A) == nparams, std::string_view(detail::keep(detail::count_msg(S, P, Q.sv(), sizeof...(A)))));
    if constexpr (all_ok<std::decay_t<A>...>()) {
      return bound{params{std::forward<A>(a)...}};
    } else {
      if constexpr (sizeof...(A) == nparams)
      [&]<std::size_t... I>(std::index_sequence<I...>) { (check_arg<I, std::decay_t<A>>(), ...); }(std::index_sequence_for<A...>{});
      return bound{};
    }
  }
};

template <schema_t S> struct of {
  static constexpr schema_t value = S;
  template <fixed_string N> using row = typename row_named<S, N>::type;
  template <fixed_string Q> using query = query_t<S, Q>;
  template <class T, fixed_string N> static constexpr std::string_view drift =
      detail::keep(detail::diff_struct(S, detail::find_table(S, N.sv()), ^^T, N.sv()));
  template <class T, fixed_string N> static constexpr bool matches = drift<T, N>.empty();
  template <class T, fixed_string N> static consteval bool matches_or_throw() {  // the `throw` flavour, for comparison
    if (!drift<T, N>.empty()) throw std::meta::exception(std::u8string(drift<T, N>.begin(), drift<T, N>.end()), ^^T);
    return true;
  }
  static consteval std::size_t table_count() { return S.ntables; }
};

// name and type of every member, for printing
template <class T> void print_struct(auto&& out, std::string_view label) {
  out << "struct " << label << " {\n";
  template for (constexpr auto m : std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked())))
    out << "  " << std::define_static_string(detail::type_name(std::meta::type_of(m))) << " " << std::meta::identifier_of(m) << ";\n";
  out << "};\n";
}
}  // namespace sqlschema

// ---- the demo: a schema in a string literal becomes checked types ----
#include <iostream>

constexpr auto db = sqlschema::parse(R"SQL(
-- schema: the one place the table shapes are written
CREATE TABLE users (
  id     INTEGER PRIMARY KEY,
  name   TEXT NOT NULL,
  email  TEXT,            -- nullable -> std::optional<std::string>
  age    INTEGER,
  active BOOLEAN NOT NULL
);
CREATE TABLE orders (
  id      INTEGER PRIMARY KEY,
  user_id INTEGER NOT NULL,
  total   REAL NOT NULL,
  note    TEXT
);
)SQL");
static_assert(db.ok(), db.message());     // a syntax error in the schema stops the build with file:line:col
using schema = sqlschema::of<db>;

using Users = schema::row<"users">;       // synthesized by define_aggregate: one member per column
using Adults = schema::query<"SELECT id, name, email FROM users WHERE age >= ? ORDER BY id">;
static_assert(std::is_same_v<Adults::params, std::tuple<std::int64_t>>);     // `?` has the type of the column
static_assert(std::is_same_v<decltype(Adults::row::email), std::optional<std::string>>);

struct User { std::int64_t id; std::string name; std::optional<std::string> email; std::optional<std::int64_t> age; bool active; };
static_assert(schema::matches<User, "users">);                               // a hand-written struct agrees with the table

struct Stale { std::int64_t id; std::string name; std::string email; bool active; double score; };  // a struct that drifted
constexpr std::string_view what_is_wrong = schema::drift<Stale, "users">;    // the text the build prints for a failed drift check

// Misuses, one per macro (each stops the build):
#ifdef V_BARE_MATCHES
static_assert(schema::matches<Stale, "users">);                              // prints no names
#endif
#ifdef V_DRIFT
static_assert(schema::drift<Stale, "users">.empty(), schema::drift<Stale, "users">);  // prints every mismatch
#endif
#ifdef V_UNKNOWN_COLUMN
using Bad = schema::query<"SELECT id, nme FROM users">;
Bad::row bad_row;
#endif
#ifdef V_WRONG_ARG
auto bad_bind = Adults::bind("adult");
#endif
#ifdef V_DOUBLE_ARG
auto bad_bind = Adults::bind(18.5);
#endif
#ifdef V_OPTIONAL_ARG
auto bad_bind = Adults::bind(std::optional<std::int64_t>{18});
#endif
#ifdef V_LITERAL
using Bad = schema::query<"SELECT id FROM users WHERE age > 18">;
Bad::row bad_row;
#endif
#ifdef V_UNKNOWN_TABLE
using Bad = schema::row<"usrs">;
Bad bad_row;
#endif
#ifdef V_BAD_SCHEMA
constexpr auto bad_db = sqlschema::parse("CREATE TABLE users (\n  id INTEGER PRIMARY KEY,\n  name VARCHAR\n);\n", "schema.sql");
static_assert(bad_db.ok(), bad_db.message());
#endif
#ifdef V_KEYWORD_COLUMN
constexpr auto bad_db = sqlschema::parse("CREATE TABLE t (\n  class TEXT\n);\n", "schema.sql");
static_assert(bad_db.ok(), bad_db.message());
#endif

int main() {
  sqlschema::print_struct<Users>(std::cout, "users");
  sqlschema::print_struct<Adults::row>(std::cout, "Adults::row");
  Users u{.id = 1, .name = "Ann", .email = std::nullopt, .age = 34, .active = true};
  auto q = Adults::bind(18);
  std::cout << "sql: " << decltype(q)::sql << "\nparam 0 = " << std::get<0>(q.args) << ", u.name = " << u.name << "\n";
  std::cout << "drift of Stale: " << what_is_wrong << "\n";
}

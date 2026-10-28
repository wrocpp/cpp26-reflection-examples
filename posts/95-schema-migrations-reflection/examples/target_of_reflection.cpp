// verify: gcc-only
// Schema migrations as lambdas (after Leonardo Rossi): reading the migration's target type from the
// lambda's parameter. Leonardo's version uses a member_traits trait specialized once per cv / ref /
// noexcept combination of operator(). This file replaces that whole family with ONE C++26 reflection
// query, shows it is cv/ref/noexcept-robust, and shows it still cannot read a generic lambda -- which
// is why generic lambdas stay out, exactly as Leonardo concluded.
// Build: g++-16 -std=c++26 -freflection -Wall -Wextra -Werror target_of_reflection.cpp
//        (Compiler Explorer: g162)
#include <meta>
#include <cstdint>
#include <cstdio>
#include <type_traits>

struct Database {};

void add_sample_table(Database&) {}
void add_audit_columns(Database&) {}

// ONE reflection query. It reads the first parameter type of the lambda's call operator, whatever the
// cv / ref / noexcept qualifiers are. No per-qualifier specialization exists anywhere below.
consteval std::meta::info call_op_first_param_type(std::meta::info lambda_type) {
  namespace m = std::meta;
  for (std::meta::info mem : m::members_of(lambda_type, m::access_context::unchecked()))
    if (m::is_function(mem) && m::is_operator_function(mem))  // a concrete operator(), not a template
      return m::type_of(m::parameters_of(mem)[0]);
  return ^^void;  // no concrete call operator (e.g. a generic lambda's operator() is a template)
}

template <typename Callable>
using TargetOf = std::remove_cvref_t<typename [: call_op_first_param_type(^^Callable) :]>;

// The one query is robust across the shapes Leonardo needed twelve trait specializations for.
static_assert(std::is_same_v<TargetOf<decltype([](Database&) {})>,          Database>);
static_assert(std::is_same_v<TargetOf<decltype([](const Database&) {})>,    Database>);
static_assert(std::is_same_v<TargetOf<decltype([](Database&&) {})>,         Database>);
static_assert(std::is_same_v<TargetOf<decltype([](Database&) noexcept {})>, Database>);

// A generic lambda's operator() is a function template: the query finds no concrete function and falls
// back to void. Reflection does not rescue the generic case -- there is no single parameter to read.
static_assert(std::is_same_v<TargetOf<decltype([](auto&) {})>, void>);

// Leonardo's Migration, with the target deduced by the reflection TargetOf instead of the trait family.
template <typename Target>
struct Migration {
  std::uint32_t version;
  void (*apply)(Target&);
};

template <typename Callable>
Migration(std::uint32_t, Callable) -> Migration<TargetOf<Callable>>;

int main() {
  // The deduction guide fires: Migration<Database> deduced from the lambda alone.
  Migration m1{9u,  [](Database& db) { add_sample_table(db); }};
  Migration m2{10u, [](Database& db) { add_audit_columns(db); }};
  static_assert(std::is_same_v<decltype(m1), Migration<Database>>);
  static_assert(std::is_same_v<decltype(m2), Migration<Database>>);

  Database db;
  m1.apply(db);
  m2.apply(db);
  std::printf("migrations: v%u then v%u, target deduced as Database by one reflection query\n",
              m1.version, m2.version);
  // Note: the stored apply is void(*)(Database&). TargetOf strips cv/ref to pick the class, but the
  // fixed pointer signature only accepts a Database& lambda; a const Database& lambda deduces the same
  // Migration<Database> yet will not convert to the stored pointer. Deduction and storage are separate.
}

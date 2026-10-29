// verify: gcc-only
// Schema migrations as lambdas (after Leonardo Rossi): reading the migration's target type from the
// lambda's parameter. Leonardo's version uses a member_traits trait specialized once per cv / ref /
// noexcept combination of operator(). This file replaces that whole family with ONE C++26 reflection
// query, constrains the deduction guide with a concept built from the same query, and shows the
// concept both rejects a generic lambda at the call site and closes the "deduction vs storage" gap.
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

// Does the type have exactly one concrete call operator? A generic lambda's operator() is a template,
// so is_function is false and this is false -- which is what makes it usable as a concept.
consteval bool has_one_param_call_op(std::meta::info lambda_type) {
  namespace m = std::meta;
  for (std::meta::info mem : m::members_of(lambda_type, m::access_context::unchecked()))
    if (m::is_function(mem) && m::is_operator_function(mem))
      return m::parameters_of(mem).size() == 1;
  return false;
}

template <typename Callable>
using TargetOf = std::remove_cvref_t<typename [: call_op_first_param_type(^^Callable) :]>;

// A migration mutates the database, so its step must take a mutable lvalue reference. That is exactly
// what the stored void(*)(Target&) can hold, so this concept closes the deduction-vs-storage gap: a
// const& or && lambda is rejected here, not after deducing Migration<Database> and failing to store.
template <typename Callable>
concept MutatingStep =
    has_one_param_call_op(^^Callable) &&
    std::is_lvalue_reference_v<typename [: call_op_first_param_type(^^Callable) :]> &&
    !std::is_const_v<std::remove_reference_t<typename [: call_op_first_param_type(^^Callable) :]>>;

// The query is cv/ref/noexcept-robust: it reads the class from any shape.
static_assert(std::is_same_v<TargetOf<decltype([](Database&) {})>,          Database>);
static_assert(std::is_same_v<TargetOf<decltype([](const Database&) {})>,    Database>);
static_assert(std::is_same_v<TargetOf<decltype([](Database&&) {})>,         Database>);
static_assert(std::is_same_v<TargetOf<decltype([](Database&) noexcept {})>, Database>);
// A generic lambda's operator() is a function template: the query finds no concrete parameter.
static_assert(std::is_same_v<TargetOf<decltype([](auto&) {})>, void>);

// The concept narrows acceptance to what a migration actually is: a mutable step on one target.
static_assert(MutatingStep<decltype([](Database&) {})>);
static_assert(!MutatingStep<decltype([](const Database&) {})>);  // cannot mutate
static_assert(!MutatingStep<decltype([](Database&&) {})>);       // not a database the DB owns
static_assert(!MutatingStep<decltype([](auto&) {})>);            // generic: no concrete operator()
static_assert(!MutatingStep<decltype([]() {})>);                 // no target at all

template <typename Target>
struct Migration {
  std::uint32_t version;
  void (*apply)(Target&);
};

// The guide takes a constrained Callable, not a bare typename. A lambda that is not a MutatingStep is
// rejected here with a deduction failure, instead of deducing Migration<void> and failing downstream.
template <MutatingStep Callable>
Migration(std::uint32_t, Callable) -> Migration<TargetOf<Callable>>;

int main() {
  Migration m1{9u,  [](Database& db) { add_sample_table(db); }};
  Migration m2{10u, [](Database& db) { add_audit_columns(db); }};
  static_assert(std::is_same_v<decltype(m1), Migration<Database>>);
  static_assert(std::is_same_v<decltype(m2), Migration<Database>>);

  Database db;
  m1.apply(db);
  m2.apply(db);
  std::printf("migrations: v%u then v%u, target read by one reflection query, step checked by a concept\n",
              m1.version, m2.version);

  // Flip this to 1 to see a rejected step. Each fails the deduction guide at the
  // call site with "class template argument deduction failed ... constraints not
  // satisfied", and the message names which conjunct of MutatingStep was false.
  // It is not a tidy message: GCC 16.2 lists all three candidates and renders the
  // splices as internal noise ("'meta_type' not supported by direct_abstract_
  // declarator", "'splice_scope' not supported by simple_type_specifier"). It says
  // which rule you broke; it does not read cleanly yet.
#if 0
  Migration g{1u, [](auto&) {}};            // generic: has_one_param_call_op(^^C) -> false
  Migration c{1u, [](const Database&) {}};  // const ref: !is_const_v<...> -> false
#endif
}

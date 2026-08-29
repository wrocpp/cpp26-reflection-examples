// jelinek_reduction.cpp
//
// NOT MY CODE. This is Jakub Jelinek's reduction of the testcase in GCC
// PR127109, posted as comment 1 on 2026-08-27, 58 minutes after the bug was
// filed. Reproduced here because the reduction is the interesting part.
//
//   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=127109#c1
//
// The bug: a C++26 P0963 structured binding used as a condition, over a type
// that decomposes through the tuple protocol, is rejected during constant
// evaluation. GCC 16.2 and trunk reject it; clang accepts it.
//
// Two things this reduction does that the original report did not:
//
//   1. No headers at all. std::size_t is spelled decltype(sizeof 0), and
//      tuple_size / tuple_element are forward-declared rather than included,
//      so <tuple> and <cstddef> both disappear. tuple_size specialises with a
//      plain `static constexpr int value = 2` instead of integral_constant.
//
//   2. It uses the get()-returns-reference form. In the original report that
//      was variant 5, added to prove the failure does not depend on temporaries
//      from a by-value get(). Choosing it for the reduction keeps that property.
//
// Expected: rejected by GCC with "accessing '<anonymous>' outside its lifetime".
//
// Compile: g++ -std=c++26 jelinek_reduction.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++26 -O1

namespace std {
using size_t = decltype (sizeof 0);
template <typename> struct tuple_size;
template <size_t, typename> struct tuple_element;
}

struct A {
  int a, b;
  bool c;
  constexpr explicit operator bool () const { return c; }
  template <std::size_t I>
  constexpr const int &get () const { return I == 0 ? a : b; }
};

template <>
struct std::tuple_size <A> { static constexpr int value = 2; };
template <std::size_t I>
struct std::tuple_element <I, A> { using type = const int; };

constexpr int
foo (int v)
{
  if (auto [a, b] = A { v, v * 2, v != 0 })
    return a + b;
  return -1;
}

static_assert (foo (1) == 3);
static_assert (foo (0) == -1);

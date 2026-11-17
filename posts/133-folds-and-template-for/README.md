# Folds, pack indexing and the `template for` trap

Companion to the wro.cpp post on fold grouping, pack indexing and `template for` over a tuple of constants. It builds on a
LinkedIn post by Leonardo Rossi about fold expressions in a Qt code base. The example contains no macros.

- `examples/folds_and_template_for.cpp`: the grouping of left and right folds, pack indexing, and three ways to walk a table of
  member pointers (`template for`, an `index_sequence` lambda, a structured binding pack).
  Compiler Explorer (g162, `-std=c++26 -Wall -Wextra -Wpedantic`): https://godbolt.org/z/sEc7Mvrf1

All results were taken on Compiler Explorer on 2026-11-17. The program builds without a warning and exits 0.

## Observed output

g162, `-std=c++26 -Wall -Wextra -Wpedantic`, and again with `-freflection` added (same output, so `template for` does not need it):

```text
left=5 right=9 third=42 links_for=21 links_seq=21 links_binding=21 sum_for=12 sum_binding=12
```

`left(10, 3, 2)` is `(... - v)`, `right(10, 3, 2)` is `(v - ...)`. Both are `static_assert`ed. Neither warns.
`clang2210` (`-std=c++26`, same warning flags) accepts the folds and pack indexing but has no `template for`.

## The forms that do not compile

Each was compiled on 2026-11-17 with `-std=c++26 -Wall -Wextra -Wpedantic`.

`template for (constexpr auto v : std::tuple{1uz, 2uz, 3uz}) { sum += twice<v>; }` inside a `constexpr` function:

- g162: `error: modification of '<anonymous>' from outside current evaluation is not a constant expression`
- clang_trunk: `error: constexpr variable '[__u0, __u1, __u2]' must be initialized by a constant expression`, with
  `note: reference to temporary is not a constant expression`

The same loop over a local `constexpr std::tuple values{...};` (without `static`):

- g162: `error: 'values' is not a constant expression`, `note: address of non-static constexpr variable 'values' may differ on each invocation of the enclosing function; add 'static' to give it a constant address`
- clang_trunk: `error: constexpr variable '[__u0, __u1, __u2]' must be initialized by a constant expression`, with the same
  `address of non-static constexpr variable 'values' may differ ...` note.

Fix: `static constexpr std::tuple values{...};`, as in the example. Without `constexpr` on the loop variable
(`template for (auto v : std::tuple{...})`) the loop compiles on both and is usable in a `static_assert`.

A structured binding pack outside a template (`auto [...xs] = table();` in `main`):

- g162: `error: structured binding pack outside of template`
- clang2210 and clang_trunk: `error: pack declaration outside of template`

Inside a template it works, including `static constexpr auto [...rows] = Table::wiring;` whose elements are template arguments.
clang_trunk accepts the example's `template for` bodies; with a body that is not a compound statement it warns
(`-Wexpansion-stmt-missing-braces`), so the example uses braces.

## Reading the standard

[stmt.expand] paragraph 5.3 of the C++26 draft (https://eel.is/c++draft/stmt.expand) defines a destructuring expansion
statement through `constexpr auto&& [u0, u1, ..., uN-1] = expansion-initializer;`, where `constexpr` is present when the loop
variable is declared `constexpr`. The compilers' own notes (reference to a temporary, address of a non-static `constexpr`
variable) fit that hidden reference. The rule that says when a reference may be a constant expression was not read for this
example.

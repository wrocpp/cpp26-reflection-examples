# optional, string_view and span as template arguments: rejected today, and a structural workaround

Companion to the wro.cpp post "pusz-buzios-report-contracts-poll". Mateusz Pusz's report from the Buzios meeting
(24 November 2026) says that P3861R1, "Pragmatic approach to standard structural types", makes `std::string_view`,
`std::span`, `std::tuple`, `std::optional` and `std::variant` structural types, so they can be template arguments.
This directory shows the limit that the paper removes, on compilers that do not implement it yet, and the workaround
that compiles today.

P3861R1 was not public on 2026-11-28: the isocpp.org and open-std.org addresses for R1 returned 404, the open-std 2026
index has no P3861 entry, and `wg21.link/p3861` redirects to R0 (2025). Nothing here is checked against the R1 text.

All runs are on Compiler Explorer, 2026-11-28, x86-64, executed (not just compiled). The examples contain no macros, and
every file in `examples/` exits 0.

Compilers: `g162` is GCC 16.2.0 (`__cplusplus` 202400 at `-std=c++26`). `gsnapshot` is GCC 17.0.0 20261127
(experimental, `__cplusplus` 202603). `clang_trunk` is Clang 24.0.0git (llvm-project a08343d1ed39, `__cplusplus` 202400),
with libstdc++ unless noted.

## structural_wrappers.cpp

`g++ -std=c++26`. Compiler Explorer (g162): https://godbolt.org/z/EhYMKfTEq

Three class templates, `Bounded`, `Named` and `Lookup`, take a small struct of our own where they would like a
`std::optional<int>`, a `std::string_view` and a `std::span<const int>`. Each struct has only public, non-mutable members
of structural types, so the language already accepts it. A requires-expression then asks the compiler whether the
standard types are accepted yet; nothing asserts the answer, so a compiler that implements the paper prints `yes` and
still exits 0.

The same output on g162, gsnapshot, clang_trunk (`-std=c++26`) and clang_trunk with `-std=c++26 -stdlib=libc++`:

```text
Bounded<{}>::describe()          no limit
Bounded<{kLimit}>::describe()    limit 8
Named<"retries">::name           retries (7 characters)
Named<"retries"> twice           same type: yes
Lookup<kPrimes>::sum             17 over 4 values
Lookup over a copy of kPrimes    same type: no
accepted as a template argument today:
  std::optional<int>               no
  std::string_view                 no
  std::span<const int>             no
```

The two wrappers for a sequence use different notions of "the same argument". The text wrapper stores the characters, so
two equal texts give one type. The span wrapper stores an address and a length, so two arrays with equal contents give
two types. R0 of P3861 gives `std::string_view` and `std::span` the address rule (equal `data()` and `size()`); see
`posts/140-cpp26-buzios-fixes`. Whether R1 keeps it I could not check.

## The rejected forms

A template parameter of a type that is not structural is a hard error, so these three cannot be part of a program that
exits 0. Each is the whole source I ran, `int main() {}` after the template:

```cpp
#include <optional>
template <std::optional<int> Limit> struct Bounded {};
```

```cpp
#include <string_view>
template <std::string_view Name> struct Named {};
```

```cpp
#include <span>
template <std::span<const int> Table> struct Lookup {};
```

| type | g162 and gsnapshot (`-std=c++26`) | clang_trunk (`-std=c++26`) |
|---|---|---|
| `std::optional<int>` | `error: 'std::optional<int>' is not a valid type for a template non-type parameter because it is not structural` | `error: type 'std::optional<int>' of non-type template parameter is not a structural type` |
| `std::string_view` | `error: 'std::basic_string_view<char>' is not a valid type for a template non-type parameter because it is not structural` | `error: type 'std::string_view' (aka 'basic_string_view<char>') of non-type template parameter is not a structural type` |
| `std::span<const int>` | `error: 'std::span<const int>' is not a valid type for a template non-type parameter because it is not structural` | `error: type 'std::span<const int>' of non-type template parameter is not a structural type` |

No compiler in this list accepted any of the three on 2026-11-28, including the GCC trunk build of 2026-11-27.

# C++26 changes adopted at Buzios, beside the contracts label: runnable limits and a reflection rule

Companion to the wro.cpp post "buzios-cpp26-fixes". The meeting adopted four main papers for C++26 (per Sutter's trip report). This directory covers what
runs today for three of them, and one wording motion:

- P3861 "Pragmatic approach to standard structural types" (draft repository issue 9401). Revision R1 was not public on 2026-11-21 (the
  isocpp.org and wg21.link addresses return 404). The runnable material is the limit that it targets and the stopgaps that exist today.
- P4466R2 "Selectively transparent using-declarators" (issue 9398). The same reflection line with `#include` and with `import std;`.
- P4453R1 "Memory allocation with senders/receivers" (issue 9400). Nothing runs: the new query is in no implementation checked.
- Core issues 3241 and 3242 (issue 9397): wording only, no example.

All runs are on Compiler Explorer, 2026-11-21, x86-64, executed (not just compiled). The examples contain no macros, and every
file exits 0 on the build listed for it.

Compilers: `g162` is GCC 16.2.0 (`__cplusplus` 202400 at `-std=c++26`). `gsnapshot` is GCC 17.0.0 20261121 (experimental).
`gcontracts-p4324-trunk` is the Compiler Explorer entry "x86-64 gcc (P4324 contracts, and profiles)", GCC 17.0.0 20261102 (experimental).
It is the compiler of the two Compiler Explorer links in the P4466R2 text, which credits Ville Voutilainen with an implementation of
its rule in a fork of GCC. I did not check what else that fork changes. `clang_bb_p2996` is the Bloomberg reflection fork of Clang with
`-std=c++26 -freflection-latest -stdlib=libc++`.

## structural_today.cpp

`g++ -std=c++26`. Compiler Explorer (g162): https://godbolt.org/z/8z4h7n7G3

The same output on g162, gsnapshot, clang2310, clang_trunk, clang_bb_p2996 (with its options above) and MSVC
`vcpp_v19_51_VS18_6_x64` (`/std:c++latest /EHsc /Zc:__cplusplus`):

```text
int                      yes
const char*              yes
plain_text               yes
std::string_view         no
std::span<const int>     no
std::tuple<int, char>    no
std::optional<int>       no
std::variant<int>        no
ByAddress<first> and ByAddress<second> are the same type: no
```

`first` and `second` are two arrays that hold the same text "Hello". A pointer argument is equal only when it points at the same
object, which is the notion of equivalence that R0 of P3861 gives `std::string_view` and `std::span`: equal `data()` and `size()`,
not equal contents. The paper has the same pair (its `Char<a>` and `Char<b>`), with `String<a>` and `String<b>` as the rejected
`string_view` version.

The direct form is rejected by every compiler. Source:

```cpp
#include <string_view>
template <std::string_view S> struct X {};
int main() {}
```

| compiler | error |
|---|---|
| g162, gsnapshot (`-std=c++26`) | `error: 'std::basic_string_view<char>' is not a valid type for a template non-type parameter because it is not structural` |
| clang2310, clang_trunk (`-std=c++26`), clang_bb_p2996 | `error: type 'std::string_view' (aka 'basic_string_view<char>') of non-type template parameter is not a structural type` |
| MSVC `vcpp_v19_51_VS18_6_x64` | `error C2993: 'std::basic_string_view<char,std::char_traits<char>>': is not a valid type for non-type template parameter 'S'`, with the note `'_Mydata' is not a public, non-mutable, non-static data member` |

`std::span<const int>` and `std::tuple<int, char>` in the same position get the same error on g162 and clang2310.

## static_string_argument.cpp

`g++ -std=c++26 -freflection`. Compiler Explorer (g162): https://godbolt.org/z/59h47bxnb

```text
by value, "abc" twice: same type yes, reads back "abc"
by value, "abc" and "abd": same type no
by address, define_static_string twice: same pointer yes, same type yes, reads back "abc"
by address, "abc" and "abd": same type no
```

The same output on g162, gsnapshot and clang_bb_p2996. `std::define_static_string` returns `const char*`. Two calls with the same
text returned the same pointer in one program, and the pointer was accepted as a `const char*` template argument. Clang trunk is
not in the matrix, and I did not run it. What a `std::string_view` built from such a pointer does as an
argument is not shown, because no compiler checked implements P3861.

## reflect_through_include.cpp

`g++ -std=c++26 -freflection`. Compiler Explorer (g162): https://godbolt.org/z/zTos79n66

```text
basic_string in __cxx11
vector in std
```

The same on g162, gsnapshot and gcontracts-p4324-trunk. With libc++ (clang_bb_p2996) the output is `basic_string in __1` and
`vector in __1`.

## inline_namespace_using.cpp

`g++ -std=c++26 -freflection`. `// verify: ce-only`. Compiler Explorer (gcontracts-p4324-trunk): https://godbolt.org/z/d4e5e138s

```text
f lives in Inline
```

| compiler | result |
|---|---|
| gcontracts-p4324-trunk | exit 0, the output above |
| clang_bb_p2996 | exit 0, the same output |
| g162 and gsnapshot | does not compile: `error: '^^' cannot be applied to a using-declaration` at `^^NS::f` |

## reflect_through_module.cpp

`g++ -std=c++26 -freflection -fmodules -fsearch-include-path bits/std.cc`. `// verify: ce-only`.
Compiler Explorer (gcontracts-p4324-trunk): https://godbolt.org/z/PjrK851ME

```text
basic_string in __cxx11
vector in std
```

| compiler | result |
|---|---|
| gcontracts-p4324-trunk | exit 0, the output above |
| g162 and gsnapshot | does not compile: `error: '^^' cannot be applied to a using-declaration` at the `^^std::basic_string` line; `^^std::vector` is accepted |

The compile gate of the Compiler Explorer API needs `"binary": true` for this build line; the web pages do it on their own. The Clang
module form was not run.

## Cases that stay ill-formed under P4466R2 (scratch files, not examples)

A reflection that names a using-declarator is a hard error, so these cannot be in a program that exits 0. Each program below is the one I ran, with the
lines compacted, and was run with `-std=c++26 -freflection` on the GCC compilers and the Clang options above on clang_bb_p2996:

```cpp
// named namespace
#include <meta>
namespace N4 { void g(); }
using N4::g;
constexpr std::meta::info r = ^^g;
int main() {}
```

```cpp
// class member
#include <meta>
struct Base { void f(); };
struct Derived : Base { using Base::f; };
constexpr std::meta::info r = ^^Derived::f;
int main() {}
```

```cpp
// enumerator
#include <meta>
namespace NS { enum Enum1 { E1 }; using Enum1::E1; }
constexpr std::meta::info r = ^^NS::E1;
int main() {}
```

| case | what P4466R2 says | g162 | gsnapshot | gcontracts-p4324-trunk | clang_bb_p2996 |
|---|---|---|---|---|---|
| `namespace NS { void f(); using NS::f; }`, `^^NS::f` | allowed | compiles | compiles | compiles | compiles |
| inline namespace between the using-declarator and the function | allowed | error | error | compiles | compiles |
| `using N4::g;` across a named namespace, `^^g` | stays ill-formed | compiles | compiles | error | error |
| `using Base::f;` in a class, `^^Derived::f` | stays ill-formed | compiles | compiles | error | error |
| `using Enum1::E1;`, `^^NS::E1` | stays ill-formed | compiles | compiles | compiles | compiles |

The errors are `'^^' cannot be applied to a using-declaration` (GCC fork) and `cannot take the reflection of a using-declarator`
(clang_bb_p2996). Where g162 and gsnapshot compile the named-namespace and class cases, a further run printed that the reflection is
of the function in `N4` and of `Base::f`: they look through the using-declarator. I did not look for the rule GCC applies.

## Sources checked for P4453R1 (no example)

- The paper: https://isocpp.org/files/papers/P4453R1.html, dated 2026-11-18. It adds the query `get_scheduler_allocator`.
- NVIDIA stdexec, main at f27cd13 (committed 2026-11-21 16:58 UTC): the pattern `scheduler_allocator` appears nowhere in the tree.
  Pull request 2462, "partial implementation of P4453", was merged on 2026-11-17. It makes the task try to build its start scheduler
  from the start scheduler and an allocator, then from the start scheduler, then from the task's allocator, then by default
  construction. The code comments call the first and third steps non-standard but proposed by P4453.
- bemanproject/execution, main at 3489ed3 (committed 2026-11-20): the same pattern appears nowhere.

## Sources checked for the wording motion

- Core issue 3241, https://cplusplus.github.io/CWG/issues/3241.html, list revision 121b: a note in [basic.stc.static] is rephrased.
- Core issue 3242, https://cplusplus.github.io/CWG/issues/3242.html: a sentence in [expr.assign] is rephrased so it avoids "shall".

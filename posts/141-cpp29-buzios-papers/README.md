# C++29 papers adopted at Buzios: what runs today

Companion to the wro.cpp post "buzios-cpp29-papers". The ISO C++ meeting in Buzios (16 to 21 November 2026) adopted
14 motions for the C++29 draft: 11 papers and 3 issue lists. These examples cover the seven papers whose code I tried:
P3568R4 (`break label;`), P0085R5 (`0o` octal prefix), P3216R6 (`views::slice`), P3411R7 (`any_view`), P4189R0 (`get_ptr()`),
P3642R8 (`std::clmul`) and P2952R5 (deduced return types for defaulted operators). The examples contain no macros.

All runs were taken on Compiler Explorer on 2026-11-21. Compiler ids: `g162` GCC 16.2, `gsnapshot` GCC trunk (`__cplusplus`
is 202603 at `-std=c++26` and 202700 at `-std=c++2d`), `clang2310` Clang 23.1, `clang_trunk`, `cg162`, `cgsnapshot`,
`cclang2310` and `cclang_trunk` for C, and MSVC `vcpp_v19_51_VS18_6_x64` with `/std:c++latest /EHsc /Zc:__cplusplus`.

| file | build (Compiler Explorer id) | link |
|---|---|---|
| `examples/break_label.c` | `-std=c2y -O1` (`cg162`; also `cgsnapshot`, `cclang2310`, `cclang_trunk`) | https://godbolt.org/z/Gb11Ton9M |
| `examples/break_label_clang.cpp` | `-std=c++26 -Xclang -fnamed-loops -O1` (`clang2310`; also `clang_trunk`) | https://godbolt.org/z/86qsdbW7r |
| `examples/break_label_today.cpp` | `-std=c++26 -O1` (`g162`) | https://godbolt.org/z/dTvbcf6cP |
| `examples/octal_trap.cpp` | `-std=c++26 -O1 -Wall -Wextra -pedantic` (`g162`) | https://godbolt.org/z/9GYe9MTPv |
| `examples/octal_prefix_clang.cpp` | `-std=c++26 -O1` (`clang2310`; also `clang_trunk`) | https://godbolt.org/z/e4x7df9jY |
| `examples/slice_today.cpp` | `-std=c++26 -O1` (`g162`) | https://godbolt.org/z/a95KvraaG |
| `examples/any_view_sketch.cpp` | `-std=c++26 -O1` (`g162`) | https://godbolt.org/z/7fhEcjhYe |
| `examples/optional_get_ptr_today.cpp` | `-std=c++26 -O1` (`g162`) | https://godbolt.org/z/96GEfae3s |
| `examples/clmul_today.cpp` | `-std=c++26 -O1` (`g162`) | https://godbolt.org/z/M9z7Yaa79 |
| `examples/clmul_gcc_trunk.cpp` | `-std=c++2d -O1` (`gsnapshot`) | https://godbolt.org/z/3fq7ro59b |
| `examples/defaulted_auto_today.cpp` | `-std=c++26 -O1` (`g162`) | https://godbolt.org/z/7Wcb5G7nn |

Every file builds and exits 0 with its build line. Each link was checked byte for byte against the file in this directory
through `https://godbolt.org/api/shortlinkinfo/<id>`. The files tagged `// verify: ce-only` need a compiler or a flag that the
repository's container does not have: the C file, the Clang-only files and the GCC trunk file. The paper's own examples
are marked "from the paper, not compiled" in the post when no compiler accepts them.

## P3568R4: `break label;` and `continue label;`

`break_label.c` is C, because C2y already has the syntax (N3355) and the C compilers accept it without a flag:

```text
found 5 at row 1 col 1 after visiting 2 rows
rows without 5: 2
```

The same output comes from `cg162`, `cgsnapshot`, `cclang2310` and `cclang_trunk` with `-std=c2y -O1`.

`break_label_clang.cpp` is the same program in C++. Clang 23.1 and Clang trunk accept `break outer;` and `continue outer;` in
C++ mode behind `-Xclang -fnamed-loops`, the flag that section 6 of the paper names, also in constant evaluation. The output
is the same two lines. A second `outer:` in one function, which the adopted paper allows, is rejected with
"redefinition of label 'outer'", so the flag is a partial implementation. A label that names no enclosing loop is rejected with
"'break' label does not name an enclosing loop or 'switch'".

Without the flag, C++ mode fails on every compiler I tried (`-std=c++26`, and `-std=c++2d` where the option exists):

| compiler | message |
|---|---|
| `g162`, `gsnapshot` | `error: expected ';' before 'outer'` |
| `clang2310`, `clang_trunk` | `error: named 'break' is only supported in C2y` |
| MSVC | `error C2146: syntax error: missing ';' before identifier 'outer'` |

`break_label_today.cpp` shows what a reader writes today, three ways: a goto to the end of the outer body, an immediately invoked
lambda and a flag. All three print `goto: 2, lambda: 2, flag: 2` on `g162`, `gsnapshot`, `clang2310` and `clang_trunk`.
The lambda and the flag work in a `static_assert`. A goto does not: a function with a goto fails `static_assert` on
GCC 16.2 and GCC trunk with `error: 'goto' is not a constant expression`, and on Clang 23.1 with
`subexpression not valid in a constant expression`.

## P0085R5: the `0o` octal prefix

`octal_trap.cpp` is the padding example of the paper, a table written as `{100, 042}` and `{107, 000}`:

```text
100 34
107 0
```

The build has no diagnostic on `g162` and `gsnapshot` with `-Wall -Wextra -pedantic`, and none on `clang2310` and `clang_trunk` with
`-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic`. A smaller file with `017`, `042` and `000` also builds without a
diagnostic on `g162` with `-Wall -Wextra -pedantic -Wdeprecated -Wcast-qual -Wconversion` and on `gsnapshot` at `-std=c++2d`
with `-Wall -Wextra -pedantic -Wdeprecated`.

In C mode it is different. With `-std=c2y -Wall -Wextra -pedantic`, Clang 23.1 and Clang trunk warn on all three literals:
`octal literals without a '0o' prefix are deprecated [-Wdeprecated-octal-literals]`. GCC 16.2 and GCC trunk in C mode do not.
All four C compilers accept `0o17` and `0O17` at `-std=c2y` and print `15 15`.

`octal_prefix_clang.cpp` prints:

```text
0o17 = 15, 0O17 = 15, 017 = 15
0o755 = 493
```

`clang2310` and `clang_trunk` accept `0o17` and `0O17` at `-std=c++26` and `-std=c++2d` with no flag. With `-pedantic` they warn
`octal integer literals are a Clang extension [-Woctal-prefix-extension]`, and with `-pedantic-errors` they stop. GCC 16.2 and GCC
trunk reject it with `error: unable to find numeric literal operator 'operator""o17'`, and MSVC with
`error C3688: invalid literal suffix 'o17'`.

## P3216R6: `views::slice`

`slice_today.cpp` builds the adaptor of the paper's wording, `drop(F)` then `take(G - F)`, for index arguments and runs the
paper's examples:

```text
world
50 51 52 53 54 55 56 57 58 59 
all slices as in the paper: yes
```

`std::views::slice` does not exist in any library build I tried. The probe is two lines:

```cpp
#include <ranges>
int main() { auto adaptor = std::views::slice; (void)adaptor; return 0; }
```

| compiler and flags | message |
|---|---|
| `g162 -std=c++26`; `gsnapshot -std=c++26` and `-std=c++2d` | `'slice' is not a member of 'std::views'; did you mean 'slide'?` |
| `clang2310`, `clang_trunk` with `-stdlib=libc++`, `-std=c++26` and `-std=c++2d` | `no member named 'slice' in namespace 'std::ranges::views'` |
| MSVC `/std:c++latest` | `error C2039: 'slice': is not a member of 'std::ranges::views'` |

## P3411R7: `any_view`

`any_view_sketch.cpp` is a 35-line class, `any_input_view<T>`, that has the defaults of `any_view<Widget>` in the paper: an input
range, move-only, not const-iterable and never a common range. Each of those is a `static_assert`. It is not an implementation of
the paper: it has no options, no size and no allocator support. It builds on `g162`, `gsnapshot` and `clang2310` and prints
`in stock: bolt nut `.

`std::ranges::any_view` does not exist in any library build I tried. The probe is `std::ranges::any_view<int> v;` after
`#include <ranges>`.

| compiler and flags | message |
|---|---|
| `g162 -std=c++26`; `gsnapshot -std=c++26` and `-std=c++2d` | `'any_view' is not a member of 'std::ranges'` |
| `clang2310`, `clang_trunk` with `-stdlib=libc++`, `-std=c++26` and `-std=c++2d` | `no member named 'any_view' in namespace 'std::ranges'` |
| MSVC `/std:c++latest` | `error C2039: 'any_view': is not a member of 'std::ranges'` |

## P4189R0: `get_ptr()`

I opened R0, dated 2026-05-12, the only revision I could find. It proposes a `get_ptr()` member for `optional<T>` and
`optional<T&>` with the effect `return has_value() ? std::addressof(val) : nullptr;`. The name the committee adopted is unconfirmed:
Sutter's trip report says `.get_ptr()`, and the issue title is "get()ing the pointer from optional".
`optional_get_ptr_today.cpp` writes that effect as free functions and prints:

```text
optional<int>: 7, empty: -1
optional<int&>: 7, same address as the one-liner: yes
```

The one-liner is the paper's own spelling of today's equivalent for `optional<T&>`. No member `get_ptr` exists on `optional<int>`
or `optional<int&>` in any library build I tried (the probe is `o.get_ptr() == nullptr`):

| compiler and flags | message |
|---|---|
| `g162 -std=c++26`; `gsnapshot -std=c++26` and `-std=c++2d` | `'class std::optional<int>' has no member named 'get_ptr'` |
| `clang2310`, `clang_trunk` with `-stdlib=libc++`, `-std=c++26` and `-std=c++2d` | `no member named 'get_ptr' in 'std::optional<int>'` |
| MSVC `/std:c++latest` | `error C2039: 'get_ptr': is not a member of 'std::optional<int>'` |

MSVC's `<optional>` has no `optional<T&>` at all on 2026-11-21 (`error C2625: ... illegal union member; type 'int &' is reference type`).

## P3642R8: `std::clmul`

This is the one paper whose library part is already in a standard library. libstdc++ on GCC trunk provides `std::clmul` and
`std::widening_clmul`, with `low()` and `high()` on the result, but only in the C++29 mode: `gsnapshot` with `-std=c++2d` or
`-std=c++29`. `clmul_gcc_trunk.cpp` prints:

```text
__cpp_lib_clmul = 202610 (the library's value)
widening_clmul(0x80000001, 0x80000001): low 00000001, high 40000000
clmul equals the loop for 65536 inputs: yes; parity equals popcount parity: yes
```

The number is the value libstdc++ defines. The paper shows a placeholder for the macro, so it is not the paper's value.
The functions are missing at `gsnapshot -std=c++26` (`'clmul' is not a member of 'std'`), on `g162` (same message),
on `clang2310` and `clang_trunk` with `-stdlib=libc++` at `-std=c++26` and `-std=c++2d` (`no member named 'clmul' in namespace 'std'`),
and on MSVC (`error C2039: 'clmul': is not a member of 'std'`). Not tried: Clang with libstdc++ at `-std=c++2d`.

`clmul_today.cpp` is the paper's naive loop for any unsigned type, with two of its uses, and runs everywhere:

```text
clmul(3, 3) = 5
parity from clmul equals popcount parity for all 16-bit values: yes
clmul(x, x) spreads the bits of x for all 16-bit values: yes
```

## P2952R5: deduced return types for defaulted operators

The paper's section 4 says "Implementation experience: None yet." The two forms it makes well-formed are rejected by every
compiler I tried: `g162`, `gsnapshot` (`-std=c++26` and `-std=c++2d`), `clang2310`, `clang_trunk` (`-std=c++26` and `-std=c++2d`) and MSVC.
The probes are one class each:

```cpp
struct X { int i; auto& operator=(const X&) = default; };
struct X { int i; auto operator==(const X&) const = default; };
```

| compiler | `auto& operator=(const X&) = default;` | `auto operator==(const X&) const = default;` |
|---|---|---|
| GCC 16.2, GCC trunk | `defaulted declaration 'auto& X::operator=(const X&)' does not match the expected signature` | `defaulted 'auto X::operator==(const X&) const' must return 'bool'` |
| Clang 23.1, Clang trunk | `explicitly-defaulted copy assignment operator must return 'X &'` | `return type for defaulted equality comparison operator must be 'bool', not 'auto'` |
| MSVC | `error C2610: ... the return type does not match the expected signature` | `error C2610: ... is not a special member function or comparison operator which can be defaulted` |

`defaulted_auto_today.cpp` shows the one case that works today, `auto operator<=>`, and that the deduced category follows the
members (`std::strong_ordering` for two ints, `std::partial_ordering` for a double). It prints
`Version 1.2 < 1.3: yes` and `Reading with NaN is unordered against 1.0: yes`.

# [[nodiscard]] after a keyword: MSVC drops it without a word at /W4

Companion to the wro.cpp short "MSVC ignores a [[nodiscard]] written after static, and /W4 says nothing" (2 December 2026).
It follows Giovanni Dicanio's post
[Where to Put [[nodiscard]]: A Silent Trap with MSVC!](https://giodicanio.com/2026/10/12/where-to-put-nodiscard-a-silent-trap-with-msvc/)
(12 October 2026) and the keyword list in Jiri Havel's comment on it (19 November 2026). The examples contain no macros.

- `examples/nodiscard_position.cpp`: the correct placement, `[[nodiscard]]` before every keyword, for seven declaration forms.
  It builds clean with a discarded result made an error and exits 0.
  Compiler Explorer (g162, `-std=c++26 -Wall -Wextra -Werror=unused-result`): https://godbolt.org/z/r3GrrT17G
- `misplaced/static_first.cpp`: Dicanio's case, `static [[nodiscard]] int f()`, with the result discarded.
  Compiler Explorer (vcpp_v19_51_VS18_6_x64, `/std:c++20 /W4`, compiles and runs with no diagnostic): https://godbolt.org/z/5GG1fYzxo
- `misplaced/standard_keywords.cpp`: the attribute after `static`, `inline`, `constexpr`, `virtual`, `friend` and `explicit`.
- `misplaced/msvc_keywords.cpp`: the attribute after `__forceinline` and `__declspec(noinline)` (MSVC only).

The files in `misplaced/` are expected NOT to compile on GCC or Clang. They sit outside `examples/`, so `verify.sh` and the
link pipeline do not pick them up. Their outputs are quoted below. All runs were made on Compiler Explorer on 2026-11-28.

## Compilers

| id | `_MSC_FULL_VER` or version |
|---|---|
| vcpp_v19_30_VS17_0_x64 | 193030715 |
| vcpp_v19_44_VS17_14_x64 | 194435219 |
| vcpp_v19_50_VS18_0_x64 | 195035717 |
| vcpp_v19_51_VS18_6_x64 and vcpp_v19_latest_x64 | 195136246 (the same build) |
| g162, gsnapshot | GCC 16.2, GCC trunk |
| clang2210, clang_trunk | Clang 22.1, Clang trunk |

MSVC options: `/std:c++20` plus the warning options in each row. GCC and Clang options: `-std=c++26 -Wall -Wextra`.

## Results

A discarded call to a function whose `[[nodiscard]]` follows a keyword:

| case | MSVC 19.30 to 19.51, `/W4` | MSVC, `/Wall` or `/w45240` | MSVC, `/we5240` | GCC 16.2 and trunk | Clang 22.1 and trunk |
|---|---|---|---|---|---|
| `static [[nodiscard]]` | silent, no C4834 | warning C5240 | error C5240 | error | error |
| `inline [[nodiscard]]` | silent | warning C5240 | error C5240 | error | error |
| `constexpr [[nodiscard]]` | silent | warning C5240 | error C5240 | error | error |
| `virtual [[nodiscard]]` (member) | silent | warning C5240 | error C5240 | error | error |
| `static [[nodiscard]]` (member) | silent | warning C5240 | error C5240 | error | error |
| `friend [[nodiscard]]` | silent | warning C5240 | error C5240 | error | error |
| `explicit [[nodiscard]]` (constructor) | silent | warning C5240 | error C5240 | warning `-Wattributes`, exit 0 | error (different text) |
| `__forceinline [[nodiscard]]` | silent | warning C5240 | error C5240 | n/a | n/a |
| `__declspec(noinline) [[nodiscard]]` | silent | warning C5240 | error C5240 | n/a | n/a |

"Silent" means exit 0 and no diagnostic at all: neither C5240 for the attribute nor C4834 for the discarded call.
Each case is a one-declaration probe with one discarded call. The `/W4` column was run on vcpp_v19_30, v19_44, v19_50,
v19_51 and latest. `/we5240` was run on v19_30 and v19_51, `/Wall` on v19_51 for every case, and `/w45240` on v19_30,
v19_44 and latest for the `static` case. GCC and Clang were run on g162, gsnapshot, clang2210 and clang_trunk.

Compared with the source: Dicanio writes in a comment (19 November) that VS 2026 at warning level 4 now emits C5240,
quoted as `'[[nodiscard]]': attribute is ignored in this syntactic position`. The two VS 2026 compilers on Compiler Explorer
(19.50 and 19.51) stay silent at `/W4` and print the text with `'nodiscard'`, so the author's build differs from these.
A newer MSVC than 19.51 was not available to run.

When `/we5240` was first available: vcpp_v19_25_VS16_5_x64 (`/std:c++latest /W4 /we5240`) compiles `static_first.cpp`
with no diagnostic; vcpp_v19_28_VS16_9_x64 and vcpp_v19_29_VS16_11_x64 report error C5240. 19.26 and 19.27 were not run.

With the attribute first, every compiler reports the discarded call (`[[nodiscard]] static int f()`, `[[nodiscard]] virtual`,
`[[nodiscard]] explicit Guard(int)`).

## Quoted outputs

MSVC, `/std:c++20 /W4 /we5240`, `misplaced/static_first.cpp` (the same text on v19_30 and latest; `/Wall` prints it as a warning):

```text
<source>(9): error C5240: 'nodiscard': attribute is ignored in this syntactic position
```

GCC 16.2, `misplaced/static_first.cpp`:

```text
<source>:9:8: error: standard attributes in middle of decl-specifiers
<source>:9:8: warning: attribute ignored [-Wattributes]
```

Clang trunk, `misplaced/static_first.cpp` (also with `-fms-extensions -fms-compatibility`):

```text
<source>:9:8: error: an attribute list cannot appear here
```

Clang trunk, the `explicit` constructor in `misplaced/standard_keywords.cpp`:

```text
<source>:24:16: error: 'nodiscard' attribute cannot be applied to types
```

GCC 16.2 accepts that constructor with `warning: attribute ignored [-Wattributes]` and exit 0 (a one-declaration probe).

With the attribute first, the discarded call (a one-line probe, `[[nodiscard]] static int answer()`):

```text
vcpp_v19_30: warning C4834: discarding return value of function with 'nodiscard' attribute
vcpp_v19_44 and later: warning C4834: discarding return value of function with [[nodiscard]] attribute
g162: warning: ignoring return value of 'int answer()', declared with attribute 'nodiscard' [-Wunused-result]
clang_trunk: warning: ignoring return value of function declared with 'nodiscard' attribute [-Wunused-result]
```

## examples/nodiscard_position.cpp

Output on g162, gsnapshot and clang_trunk (`-std=c++26 -Wall -Wextra -Werror=unused-result`) and on vcpp_v19_latest_x64
(`/std:c++latest /W4 /we4834 /we5240`), all exit 0:

```text
[[nodiscard]] static:            7
[[nodiscard]] inline:            7
[[nodiscard]] constexpr:         7
[[nodiscard]] static member:     11
[[nodiscard]] virtual:           13
[[nodiscard]] friend:            17
[[nodiscard]] explicit ctor:     19
```

## A grep that finds the misplaced form

A heuristic, not a parser. It matched all nine misplaced declarations of the probes and none of the correct ones:

```sh
grep -nE '\b(static|inline|constexpr|virtual|friend|explicit|__forceinline|__declspec\([^)]*\))[[:space:]]+\[\[' -r src/
```

It also matches other attributes in the same position, and it misses a declaration split across lines.

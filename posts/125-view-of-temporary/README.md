# A view built from a temporary owner: what bugprone-dangling-handle, Clang 23.1 and GCC 16.2 report

Companion to episode 4 of the wro.cpp series "clang-tidy checks for the mistakes AI coding agents keep making". The
subject is `view-of-temporary` of https://github.com/wrocpp/tidy-agent, which is a configuration of the existing clang-tidy
check `bugprone-dangling-handle` and not a new check. The examples contain no macros.

- `examples/cases.cpp`: thirteen cases that dangle and four that do not. Run without an argument it runs the four safe cases,
  prints one line and exits 0. `./cases N` runs dangling case N once, for a sanitizer.
  Compiler Explorer (g162, `-std=c++23 -O2 -Wall -Wextra -Wdangling-pointer=2 -Wdangling-reference`): https://godbolt.org/z/KrMbdjqfr
  The same source with clang-tidy trunk and the configuration below (clang_trunk `-std=c++23`, tool `clangtidytrunk`):
  https://godbolt.org/z/fGv7853da
- `examples/guarded_slice.cpp`: the project view with an rvalue overload deleted. The temporary is refused at compile time.
  Compiler Explorer (g162, `-std=c++23 -O1`): https://godbolt.org/z/7P9zxjjqj

Runs on 2026-11-17. Both programs exit 0.

## The configuration

From `checks/config/view-of-temporary.yaml` of tidy-agent at commit 7c64b6c:

```yaml
Checks: "-*,bugprone-dangling-handle"
CheckOptions:
  bugprone-dangling-handle.HandleClasses: "std::basic_string_view;std::experimental::basic_string_view;std::span;Slice"
```

`Slice` is the project's own view class. `Other` in the example is the same shape and is deliberately not listed.

## The cases

| case | source | what dangles |
|---|---|---|
| 1 | `Slice s{std::string{kText}};` | listed project view over a prvalue |
| 2 | `Slice s(make());` | listed project view over a function result |
| 3 | `std::string_view v = make();` | |
| 4 | `std::string_view v = make() + "!";` | |
| 5 | `std::span<const char> v = make_chars();` | |
| 6 | `std::string_view v{make()};` | braced direct initialisation |
| 7 | `Other o{make()};` | view class not in `HandleClasses` |
| 8 | `auto v = first_word(make());` | function returning a view of its argument |
| 9 | `!named.empty() ? std::string_view{make()} : std::string_view{named}` | the temporary is one branch |
| 10 | `std::string_view v = pass(make());` | function taking and returning a view |
| 11 | `std::string_view v = make().substr(0, n);` | `substr` of a `std::string` returns a `std::string` |
| 12 | `const char* p = make().c_str();` | a raw pointer, not a view class |
| 13 | `std::string_view returns_view() { return make(); }` | returned from a function |

Safe cases: a slice of a named string, a view of a string literal, `make().size()`, and `consume(Slice{make()})` where the
temporary lives for the whole call.

## Every dangling case is real

Homebrew clang 23.1.5, `-std=c++23 -O0 -g -fsanitize=address`, `ASAN_OPTIONS=detect_leaks=0`, then `./cases N`:
cases 1 to 13 each report `AddressSanitizer: heap-use-after-free`. `./cases` with no argument prints
`4 safe cases ran` and exits 0 under the same build.

## What each tool reported

Local runs: Homebrew LLVM 23.1.5 (`clang-tidy`, `clang++`) and Homebrew GCC 16.2.0 (`g++-16`), `-std=c++23`.
`clang-tidy --config-file=<the yaml above> cases.cpp -- -std=c++23`; `clang++ -fsyntax-only -Wall -Wextra` and the same
with `-Wlifetime-safety`; `g++-16 -O2 -c -Wall -Wextra -Wdangling-pointer=2 -Wdangling-reference` (also at `-O0`).

| case | bugprone-dangling-handle | Clang 23.1 without the flag | Clang 23.1 with -Wlifetime-safety | GCC 16.2 |
|---|---|---|---|---|
| 1 | flagged | silent | silent | silent |
| 2 | flagged | silent | silent | silent |
| 3 | flagged | `-Wdangling-gsl` | adds `-Wlifetime-safety-use-after-scope` | silent |
| 4 | flagged | `-Wdangling-gsl` | adds `-Wlifetime-safety-use-after-scope` | silent |
| 5 | flagged | `-Wdangling-gsl` | adds `-Wlifetime-safety-use-after-scope-moved` | silent |
| 6 | silent | silent | `-Wlifetime-safety-use-after-scope` | silent |
| 7 | silent | silent | silent | silent |
| 8 | silent | silent | silent | silent |
| 9 | silent | `-Wdangling-gsl` | adds `-Wlifetime-safety-use-after-scope` | silent |
| 10 | silent | silent | silent | silent |
| 11 | flagged | `-Wdangling-gsl` | adds `-Wlifetime-safety-use-after-scope` | silent |
| 12 | silent | `-Wdangling-gsl` | adds `-Wlifetime-safety-use-after-scope` | silent |
| 13 | flagged | `-Wreturn-stack-address` | adds `-Wlifetime-safety-return-stack-addr` | silent |

Totals: bugprone-dangling-handle 7 of 13, Clang without the flag 7, Clang with the flag 8, GCC 0. The union of
bugprone-dangling-handle and Clang with the flag is 10; cases 7, 8 and 10 are reported by none.
None of the four safe cases is reported by any tool.

Compiler Explorer, clang-tidy trunk (LLVM 24.0git), same source and configuration: the same seven lines
(51, 55, 56, 57, 58, 59, 68), each `outlives its value [bugprone-dangling-handle]`.
Compiler Explorer, g162 with the options above: no warning, and the program prints `4 safe cases ran`.

Output of the check on the first two cases (local, clang-tidy 23.1.5):

```text
cases.cpp:55:40: warning: Slice outlives its value [bugprone-dangling-handle]
cases.cpp:56:40: warning: Slice outlives its value [bugprone-dangling-handle]
```

## Shapes outside the file

A scratch file, not kept here, with five more safe shapes (a lifetime-extended `const std::string&`, a function returning
`const std::string&`, a temporary passed to a `std::string_view` parameter, an rvalue-reference variable, a view of a view): none was reported by bugprone-dangling-handle or by Clang with the flag. The same file had two
dangling shapes, `names().front()` and `make_holder().name()` where `name()` returns a reference into the holder. Only
`names().front()` was reported, by Clang (`-Wdangling-gsl` and `-Wlifetime-safety-use-after-scope`).
bugprone-dangling-handle reported neither. A `std::span<const int>{make_ints()}` was reported by bugprone-dangling-handle and by Clang.

## The guard

`guarded_slice.cpp` states with `static_assert` that `LooseSlice` is constructible from a `std::string` rvalue and
`GuardedSlice` is not, and that a string literal, which builds a temporary string, is refused by `GuardedSlice` as well.
Observed (g162, `-std=c++23 -O1`): `size 51, first 'a'`.
The idea of deleting the rvalue overload comes from `cloudevents-sdk-cpp` commit c4b4311, whose message says each view
class "deletes an overload for an rvalue std::string, so a view of a temporary fails to compile rather than dangle, which gcc 16
does not warn about".

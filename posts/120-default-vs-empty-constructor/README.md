# = default against {} constructors: what each form zeroes

Companion to the wro.cpp short "Choose `= default` or `{}` for an empty constructor". Daniel Lemire's post
[In C++, = default and {} are not the same constructor](https://lemire.me/blog/2026/11/02/default-and-braces-are-not-the-same-constructor/)
(2 November 2026) measures the speed difference. These examples check the semantics only: which initialisation forms zero
an object, what an empty constructor does to the type traits, and what a read of an unset value does. They contain no macros
and both exit 0 by default.

- `examples/zeroing_by_form.cpp`: zeroing and traits, decided in constant expressions and checked with `static_assert`.
  Compiler Explorer (g162, `-std=c++26 -O1 -Wall -Wno-deprecated-declarations`): https://godbolt.org/z/4PcM4bG9j
- `examples/indeterminate_read.cpp`: reads an unset length. The first argument picks the mode, so a run without arguments
  reads nothing. Not linked on Compiler Explorer, because a link cannot carry the argument and the reading modes either
  print compiler-dependent numbers or fail under the sanitizer.

All runs were taken on Compiler Explorer on 2026-11-17.

## zeroing_by_form.cpp

Three classes hold a start pointer, a length and an end pointer: `Defaulted` (`Defaulted() = default;` in the class),
`Braced` (`Braced() {}`) and `OutOfLine` (declared in the class, `= default` on the definition outside). `WithInitialiser`
is `int length; int extra = 7;`, `Plain` is `int length; int extra;` and `Initialised` gives all three members an initialiser. "Zeroed" means that `T().length == 0` or
`T{}.length == 0` is a constant expression. A form that leaves the length indeterminate cannot pass, so no indeterminate value
is read.

```text
Defaulted        trivial 1 | T t;: left alone | T(): zeroed     | T{}: zeroed
Braced           trivial 0 | T t;: left alone | T(): left alone | T{}: left alone
OutOfLine        trivial 0 | T t;: left alone | T(): left alone | T{}: left alone
Plain            trivial 1 | T t;: left alone | T(): zeroed     | T{}: zeroed
std::span<int> view{}: data() is null, size() is 0
make_unique<Defaulted>() value-initialises: length 0, start null
Plain{.length = 7}: length 7, extra 0
Initialised plain; : length 0, start null
WithInitialiser() : length 0, extra 7
WithInitialiser{} : length 0, extra 7
```

Identical output on g162 (`-O0`, `-O1`, `-O2`), gsnapshot, clang2210 and clang_trunk (`-std=c++26`). The `static_assert`s check:

- `Defaulted` is trivial and trivially default constructible. `Braced`, `OutOfLine` and `WithInitialiser` are not trivially
  default constructible. `Braced` and `OutOfLine` are still trivially copyable.
- `Defaulted` and `Braced` are not aggregates (they have a constructor with parameters). `WithInitialiser` and `Plain` are.
- `Initialised` is not trivially default constructible and is still an aggregate; `Initialised plain;` reads as 0.
- `std::span<int>` is not trivially default constructible and not an aggregate.

GCC 16.2 and gsnapshot reject `WithInitialiser().length == 0` in a constant expression with `accessing uninitialized member`,
although the program prints 0 at run time. Clang 22.1 accepts it. So the class with a default member initialiser is checked
at run time in the file, not in a `static_assert`. I did not search for a GCC bug report.

## indeterminate_read.cpp

`Defaulted` has `Defaulted() = default;` and `Braced` has `Braced() {}`. Each mode reads the length of an object that no
constructor set: `automatic` (a `Defaulted` local), `braced` (a `Braced` local), `marked` (an `[[indeterminate]] int`
local), `heap` (`std::make_unique_for_overwrite<Defaulted>()`), `heap-value` (`std::make_unique<Defaulted>()`, which is
defined and prints 0).

GCC 16.2, plain build, the value printed (mode, then flags):

| mode | `-std=c++23 -O0` | `-std=c++26 -O0` | `-std=c++26 -O0 -ftrivial-auto-var-init=pattern` |
|---|---|---|---|
| `automatic` | `-104` | `0` | `-16843010` |
| `braced` | `0` | `0` | `-16843010` |

`-std=c++26 -O0` also printed `marked: length 29097` (a different number on every run, so a leftover from the stack),
`heap: length 0` and `heap-value: length 0`. Clang 22.1 prints `-1431655766` with `-ftrivial-auto-var-init=pattern`.
Clang 22.1 does not know the attribute: `warning: unknown attribute 'indeterminate' ignored`.

GCC 16.2 with `-std=c++26 -O1 -Wall` warns on the three reads and prints 0:

```text
warning: 'd.Defaulted::length' is used uninitialized [-Wuninitialized]
warning: 'b.Braced::length' is used uninitialized [-Wuninitialized]
warning: 'x' is used uninitialized [-Wuninitialized]
warning: '*(Defaulted*)<unknown>.Defaulted::length' is used uninitialized [-Wuninitialized]
```

Clang 22.1 with `-std=c++26 -O0 -g -fsanitize=memory` stops with `MemorySanitizer: use-of-uninitialized-value` in
`read_automatic`, `read_braced`, `read_marked` and `read_heap`, exit code 1. `heap-value` runs clean. At `-O1` the
sanitizer reported only the heap read, because the optimiser folded the other reads before instrumentation, so use `-O0`
(or `-fno-inline`).

Compiler Explorer caches a run by source and options and ignores the program arguments, so the runs above were repeated
with a changing trailing comment to avoid stale answers.

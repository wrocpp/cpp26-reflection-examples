# Three ways to sum a vector, and the zero that decides the type

Companion to a short post on wro.cpp. The post answers, in C++, a LinkedIn post by Prince Patel that showed a PHP
sum written three ways (a loop, one library call, a defensive version). The examples contain no macros and both exit 0.

- `examples/sum_three_ways.cpp`: a loop, `std::accumulate` and `std::ranges::fold_left` over `{0.5, 0.5, 0.5}`, `static_assert`s
  on the result types, and `std::saturating_add` at `INT_MAX`. Compiler Explorer (g162, `-std=c++26 -O1`): https://godbolt.org/z/PsEY39G3a
- `examples/sum_codegen.cpp`: the three sums over `std::span<const int>`, for the assembly view.
  Compiler Explorer (g162, `-std=c++26 -O2`, assembly pane): https://godbolt.org/z/q5o9jqhdE

All results were taken on Compiler Explorer on 2026-11-17.

## sum_three_ways.cpp

```text
loop        1.5
accumulate  0
fold_left   1.5
saturating_add(INT_MAX, 1)  2147483647
saturating loop      2147483647
```

Same output on g162 and gsnapshot. `std::saturating_add` is the C++26 name; `std::add_sat` (the name of the proposal) is not a
member of `std` on g162 (`error: 'add_sat' is not a member of 'std'`). clang2210 (libc++ and libstdc++) does not compile the file:
`no member named 'saturating_add' in namespace 'std'`.

## Who warns about accumulate(v.begin(), v.end(), 0) over a vector<double>

Tested on a copy of the program without `saturating_add` (clang2210 lacks it); the file has the same `accumulate` and `fold_left` calls.

| compiler id | options | warning for accumulate | warning for fold_left |
|---|---|---|---|
| g162 | `-std=c++26 -O1 -Wall -Wextra -Wpedantic -Wconversion -Wfloat-conversion -Wsign-conversion -Wdouble-promotion` | none | none |
| gsnapshot | `-std=c++26 -O1 -Wall -Wextra -Wconversion -Wfloat-conversion` (on the full example) | none | none |
| clang2210, libc++ | `-std=c++26 -O1 -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic` | none | none |
| clang2210, libstdc++ | same, without `-stdlib=libc++` | none | none |
| vcpp_v19_latest_x64 | default, `/W1`, `/W2`, `/W3`, `/W4`, `/Wall` (with `/std:c++latest /EHsc`) | C4244 | none |
| clangtidytrunk, on clang2210 | `-checks=-*,bugprone-fold-init-type` | `folding type 'double' into type 'int' might result in loss of precision` | none |

MSVC text: `numeric(31): warning C4244: '=': conversion from 'double' to '_Ty', possible loss of data` with `_Ty=int`, and a note
`see reference to function template instantiation '_Ty std::accumulate<...,int>(...)' being compiled` that points at the call line.

## sum_codegen.cpp

Compiled for x86-64 with the Compiler Explorer assembly filters (directives kept, comments removed, library code hidden).

| compiler | options | result |
|---|---|---|
| g162 | `-std=c++26 -O2` | scalar loop `add eax, [rdi]; add rdi, 4; cmp; jne` in `sum_loop` and `sum_accumulate`; `sum_fold` loads the first element before the same loop |
| g162 | `-std=c++26 -O3` | all three vectorised, with `paddd` in each |
| clang2210, libc++ | `-std=c++26 -O2` and `-O3` | all three vectorised, with `paddd` in each |

# The 2014 clone() helper and a forgotten override

Companion to episode 1 of the wro.cpp series "clone() and smart pointers: from 2014 to C++26".
The helper is from the 2014 article by Filip Sajdak on CppCodeReviewers,
[Covariant-Return-Types-and-Smart-Pointers](https://github.com/CppCodeReviewers/Covariant-Return-Types-and-Smart-Pointers),
file `FigureTests-v5.cpp` at commit `4729ab8`. Only its formatting is changed. `Tile` is new: it derives from `Square` and does
not override `clone()`.

- `examples/forgotten_override_2014.cpp`: the helper, `Tile`, and a `main` that clones a `Tile` through the helper.
  Compiler Explorer (g162, `-std=c++26 -O0 -Wall -Wextra -Wpedantic -fsanitize=undefined`): https://godbolt.org/z/j93fxGhb5

The program has no macros. Run without an argument it reads nothing that only `Tile` has, so it exits 0. Run with any argument
it also calls `joints()`, a virtual function that only `Tile` declares. All results were taken on Compiler Explorer on 2026-11-09.

## What the build and the run say

| compiler and options | compiler diagnostics | run |
|---|---|---|
| g162 (GCC 16.2), `-std=c++26 -O0 -Wall -Wextra -Wpedantic -fsanitize=undefined` | none | exit 0, `area=4`, two UBSan reports |
| clang2210 (Clang 22.1.0), same with `-fsanitize=undefined,vptr` | none | exit 0, `area=4`, two UBSan reports |
| g162, same as the first row, with the argument `crash` | none | SIGSEGV (exit 139) |
| g162, `-std=c++26 -O2`, no sanitizer, with the argument `crash` | none | SIGSEGV (exit 139), no output |
| g162 and clang2210, `-std=c++26 -O2`, no sanitizer, no argument | none | exit 0, `area=4` |
| g162, `-std=c++26 -O2 -Wall -Wextra -Wpedantic -fsanitize=address` | none | exit 0, `area=4`, no report |
| g162 and clang2210, `-std=c++11 -O0 -Wall -Wextra -Wpedantic` (the standard of 2014) | none | exit 0, `area=4` |

The two UBSan reports are the same on both compilers:

1. `runtime error: downcast of address ... which does not point to an object of type 'Tile'`, with the note
   `object is of type 'Square'`. The downcast is the `static_cast<T*>(ptr)` in `object::clone`.
2. `runtime error: member call on address ... which does not point to an object of type 'Tile'`, raised where the
   `std::unique_ptr<Tile>` is dereferenced.

## Why nothing warns

`Square::clone()` is not pure, so `Tile` is a complete class without its own override. The helper gets `T = Tile`,
calls `clone()` through `base_type`, receives the `Square` that `Square::clone()` made, and casts it to `Tile*`.
That cast is undefined behaviour ([expr.static.cast]). The dynamic type behind a pointer is not known at compile time,
and neither compiler warned.

# One element in braces: a copy that becomes an array

Companion to episode 3 of the wro.cpp series on clang-tidy checks for the mistakes AI coding agents keep making.
The check is `wrocpp-brace-init-list-hijack` from https://github.com/wrocpp/tidy-agent, a query check. Neither file
contains a macro, and both build and exit 0 with the default command.

- `examples/brace_init_list_hijack.cpp`: a JSON-like `Value` copied with braces and with parentheses, and
  `std::vector<int>` built with `{5}` and `(5)`. Run on GCC 16.2: https://godbolt.org/z/4dzdda3zv .
  Check on Compiler Explorer: https://godbolt.org/z/1P8qsdsWW
- `examples/which_braces_are_flagged.cpp`: eight braced initializations, five flagged and three not.
  Check on Compiler Explorer: https://godbolt.org/z/fv3vdeqj1

The check links open `x86-64 clang (trunk)` with `-std=c++20 -fsyntax-only` and the tool `clang-tidy (trunk)`
(LLVM 24.0git). The tool arguments are the query from `checks/query/brace-init-list-hijack.yaml`, passed with
`--experimental-custom-checks`. The shortlinks were compared byte for byte with the files. Runs of 2026-11-17.

## brace_init_list_hijack.cpp

Program output (GCC 16.2, `-std=c++20 -O1`, exit 0):

```text
braces: array of 1
parens: object
return {v}: array, return Value(v): object
vector<int>{5}: size 1, vector<int>(5): size 5
```

The same file built on 2026-11-17 with `-std=c++20 -O1` gave identical output on GCC 12.1, 13.2, 16.1, trunk, Clang 22.1 and
Clang trunk. MSVC 19.40, 19.44 and 19.51 (`/std:c++20` and `/std:c++latest`) gave a different first three lines:

```text
braces: object of 0
parens: object
return {v}: object, return Value(v): object
vector<int>{5}: size 1, vector<int>(5): size 5
```

clang-tidy, two warnings (the `return {v};` and the `braces{object}` line):

```text
brace_init_list_hijack.cpp:28:12: warning: one-element braced list selects the initializer_list constructor and wraps the value; use parentheses to copy or convert [custom-wrocpp-brace-init-list-hijack]
   28 |     return {v};  // flagged: this is a one-element array, not a copy
brace_init_list_hijack.cpp:43:17: warning: one-element braced list selects the initializer_list constructor and wraps the value; use parentheses to copy or convert [custom-wrocpp-brace-init-list-hijack]
   43 |     const Value braces{object};  // flagged
```

## which_braces_are_flagged.cpp

Program output (GCC 16.2 and Clang 22.1, `-std=c++20 -O1`, exit 0):

```text
Value{v}: 1, = {v}: 1, in make_unique: 1, Value{3}: 1, Value{v, v}: 2, member: 1, Box<Value>: 1
vector<int>(3): 3, vector<int>{3}: 1
```

clang-tidy flags five lines: the member `Value member{Value{}}`, the template member `stored{std::move(value)}` in `Box`
(reported once, at the template), `Value{source}`, `= {source}` and the inner `Value{source}` of `make_unique`.
It does not flag `Value{3}`, the two-element `Value{source, source}`, or `std::vector<int>{3}`. The two-element list is
correct. `Value{3}` and `std::vector<int>{3}` are hijacks of the same kind that the check does not see, because the
element is an `int` and the check looks for an element of the class itself.

## Local run

The same two files with Homebrew clang-tidy (LLVM 23.1.5) and `dist/tidy-agent.clang-tidy` from the repo:
`clang-tidy --experimental-custom-checks --config-file=dist/tidy-agent.clang-tidy --checks='-*,custom-wrocpp-brace-init-list-hijack' FILE -- -std=c++20`.
Two warnings on the first file and five on the second, on the same lines as on Compiler Explorer.
The repository has no plugin twin with the `{x}` to `(x)` fix-it yet (`clang-tidy -load=build/libwrocpp-tidy.so --list-checks`
lists four `wrocpp-` checks, none of them this one), so no fix-it was run.

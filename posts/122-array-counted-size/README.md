# A std::array with a hand-counted size accepts a list that is too short

Companion to episode 1 of the wro.cpp series "clang-tidy checks for the mistakes AI coding agents keep making". The check is
`array-counted-size` of https://github.com/wrocpp/tidy-agent (query form). The examples contain no macros.

- `examples/counted_size_trap.cpp`: a table declared for five levels with four names. Compiles, and the fifth name is empty.
  Compiler Explorer (g162, `-std=c++23 -O1`): https://godbolt.org/z/Gas58M57Y
  The same source with clang-tidy trunk and the query check (clang_trunk `-std=c++23`, tool `clangtidytrunk` with
  `--experimental-custom-checks`): https://godbolt.org/z/exYq39df8
- `examples/to_array_table.cpp`: the same table through `std::to_array`, with the count stated in a `static_assert`.
  Compiler Explorer (g162, `-std=c++23 -O1`): https://godbolt.org/z/x7azYxq53

Runs on 2026-11-17. Both programs exit 0.

## Observed output

`counted_size_trap.cpp`, g162, `-std=c++23 -O1`:

```text
5 names declared, name of Fatal: '' (0 characters)
```

`to_array_table.cpp`, g162, `-std=c++23 -O1`:

```text
complete: 5 of 5 entries
short:    4 of 5 entries
```

## Compiler warnings

`counted_size_trap.cpp` was compiled without errors on g162 (`-std=c++23 -Wall -Wextra -Wpedantic -Wmissing-field-initializers
-Wmissing-braces`), clang_trunk and clang2210 (same flags, and `-Weverything` without the C++98 and pre-C++20 compatibility
groups). The only warning is GCC's `-Wmissing-braces` about brace elision, which a complete list triggers as well. No flag
reported the short list.

## clang-tidy with the query check

Query from `checks/query/array-counted-size.yaml` of tidy-agent at commit 7c64b6c.

- Compiler Explorer, clang-tidy trunk (LLVM 24.0git), `counted_size_trap.cpp`:

```text
<source>:20:1: warning: constant std::array with a hand-counted size; a shorter list compiles and zero-fills the tail, use std::to_array [custom-wrocpp-array-counted-size]
   20 | constexpr std::array<std::string_view, kLevelCount> kNames{"debug", "info", "warning", "error"};
      | ^
```

- Local, Homebrew clang-tidy 23.1.5, same config: the same single warning on `counted_size_trap.cpp`, and none on
  `to_array_table.cpp`. `tools/run_tests.py array-counted-size` passes.
- Cases outside the repository's tests, run on Compiler Explorer (clang-tidy trunk): a `const std::array<int, kCount>` with a
  named constant `kCount` is flagged (the spec says it must not be); a `static constexpr std::array<int, N>` in a class template
  with `N` a template parameter is flagged (the spec says it must not be); `std::array<int, 8> zeroed{}`, a non-const
  array and `constexpr auto x = std::to_array<int>({1, 2, 3})` are not flagged.

The plugin twin with the fix-it to `std::to_array` that the spec describes does not exist in tidy-agent at 7c64b6c, so no fix-it
was run.

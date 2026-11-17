# views::as_input in front of filter

Companion to the wro.cpp short "views::as_input before filter makes reverse a compile error". The example contains no macros.

- `examples/as_input_before_filter.cpp`: type-level checks that `reverse` accepts a filter over a vector and refuses the same
  filter behind `views::as_input` (and a filter over a `std::generator`), a pipeline that moves the long names out in order,
  and, selected by `argv[1] == "unsafe"`, the filter then reverse then `as_rvalue` pipeline that is undefined behavior.
  Compiler Explorer (g162, `-std=c++26 -O1`): https://godbolt.org/z/j1dEh9K7W

All results were taken on Compiler Explorer on 2026-11-17.

## Default run (no argument), exit 0

```text
moved in order: Amsterdam Berlin Cologne
```

Same output on g162, gsnapshot (GCC 17.0.0 trunk of that day) and clang_trunk with libstdc++, all `-std=c++26`.

## `unsafe` argument

g162 and gsnapshot, `-std=c++26 -fsanitize=address,undefined`, argument `unsafe`: exit code 1.

```text
==2==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x760dc5fe0028 at pc 0x00000040f14d ...
READ of size 8 at 0x760dc5fe0028 thread T0
    #0 ... in std::__cxx11::basic_string<...>::size() const
    #1 ... in operator() /app/example.cpp:25            (the IsLong predicate)
    #4 ... in operator-- .../ranges:1825                (filter_view iterator, decrement)
```

The decrement of the filter iterator calls the predicate on elements that `as_rvalue` already moved from, and walks past the
first element of the vector. That reading is mine; the paper P3725R3 reports the same program shape as undefined behavior.

## What does not compile

Dropping the `static_assert`s and writing the pipeline directly:

```cpp
auto r = names | std::views::as_input | std::views::filter(IsLong{}) | std::views::reverse;
```

- g162 and gsnapshot: `error: no match for 'operator|' (operand types are 'std::ranges::filter_view<std::ranges::as_input_view<std::ranges::ref_view<std::vector<std::__cxx11::basic_string<char> > > >, IsLong>' and 'const std::ranges::views::_Reverse')`
- clang_trunk with libstdc++: `error: invalid operands to binary expression`.
- clang_trunk with `-stdlib=libc++`: `error: no member named 'as_input' in namespace 'std::ranges::views'`. The whole file also
  fails there because libc++ has no `<generator>`: `fatal error: 'generator' file not found`.
- clang2210 (Compiler Explorer's libstdc++ 15, `_GLIBCXX_RELEASE` 15, `__GLIBCXX__` 20250808): `as_input` is unknown
  (`did you mean 'to_input'?`). `to_input` and `__cpp_lib_ranges_to_input` (202502) exist there. GCC 16.2 and trunk have it the
  other way round: `as_input`, `__cpp_lib_ranges_as_input` (202502), and `views::to_input` is `not a member of 'std::views'`.

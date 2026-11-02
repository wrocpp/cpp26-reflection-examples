# A generator over a struct's members, and why `co_yield` cannot sit in `template for`

Companion to the wro.cpp short `co-yield-inside-template-for`. The earlier shorts are
`generator-enumerate-by-value` (examples in `posts/101-generator-enumerate`) and
`generator-const-reference-lifetime` (examples in `posts/108-generator-lifetime`). Nothing from them is
copied here.

- `examples/fields.cpp`: `fields(object)` yields (member name, value as text) for each non-static data member.
  The members are reflected into a `constexpr` array outside the coroutine and an `index_sequence` fold yields
  one pair per member. `-DSHOW_TEMPLATE_FOR` and `-DSHOW_TEMPLATE_FOR_INDEX` swap in the `template for` forms,
  which do not compile. The last output line is the heap allocations of one walk. Compiler Explorer (g162,
  `-std=c++26 -freflection`): https://godbolt.org/z/3xMfz3vbY
- `examples/nested.cpp`: the same for nested aggregates, with one helper coroutine per member and
  `std::ranges::elements_of` handing the inner values to the outer generator. Compiler Explorer (g162,
  `-std=c++26 -freflection`): https://godbolt.org/z/18W1xshaM
- `examples/elements_of.cpp`: `elements_of(first | std::views::as_rvalue)` against a bare `elements_of(first)`
  (`-DSHOW_BARE_ELEMENTS_OF`, does not compile).
- `examples/enumerate_fields.cpp`: `std::views::enumerate(fields(point))`. `-DSHOW_TUPLE_ENUMERATE` adds the
  `std::tuple<int, int>` enumerate from `generator-enumerate-by-value`, which yields integer pairs only and
  does not accept the pair that `fields()` yields.

This is an example, not a library.

Everything ran on Compiler Explorer (x86-64) on 2 November 2026: g162 is GCC 16.2.0 and gsnapshot is
GCC 17.0.0 20261102, both with `-std=c++26 -freflection`. Clang trunk on Compiler Explorer has no `std::meta`,
so nothing here ran on Clang.

## What does not compile

First error line of each, from `fields.cpp`:

| form | g162 | gsnapshot |
|---|---|---|
| `template for (constexpr auto member : members<T>)` with `co_yield` in the body | `consteval-only expressions are only allowed in a constant-evaluated context` | `call to consteval function 'std::meta::identifier_of(((std::meta::info)member))' is not a constant expression` |
| `template for (constexpr std::size_t index : std::views::iota(0uz, members<T>.size()))` | `call to consteval function 'std::meta::identifier_of(...)' is not a constant expression` | `call to consteval function 'std::meta::identifier_of(...)' is not a constant expression` |

Both compilers also report `'frame_ptr' is not a constant expression`. Controls: the same `template for` with
`co_yield` over a `constexpr std::array<int, 3>` builds and runs on both. With only a splice
(`co_yield object.[:member:]`) it fails on g162 and builds on gsnapshot.

## What runs

| file | compiler | options | output |
|---|---|---|---|
| fields.cpp | g162, gsnapshot | `-std=c++26 -freflection` | three lines, then `3 fields, 1 allocations, 448 bytes` |
| fields.cpp | g162, gsnapshot | `-O2` added | `call to consteval function 'std::meta::identifier_of(...)' is not a constant expression` |
| nested.cpp | g162, gsnapshot | `-std=c++26 -freflection`, and `-O2` | six lines, then `6 fields, 11 allocations, 3104 bytes` |
| elements_of.cpp | g162, gsnapshot | `-std=c++26` | `1 2 3` on three lines |
| enumerate_fields.cpp | g162, gsnapshot | `-std=c++26 -freflection` | `0: x = 1`, `1: y = 2.5`, `2: name = p` |

## Warnings (observed, not explained)

With `-std=c++26 -freflection -O2 -Wall -Wextra`, both compilers print `-Wmismatched-new-delete` for
`fields.cpp` and `nested.cpp`, naming `std::__gen::_Promise_alloc<void>::operator delete(void*, std::size_t)`.
Adding `-Werror` fails the build. At `-O0` there is no warning. A fold with no reflection and no replaced
`operator new`, `((co_yield int(Is)), ...)` over an `index_sequence`, does the same at `-std=c++26 -O2`.
Three separate statements `co_yield 0; co_yield 1; co_yield 2;` and a `for` loop do not. I made no further
reduction, so I do not know whether the warning is correct.

On gsnapshot at `-O2`, eight builds submitted in parallel were killed (exit 143, no compiler output); the same
builds run one at a time finished in about 15 seconds. The `nested.cpp` builds on gsnapshot at `-O2` finished
in both runs I made of them, one of them in parallel with three others.

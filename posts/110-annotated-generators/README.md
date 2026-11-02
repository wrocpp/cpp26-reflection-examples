# Annotate a class, get a generator

Companion to the wro.cpp short `annotate-a-class-get-a-generator`. It builds on
`co-yield-inside-template-for` (examples in `posts/109-co-yield-template-for`, which explains why the
reflection values live in a `constexpr` array outside the coroutine) and on
`generator-const-reference-lifetime` (`posts/108-generator-lifetime`). Nothing is copied from them.

This is an example, not a library.

- `examples/annotated_fields.cpp`: `[[=skip{}]]` hides a data member and `[[=label{"user_name"}]]` renames it.
  `fields(object)` yields (name, value as text) for the rest. `-DSHOW_STRING_VIEW_LABEL` makes the annotation
  payload a `std::string_view`: does not compile (`annotation does not have structural type`).
  Compiler Explorer (g162, `-std=c++26 -freflection`): https://godbolt.org/z/bx33jv579
- `examples/annotated_steps.cpp`: `[[=step{"parse"}]]` marks member functions. `run_steps(object)` yields
  (title, result) and calls the next function only when the consumer asks. Breaking after the second step
  leaves the third uncalled. Compiler Explorer (g162, `-std=c++26 -freflection`): https://godbolt.org/z/zjoj5cGzG
- `examples/lazily_adapter.cpp`: the negative result. `lazily<^^Catalog::evens>(catalog, n)` wraps an ordinary
  vector-returning method in a `std::generator`. For three elements taken it builds 1000, against 7 for the same
  method written as a coroutine. Compiler Explorer (g162, `-std=c++26 -freflection`): https://godbolt.org/z/q8ETW1v38

All three ran on Compiler Explorer (x86-64) on 2 November 2026 on g162 (GCC 16.2.0) and gsnapshot (GCC
17.0.0 20261102) with `-std=c++26 -freflection`, with identical output. At `-O2 -Wall -Wextra -Werror` all
three stop on `-Wmismatched-new-delete` inside `<generator>`, as in `posts/109-co-yield-template-for`.
Clang on Compiler Explorer has no `std::meta`, so nothing ran on Clang.

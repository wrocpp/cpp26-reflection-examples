# A task next to a generator

Companion to the wro.cpp short `task-next-to-a-generator`. It builds on `generator-const-reference-lifetime`
(examples in `posts/108-generator-lifetime`). Nothing is copied from it.

`exec::task` here is the coroutine task of NVIDIA's `stdexec` library, a reference implementation. It is not
`std::execution::task` from the standard: `__cpp_lib_senders` and `__cpp_lib_task` are undefined on g162 and
gsnapshot with `-std=c++26`.

- `examples/task_and_generator.cpp`: a `std::generator` and an `exec::task` side by side, with a counter on
  `operator new`. The counters are read through a `noinline` function because GCC 15 and later assume a replaced
  `operator new` has no side effects (`-fassume-sane-operators-new-delete`). Compiler Explorer (g162,
  `-std=c++26 -O2`, library `stdexec` trunk): https://godbolt.org/z/d6WWjhWeq
- `examples/task_reference_lifetime.cpp`: a task that takes `const std::string&` and is called with a temporary.
  The default build takes the string by value and is clean. `-DBY_REFERENCE` takes it by reference: AddressSanitizer
  reports `stack-use-after-scope`. Compiler Explorer (g162, `-std=c++26 -O2 -fsanitize=address -g`, library
  `stdexec` trunk, default build): https://godbolt.org/z/91c9s48Pj

## Measured (Compiler Explorer, x86-64, 2 November 2026)

Heap allocations and bytes, exact and identical across two runs:

| coroutine | g162 -O0 and -O2 | clang_trunk (24.0.0git) -O0 | clang_trunk -O2 |
|---|---|---|---|
| generator, 3 values | 1, 104 bytes | 1, 96 bytes | 0 |
| `fetch_answer()` task | 1, 176 bytes | 1, 176 bytes | 1, 168 bytes |
| task awaiting two tasks | 3, 968 bytes | 3, 936 bytes | 3, 688 bytes |

stdexec is the Compiler Explorer library `stdexec` version `trunk`. The headers define no version macro
(`STDEXEC_VERSION` and `STDEXEC_MAJOR` are undefined), so the commit is not known. gsnapshot (GCC 17.0.0
20261102) stops with an internal compiler error in `exec/any_sender_of.hpp:93` when `<exec/task.hpp>` is
included. `beman::task` trunk fails on g162 and clang_trunk with a static assertion at
`beman/task/detail/task_scheduler.hpp:198`.

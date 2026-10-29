# Coroutines as a debugger

Companion to two wro.cpp posts that start from Alexey Ozeritskiy's LinkedIn post about a WebAssembly
debugger built on coroutines. Both files are toy models written with C++20 coroutines. They are not
Qumir's code: the Qumir design notes (docs/arch/browser-debugger.md, docs/arch/coroutine.md in
github.com/QumirLabs/qumir) describe their own coroutine/Future ABI lowered through LLVM coroutine intrinsics.

- `examples/stepper.cpp`: a single-stepper. A checkpoint before each statement, shared state, step into,
  step over, step out, breakpoints, a call stack at each stop, and a fast path that never suspends.
  Compiler Explorer (g162, runs natively, not in WebAssembly): https://godbolt.org/z/EWYanxPhG
- `examples/coroutine_cost.cpp`: what turning every function into a coroutine costs. Compiler Explorer:
  https://godbolt.org/z/rsnxqno9P

## Measured (GCC 16.2)

Heap allocations, exact on every machine (fib(27), 635,621 calls):

| variant | allocations | per call | bytes per allocation |
|---|---|---|---|
| plain function | 0 | 0 | |
| coroutine, no checkpoint | 635,621 | 1 | 80 |
| coroutine, checkpoint that never stops | 635,621 | 1 | 80 |

Time is noisy. On an Apple M2 Max (loaded) the plain function took 0.29 to 0.49 ms in every run, and the
fastest coroutine run in any of eleven runs was 16.3 ms; the same variant ranged up to 79 ms. The variants
with and without a never-stopping checkpoint were not distinguishable within that noise. One Compiler
Explorer run (x86-64 g162, cached output): plain 0.29 ms, frame only 11.99 ms, with checkpoint 12.41 ms.

At -O0 and -O1 the large run crashes (SIGSEGV) with the default 8 MB stack and completes on a 1 GiB
stack. fib(20) runs and fib(22) does not at -O0. Why -O2 avoids it is not known: -O2
-fno-optimize-sibling-calls still runs, and -O1 -foptimize-sibling-calls still crashes.

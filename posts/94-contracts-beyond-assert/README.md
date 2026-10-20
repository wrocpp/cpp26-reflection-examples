# What contracts would have to be to prove something

Experiments behind the post `contracts-that-prove-something`. They test five wishes from a reader's
wishlist (Pavel Simerda, https://lnkd.in/p/ez6SdGbv) against GCC 16.2.0, and a few patterns that work today.
Every figure below was produced by the file and script named next to it.

Run one experiment in the image (the worktree is mounted at `/src`):

    docker run --rm -v "$PWD":/src:ro -v "$PWD/posts/94-contracts-beyond-assert/scripts/e1.sh":/e.sh:ro \
      reflect-moc/gcc16-qt610 bash /e.sh

The runnable demo (`contracts_that_repeat.cpp`) is also on Compiler Explorer, GCC 16.2, x86-64:
https://godbolt.org/z/9fjGY8fYo

Assembly counts were taken on aarch64 (the local image). E2 and E3 were repeated on x86-64 (Compiler
Explorer, `g162`): the call counts agree, and the instruction counts differ by a few. E5 and E8 were
repeated there too, with the same output. E6 and E7 are aarch64 only.

| # | File | Question | Result on GCC 16.2 |
|---|---|---|---|
| E1 | `e1_bodyless_predicate.cpp` | Is a condition with no definition a promise? | At `ignore` it links and runs. At `observe`, `enforce` and `quick_enforce` it fails to link: `undefined reference to valid_port(int)` and `valid_address(Address const&)` |
| E2 | `e2_chain.cpp` | Does `make_address`'s postcondition make `connect_to`'s precondition free? | The checks run inside each callee. With the predicate defined elsewhere, or marked `[[gnu::pure]]`, all three remain even when both functions are forced inline and the port is the constant 8080 (3 calls in `main`). With the predicate visible (defined inline), a constant port and `enforce`, `main` is as short as the `ignore` build (9 instructions on aarch64, 8 on x86-64). With a visible predicate and a runtime port, one range test remains (34 instructions against 11) |
| E3 | `e3_result.cpp` | Do `r < 0`, `r == -1`, `r != 0` compile alike when the postcondition says 0 or -1? | Callee defined elsewhere: no, the code differs (`shr 31`, `cmp -1`, `test`), at `ignore` and `enforce` alike. Callee visible: yes, one `cmp [port], 1; sete`, and `r > 0` becomes `xor eax, eax` and `r <= 0` becomes `mov eax, 1`, the same at `ignore` and `enforce`. No warning from `-Wall -Wextra -Wtype-limits -Wtautological-compare` or `-fanalyzer` |
| E4 | `e4_switch.cpp` | What warns about a switch that misses or invents a case? | `switch (errno)` on an `int`: nothing. A `switch` over an `enum class`: `-Wswitch` names the missing enumerator and the case that is not an enumerator |
| E5 | `e5_constant_port.cpp` | Is a bad constant port already a compile-time error? | Port 8080: compiles at every semantic. Port 70000: silent at `ignore`, a warning at `observe`, an error at `enforce` and `quick_enforce`: `contract predicate is false in constant expression` |
| E6 | `e6_types.cpp` | Does a type move the check to one place? | Plain ints with four functions: 4 violation-handler call sites at `enforce` (138 instructions in the file). `Port` type with a `parse` boundary: 0 call sites (53 instructions), because the constructor's precondition follows an `if` that proves it |
| E7 | `e7_assume.cpp` | What does `[[assume]]` do that a postcondition does not? | The test after the assume is removed: the call that breaks the promise prints -1 at `-O0` and 0 at `-O2`. It is unchecked, and false means undefined behaviour |
| E8 | `contracts_that_repeat.cpp` | How often is one bad value reported? | With plain ints, port 70000 is reported by 4 functions. With the `Port` type behind `parse`, 0. `Port{70000}` built directly: 1 |

# RAII for C APIs, part 1: scope guards

Companion code for the wro.cpp post on the three Library Fundamentals (P0052) scope guards, as shipped
in libstdc++'s `<experimental/scope>`.

## Verified on GCC 16.2.0 (Compiler Explorer g162, `-Wall -Wextra -Werror`, C++23 and C++26)

- `<experimental/scope>` ships `scope_exit`, `scope_fail` and `scope_success`. They fire on every exit,
  on an unwinding exception, and on a normal exit respectively (confirmed by the run).
- `scope_exit` and `scope_fail` destructors are unconditionally noexcept: they can run while an
  exception unwinds, so a throwing action calls `std::terminate`.
- `scope_success`'s destructor is conditionally noexcept (`is_nothrow_destructible` is 0 for a throwing
  action). It runs only on a normal exit, so its action may throw and the exception propagates.
- A `scope_success` can therefore carry a commit that fails on a normal exit. A header that
  `static_assert`s every action noexcept (for example AmitGDev/scope) rejects that case by design.

File: `examples/scope_guards.cpp`. Godbolt: https://godbolt.org/z/YT6xMs4s5

# The initialization profile against real code

Companion to the wro.cpp post `init-profile-against-real-code`. The code runs on the experimental
Compiler Explorer build `clang_profiles_init` ("x86-64 clang (std::init profile - P4222)"), which
implements the initialization profile of P4222R3 in a Clang fork. It is not a shipping compiler.

- `examples/init_cases.cpp`: cases from P4222R3 that the profile accepts, and one macro per case it
  rejects (`SHOW_MEMBER_WRITE`, `SHOW_STILL_UNINIT`, `SHOW_WRONG_STATE`, `SHOW_STATIC_INIT`,
  `SHOW_WRONG_RULE`). It uses the rule-level suppression form, which the GCC build rejects.
  Compiler Explorer (`clang_profiles_init`, `-std=c++26 -fprofiles`): https://godbolt.org/z/fsad41GWc
- `examples/ticker_member_order.cpp`: a thread member declared before the counter it uses, run
  through the profile. `-DTHREAD_LAST` declares the thread last. Both orders print `done`.
  Compiler Explorer (`clang_profiles_init`, `-std=c++26 -fprofiles -pthread`): https://godbolt.org/z/qrGb8fxMv
- `examples/member_order.cpp`: compile only. A constructor reads a member before its initializer.

The thread case is a reworded version of case 2, "Timing Is Everything", from Jelle Bakker's ebook
"25 C++ Cases That Will Humble You" (LinkedIn: https://lnkd.in/p/d9Xiewbs). The code is new.

## Observed on 2 November 2026

Both builds follow their development branches, so the output can change.

| build | options | `init_cases.cpp` | `ticker_member_order.cpp` |
|---|---|---|---|
| `clang_profiles_init` | `-std=c++26 -fprofiles` | prints `3 4 4 4`, exit 0 | prints `done`, exit 0 |
| `gcontracts-p4324-trunk` | `-std=c++23 -pedantic-errors` | rejects the `rule:` argument | prints `done`, exit 0 |

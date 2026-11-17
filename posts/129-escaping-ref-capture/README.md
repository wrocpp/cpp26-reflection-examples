# A lambda that captures by reference and escapes: what wrocpp-escaping-ref-capture, Clang 23.1 and GCC 16.2 report

Companion to episode 8 of the wro.cpp series "clang-tidy checks for the mistakes AI coding agents keep making". The
subject is `escaping-ref-capture` of https://github.com/wrocpp/tidy-agent, a plugin check without a fix-it. The examples
contain no macros.

- `examples/cases.cpp`: nine cases that dangle and five that do not. Run without an argument it runs the five safe cases,
  prints `5 safe cases ran` and exits 0. `./cases N` runs dangling case N (1 to 9): the escaped lambda is called after its
  frame ended, for a sanitizer.
  Compiler Explorer (g162, `-std=c++23 -O0 -g -fsanitize=address -Wall -Wextra`, default path): https://godbolt.org/z/6GhbbGz99
- `examples/move_only.cpp`: the same mistake with `std::move_only_function`. `./move_only` prints `read 'a'`; `./move_only bug`
  calls the dangling lambda.
  Compiler Explorer (g162, same options, default path): https://godbolt.org/z/5xrPG61fa

Runs on 2026-11-17. Both programs exit 0 without an argument.

## The check

Plugin of tidy-agent at commit 7c64b6c (the check source and its spec were last changed in 48a66cd). It reports a lambda with
a by-reference capture (`[&]` or `[&x]`, also an init-capture `[&r = x]`) that is the operand of a `return`, or that is
converted to `std::function`, `std::move_only_function` or `std::copyable_function`. It has no fix-it. Plugins cannot run
on Compiler Explorer, so these runs are local: Homebrew LLVM 23.1.5, the plugin built against it from the tidy-agent tree
(`cmake -S checks/plugin -B build -G Ninja -DLLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm`), then

```text
clang-tidy -load=build/libwrocpp-tidy.so --checks='-*,wrocpp-escaping-ref-capture' cases.cpp -- -std=c++23
```

`move_only.cpp` needs `std::move_only_function`, which libc++ 23.1 does not have, so it was run with GCC 16.2's libstdc++
headers: `-- -std=c++23 -nostdinc++ -isystem <gcc 16.2 include/c++/16> -isystem <same>/aarch64-apple-darwin25`.

## The cases

| case | function | what escapes |
|---|---|---|
| 1 | `make_explicit` | `return [&label] {...};` with an `auto` return type |
| 2 | `make_default` | `return [&] {...};` as a `std::function` |
| 3 | `make_init_capture` | `return [&ref = label] {...};` as a `std::function` |
| 4 | `fill_stored_variable` | `Handler h = [&label] {...}; registry.add(h);` |
| 5 | `fill_direct_argument` | `registry.add([&label] {...});` |
| 6 | `fill_push_back` | `registry.handlers.push_back([&] {...});` |
| 7 | `make_named_variable` | `auto f = [&label] {...}; return f;` |
| 8 | `fill_from_variable` | `auto f = [&label] {...}; registry.add(f);` |
| 9 | `make_holder` | `return Holder{[&label] {...}};` with a template wrapper |

Safe cases: 1 a by-value capture, 2 a by-reference lambda called in the same scope, 3 a `[&]` lambda stored in a member of
its own object (`bump` in a local struct), 4 a `std::function` variable called at once, 5 a `[&]` lambda passed to a
`const std::function<int()>&` parameter that calls it before returning.

## Every dangling case is real

Compiler Explorer, g162 and clang2210, `-std=c++23 -O0 -g -fsanitize=address`, argument 1 to 9 each: every run exits 1 with
`AddressSanitizer: stack-use-after-return` (18 of 18). Without an argument, g162 prints `5 safe cases ran` and exits 0. `move_only bug` on g162: the same report. Excerpt for case 4:

```text
==2==ERROR: AddressSanitizer: stack-use-after-return on address 0x70bc5a3f0068 at pc 0x00000040d15b bp 0x7fff3b10b7b0 sp 0x7fff3b10b7a8
READ of size 8 at 0x70bc5a3f0068 thread T0
    #2 0x00000040331d in first_char(...) /app/example.cpp:38
    #3 0x0000004038cf in operator() /app/example.cpp:59
```

Local macOS check, Homebrew clang 23.1.5, `-O0 -g -fsanitize=address`: with `ASAN_OPTIONS=detect_stack_use_after_return=1`
all nine cases report `stack-use-after-return`. With the macOS default only cases 5 and 6 are reported (a
`heap-use-after-free`); cases 2, 3, 4 and 8 print a value (a stale byte) and exit 0, and cases 1, 7 and 9 end without output.

## What each tool reported on cases.cpp

Local runs, `-std=c++23`: `clang++ -fsyntax-only -Wall -Wextra` (Homebrew 23.1.5), the same with `-Wlifetime-safety`, and
`g++-16 -O2 -c -Wall -Wextra -Wdangling-pointer=2 -Wdangling-reference` (Homebrew GCC 16.2.0; `-O0` printed nothing).

| case | wrocpp-escaping-ref-capture | Clang 23.1 | Clang 23.1 with -Wlifetime-safety | GCC 16.2 -O2 |
|---|---|---|---|---|
| 1 | flagged (line 43) | `-Wreturn-stack-address` | adds `-Wlifetime-safety-return-stack-addr` | `-Wuninitialized`, inside `basic_string.h` |
| 2 | flagged (48) | silent | `-Wlifetime-safety-return-stack-addr` | silent |
| 3 | flagged (53) | silent | `-Wlifetime-safety-return-stack-addr` | silent |
| 4 | flagged (59) | silent | silent | silent |
| 5 | flagged (65) | silent | silent | silent |
| 6 | flagged (70) | silent | silent | silent |
| 7 | silent | silent | `-Wlifetime-safety-return-stack-addr` | `-Wuninitialized`, inside `basic_string.h` |
| 8 | silent | silent | silent | silent |
| 9 | silent | silent | silent | `-Wuninitialized`, inside `basic_string.h` |

Totals on the nine dangling cases: check 6, Clang without the flag 1, Clang with the flag 4, GCC 16.2 at -O2 3 (all three
in `basic_string.h`, worded as an uninitialized value). The check and Clang with the flag together cover 7 (cases 1 to 7).
Case 8 is reported by no tool.

On the five safe cases the check reported three: safe cases 3, 4 and 5 (lines 142, 150 and 158). Clang and GCC reported none.
The spec of the check lists the `[&]` lambda in a member of its own object under "Must not flag"; the check as built
reports it.

Output of the check on case 4 (local, clang-tidy 23.1.5):

```text
cases.cpp:59:17: warning: lambda captures by reference and escapes the enclosing scope; capture by value, or keep the call inside the scope [wrocpp-escaping-ref-capture]
```

`move_only.cpp`: the check reports line 26 (`make_dangling`) and not `make_safe`.

## What the check cannot see

Cases 7, 8 and 9. The check looks for a lambda expression directly under a `return` or under the construction of a
`std::function`. Once the lambda is first bound to an `auto` variable, or wrapped in a class template, it is gone.
Also not covered: a lambda handed to `std::thread` or `std::async`, which are not in the `CallableTypes` option, and a
`[this]` capture of an object that is destroyed first (the check only looks at by-reference captures).

# A shared_ptr that is never shared: what wrocpp-needless-shared-ptr flags, fixes and misses

Companion to episode 7 of the wro.cpp series "clang-tidy checks for the mistakes AI coding agents keep making". The subject
is `needless-shared-ptr` of https://github.com/wrocpp/tidy-agent, a clang-tidy plugin check with a fix-it. A plugin cannot
run on Compiler Explorer, so its output below comes from a local run. The examples contain no macros.

- `examples/cases.cpp`: fifteen shapes of a local `shared_ptr`. Three are flagged, nine are not, three show the check's limits.
  Prints `15 cases ran, 2 pointers stored, total 32` and exits 0.
  Compiler Explorer (g162, `-std=c++23 -O2 -Wall -Wextra`): https://godbolt.org/z/dW4n9bTW8
- `examples/shared_from_this.cpp`: the one false positive I found. The check flags a function whose object shares itself
  through `shared_from_this()`, and the fix-it breaks it at run time. Prints `before the fix: registry holds 1` and
  `after the fix: bad_weak_ptr`, and exits 0.
  Compiler Explorer (g162, `-std=c++23 -O2 -Wall -Wextra`): https://godbolt.org/z/5PPq3Y7jY
- `examples/savings.cpp`: sizes, allocation count and bytes for `make_shared` against `make_unique`, with `operator new`
  replaced to count. Built at `-O0` so the optimiser cannot elide the unused allocation.
  Compiler Explorer (g162, `-std=c++23 -O0 -Wall -Wextra`): https://godbolt.org/z/d8e8sEjar

Runs on 2026-11-17. All three programs exit 0. Each link was compared with its file through `/api/shortlinkinfo`.

## How the check was run

Local, because it is a plugin. tidy-agent at commit 7c64b6c, plugin built with Homebrew LLVM 23.1.5 (the same major version
as the `clang-tidy` that loads it):

```text
cmake -S checks/plugin -B build -G Ninja -DLLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm \
  -DClang_DIR=/opt/homebrew/opt/llvm/lib/cmake/clang && cmake --build build
clang-tidy -load=build/libwrocpp-tidy.so --checks='-*,wrocpp-needless-shared-ptr' cases.cpp -- -std=c++23
```

The spec of the check is `docs/checks/needless-shared-ptr.md` in that repository. It records no incident in the two codebases
mined for the series, and keeps the check as a common agent habit and as the worked example for a data-flow check.

## What it reported on cases.cpp

```text
cases.cpp:31:10: warning: 'w' is a shared_ptr that is never shared; use std::unique_ptr or a value [wrocpp-needless-shared-ptr]
cases.cpp:37:10: warning: 'w' is a shared_ptr that is never shared; use std::unique_ptr or a value [wrocpp-needless-shared-ptr]
cases.cpp:44:29: warning: 'w' is a shared_ptr that is never shared; use std::unique_ptr or a value [wrocpp-needless-shared-ptr]
```

Line 31 is `flagged_arrow`, line 37 `flagged_star_and_get`, line 44 `flagged_explicit_type`. The nine `kept_*` functions are not reported.
The three `missed_*` functions are not reported either, although their pointers are never shared.

| function | uses of the pointer | result |
|---|---|---|
| `flagged_arrow` | `->`, `get()`, `operator bool` | flagged, fix-it |
| `flagged_star_and_get` | `*`, `get()` | flagged, fix-it |
| `flagged_explicit_type` | `->`, declared as `std::shared_ptr<Widget>` | flagged, no fix-it |
| `kept_returned`, `kept_weak` | returned | not flagged |
| `kept_stored`, `kept_moved` | `push_back` of a copy, of a move | not flagged |
| `kept_captured` | captured by value in a lambda | not flagged |
| `kept_passed_by_value`, `kept_passed_by_const_ref` | passed to a function | not flagged |
| `kept_alias` | bound to a `const auto&` | not flagged |
| `kept_use_count` | `use_count()` | not flagged |
| `missed_null_compare` | `w == nullptr` | not flagged, false negative |
| `missed_new_expression` | `std::shared_ptr<Widget> w{new Widget{}}` | not flagged, false negative |
| `missed_allocate_shared` | `std::allocate_shared` | not flagged, false negative |

## The fix-it

`clang-tidy --fix` on a copy of `cases.cpp`:

```text
clang-tidy applied 2 of 2 suggested fixes.
```

That counts edits, not warnings: three warnings were reported and two of them carry a fix. The diff is two lines, each
`std::make_shared<Widget>()` becoming `std::make_unique<Widget>()` on an `auto` variable. The fixed file builds and prints the
same line, `15 cases ran, 2 pointers stored, total 32` (g++-16, `-std=c++23`).

## The false positive

`shared_from_this.cpp`: `Node` derives from `std::enable_shared_from_this<Node>`, and `Node::announce` stores
`shared_from_this()` in a registry. `register_node` only calls `n->announce(...)`, so the check flags it (line 26), and the fix-it
changes it to `std::make_unique<Node>()`. With the fixed function in place, the program from a copy of the file under
`--fix` ends with:

```text
terminate called after throwing an instance of 'std::bad_weak_ptr'
  what():  bad_weak_ptr
```

(exit status 134). The file in this directory keeps both versions side by side and catches the exception, so it exits 0.

## What the change saves

`savings.cpp`, g162, `-std=c++23 -O0 -Wall -Wextra`:

```text
sizeof(Widget) 4, sizeof(shared_ptr) 16, sizeof(unique_ptr) 8
make_shared: 1 allocation, 24 bytes, use_count 1
make_unique: 1 allocation, 4 bytes
```

The same source built locally with Homebrew clang 23.1.5 and libc++ (`-std=c++23 -O0 -stdlib=libc++`) printed the same sizes
and `make_shared: 1 allocation, 32 bytes`, `make_unique: 1 allocation, 4 bytes`. At `-O1` GCC 16.2 printed `0 allocation, 0 bytes` for both,
because it removed the unused allocations; that is why the file is built at `-O0`. No time was measured.

## The public codebase

`cloudevents-sdk-cpp` at commit a414e8d (the head on 2026-11-17): `grep` over `include`, `test`, `examples`, `bench`, `fuzz` and
`interop` finds two `std::make_shared` calls, both in `json_document::make` (`include/cloudevents/core.hpp:39` and
`include/cloudevents/v3/core.hpp:38`). Each passes the pointer to the constructor of a class that keeps it as a copyable
handle. I did not run the check on that file, because the headers need `ctre.hpp`, which was not installed.

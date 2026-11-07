# Structured bindings in a range-for: copies, and what a copy protects

Companion to the wro.cpp post "A by-value structured binding copies the element but not the iterator".
The loop `for (auto [id, payload] : registry)` copies every element of a `std::map<int, Payload>`.
Adam Bienkunski's LinkedIn post of 7 November 2026 shows it. The code here is our own.

- `examples/structured_binding_copies.cpp`: one file. The default build runs only loops that stay defined
  and exits 0. `-DMATRIX_CASE` selects one loop form that can be undefined. `-DSHOW_ERROR` adds a
  move-only payload that fails to compile. Compiler Explorer (g162, `-std=c++23 -O2`): https://godbolt.org/z/MPoEE7n8j

Copies are counted by a `Payload` whose copy constructor increments a counter. Allocations are counted by a
replaced `operator new`. All numbers were taken on Compiler Explorer on 2026-11-07.

## Copies per loop, 3 elements

| loop form | copies |
|---|---|
| `auto [id, p] : c` | 3 |
| `const auto [id, p] : c` | 3 |
| `auto entry : c` | 3 |
| `auto p : c \| std::views::values` | 3 |
| `const auto& [id, p] : c` | 0 |
| `auto&& [id, p] : c` (const or non-const map) | 0 |
| `auto& entry : c`, `auto&& entry : c` | 0 |
| `const auto& p : c \| std::views::values` | 0 |

The same on `std::map`, `std::unordered_map` and `std::vector<std::pair<int, Payload>>`, on g162, clang_trunk
(libstdc++) and MSVC (`vcpp_v19_latest_x64`).

## When the loop body changes the container

`MATRIX_CASE` takes three numbers. The callback returns a plan, and the loop applies it between reading the
element and using it.

| macro | values |
|---|---|
| `CONTAINER` | 0 `std::map`, 1 `std::unordered_map`, 2 `std::vector<std::pair<int, Payload>>` |
| `MODE` | 0 erase the current element (key 2), 1 erase the next one (key 2, while on key 1), 2 insert 64 keys while on key 1 |
| `FORM` | 0 `auto [id, p]`, 1 `const auto& [id, p]`, 2 keys then lookup with a held pointer, 3 snapshot of the container, 4 keys then lookup then copy the payload, 5 deferred queue, 6 `std::erase_if`, 7 `std::map` of `shared_ptr<const Payload>` with `auto [id, p]`, 8 `auto&& [id, p]` |

Forms 0, 1, 2, 7 and 8 can be undefined. Forms 3, 4, 5 and 6 stay defined in every case (run under ASan+UBSan and
under `_GLIBCXX_DEBUG`: all 72 runs completed, no diagnostic). Form 6 is only meaningful for mode 0. `std::extract`
was not run.

Configurations (Compiler Explorer): `g162 -std=c++23` with `-O0`, `-O2`, `-O1 -g -fsanitize=address,undefined`,
`-O0 -D_GLIBCXX_DEBUG`, `-O2 -D_GLIBCXX_ASSERTIONS`; `clang_trunk -std=c++23 -stdlib=libc++ -O1` plain, with
`-fsanitize=address,undefined`, and with `-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG`.
The compiler id and options are the whole recipe; a run is
`ce_run.py structured_binding_copies.cpp g162 "-std=c++23 -O2 -DMATRIX_CASE -DCONTAINER=0 -DMODE=0 -DFORM=0"`.

Outcome codes for the by-value form (`FORM=0`). `v=N` means the loop ran to the end and visited N elements.
A 3-element container visited 4 times, or 33 of 67, is undefined behavior that happened to finish.

By value, `FORM=0`:

| container, mode | g162 -O0 | g162 -O2 | g162 ASan+UBSan | g162 _GLIBCXX_DEBUG | libc++ -O1 | libc++ ASan | libc++ hardening |
|---|---|---|---|---|---|---|---|
| map, erase current | ran, 4 visited | ran, 4 visited | ran, 4 visited | debug: iterator check abort | SIGSEGV | ASan UAF | SIGSEGV |
| map, erase next | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited |
| map, insert 64 | ran, 67 visited | ran, 67 visited | ran, 67 visited | ran, 67 visited | ran, 67 visited | ran, 67 visited | ran, 67 visited |
| unordered_map, erase current | SIGSEGV | SIGSEGV | ASan UAF | debug: iterator check abort | SIGSEGV | ASan UAF | SIGSEGV |
| unordered_map, erase next | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited |
| unordered_map, insert 64 | ran, 33 visited | ran, 33 visited | ran, 33 visited | debug: iterator check abort | ran, 11 visited | ran, 11 visited | ran, 11 visited |
| vector<pair>, erase current | assert: front() empty | SIGSEGV | UBSan null ref | debug: iterator check abort | SIGSEGV | ASan container-overflow | hardening: front() empty |
| vector<pair>, erase next | assert: front() empty | SIGSEGV | UBSan null ref | debug: iterator check abort | SIGSEGV | ASan container-overflow | hardening: front() empty |
| vector<pair>, insert 64 | assert: front() empty | ran, 3 visited | ASan UAF | debug: iterator check abort | SIGSEGV | ASan UAF | hardening: front() empty |

By reference, `FORM=1` (`auto&&`, `FORM=8`, matched it in 35 of 36 runs):

| container, mode | g162 -O0 | g162 -O2 | g162 ASan+UBSan | g162 _GLIBCXX_DEBUG | libc++ -O1 | libc++ ASan | libc++ hardening |
|---|---|---|---|---|---|---|---|
| map, erase current | ran, 4 visited | ran, 4 visited | ASan UAF | debug: singular iterator | SIGSEGV | ASan UAF | hardening: front() empty |
| map, erase next | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited |
| map, insert 64 | ran, 67 visited | ran, 67 visited | ran, 67 visited | ran, 67 visited | ran, 67 visited | ran, 67 visited | ran, 67 visited |
| unordered_map, erase current | SIGSEGV | SIGSEGV | ASan UAF | debug: singular iterator | SIGSEGV | ASan UAF | hardening: front() empty |
| unordered_map, erase next | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited |
| unordered_map, insert 64 | ran, 33 visited | ran, 33 visited | ran, 33 visited | debug: singular iterator | ran, 11 visited | ran, 11 visited | ran, 11 visited |
| vector<pair>, erase current | assert: front() empty | SIGSEGV | UBSan null ref | debug: singular iterator | SIGSEGV | ASan container-overflow | hardening: front() empty |
| vector<pair>, erase next | assert: front() empty | SIGSEGV | UBSan null ref | debug: singular iterator | SIGSEGV | ASan container-overflow | hardening: front() empty |
| vector<pair>, insert 64 | SIGSEGV | SIGSEGV | ASan UAF | debug: front() empty | SIGSEGV | ASan UAF | SIGSEGV |

Keys first with a held pointer, `FORM=2`:

| container, mode | g162 -O0 | g162 -O2 | g162 ASan+UBSan | g162 _GLIBCXX_DEBUG | libc++ -O1 | libc++ ASan | libc++ hardening |
|---|---|---|---|---|---|---|---|
| map, erase current | ran, 3 visited | ran, 3 visited | ASan UAF | ran, 3 visited | ran, 3 visited | ASan UAF | hardening: front() empty |
| map, erase next | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited |
| map, insert 64 | ran, 3 visited | ran, 3 visited | ran, 3 visited | BUILDFAIL ? | ran, 3 visited | ran, 3 visited | ran, 3 visited |
| unordered_map, erase current | ran, 3 visited | ran, 3 visited | ASan UAF | ran, 3 visited | ran, 3 visited | ASan UAF | hardening: front() empty |
| unordered_map, erase next | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited |
| unordered_map, insert 64 | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited |
| vector<pair>, erase current | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited | ran, 3 visited |
| vector<pair>, erase next | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited | ran, 2 visited |
| vector<pair>, insert 64 | SIGSEGV | SIGSEGV | ASan UAF | debug: front() empty | SIGSEGV | ASan UAF | SIGSEGV |

GCC 16.2 enables the library assertions at `-O0` without any define, so the `-O0` vector aborts are
`front()` assertions in the loop body reading an element that is already gone. They are not an iterator check.
The same holds for the libc++ hardening column: its abort is `front()` on an emptied vector, and libc++
hardening has no iterator-invalidation check. Only `_GLIBCXX_DEBUG` checks the iterators.

Unordered containers visit in an implementation-defined order: with the same source, MSVC visits key 2 before
key 1, so the "erase next" strategies give different counts there. Read the `unordered_map` counts only for
libstdc++ (g162).

## Allocations (exact, 3 elements)

| strategy | `std::map` | `std::unordered_map` | `std::vector<pair>` |
|---|---|---|---|
| copy the container | 6 allocations, 264 B | 7 allocations, 296 B | 4 allocations, 168 B |
| vector of keys | 1 allocation, 12 B | 1 allocation, 12 B | 1 allocation, 12 B |

g162 and `-O0`/`-O2`. Bytes differ by library: the key vector is 16 B on gsnapshot and clang_trunk
(libstdc++), the map copy is 336 B on MSVC. A `std::map<int, std::shared_ptr<const Payload>>` walked with
`auto [id, p]`: 0 payload copies, 0 allocations, `use_count` 2 inside the loop and 1 outside.

An `unordered_map` of 3 elements with 64 inserts: `bucket_count` 13 before and 127 after on g162 (a rehash).

## Tools

| tool | `auto [id, p]` | `const auto [id, p]` | `auto entry` | mutating loop, form 0 |
|---|---|---|---|---|
| g162, gsnapshot `-Wall -Wextra` | silent | `-Wrange-loop-construct` | silent | silent |
| clang2310, clang_trunk `-Wall -Wextra` | silent | silent in these templates (see post) | silent | silent |
| clang-tidy trunk `performance-for-range-copy` | flagged | flagged | flagged | not flagged |
| MSVC `/W4 /analyze` | silent | silent | silent | silent |

Compile error with a move-only payload (`-DSHOW_ERROR`), `for (auto [id, p] : registry)`:
g162 `error: use of deleted function 'constexpr std::pair<_T1, _T2>::pair(const std::pair<_T1, _T2>&) [with _T1 = const int; _T2 = MoveOnlyPayload]'`;
clang_trunk `error: call to implicitly-deleted copy constructor of 'std::pair<const int, MoveOnlyPayload>'`.

Not run: Thread-safety (a reference to live data can still race), MSVC `/analyze:plugin EspXEngine.dll` (it
ran without output; we could not tell whether the plugin loaded).

## auto&&

Const map: 0 copies, `p` const. Non-const map: 0 copies, `p.tag += 1` changes the map. A `views::transform`
that returns pairs by value: 3 copies with `auto&& [id, p]` and 3 with `auto [id, p]`, all inside the lambda.
`for (auto&& f : std::vector<bool>) f = true;` sets every flag. g162 `-O0`/`-O2` and clang_trunk `-O0`.
`-Wrange-loop-construct` and clang-tidy `performance-for-range-copy` stay silent on `auto&&` loops.

Shared-pointer reentrancy (`FORM=7`, erase current, `std::map`): g162 ASan 4 visited with no report,
`_GLIBCXX_DEBUG` abort, libc++ ASan heap-use-after-free.

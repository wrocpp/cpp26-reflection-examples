# What a by-value std::generator enumerate costs

Companion to a wro.cpp short that starts from Alex Dathskovsky's LinkedIn post about replacing a
hand-written enumerate with C++23 `std::generator` (https://lnkd.in/p/d423e3Yz). These files are our own
version of the same shape: a coroutine that takes the container by value and yields an (index, value)
tuple. They are not Alex's code.

- `examples/enumerate.cpp`: the shape, runnable, four lines of output. Compiler Explorer (g162,
  `-std=c++23`): https://godbolt.org/z/WznrdPWaT
- `examples/alloc_count.cpp`: heap allocations and bytes for six ways of numbering a range, from a replaced
  global `operator new`. Compiler Explorer (g162, `-std=c++23 -O2`): https://godbolt.org/z/YfaPrsY4K
- `examples/time_compare.cpp`: per-element time of four of them. Not deterministic, so there is no embed.
- `examples/support.cpp`: the compiler, the standard library and the two feature-test macros. Needs only
  `<version>`, so it also compiles where `<generator>` does not exist.
- `examples/lifetime_probe.cpp`: counts live objects at the first resume of a generator, for a `const&`
  and a by-value parameter, with the temporary in a range-for and with the generator stored first.
- `examples/dangling.cpp`: the failing cases under AddressSanitizer, behind `-DSHOW_REF_IN_RANGE_FOR` and
  `-DSHOW_REF_STORED`. The default build runs clean.

Everything below ran on Compiler Explorer (x86-64) on 2 November 2026.

## Allocations (alloc_count.cpp, `-std=c++23`)

The counts were the same at -O0, -O1, -O2 and -O3 on GCC 16.2 and GCC trunk (17.0.0 20261102). They were
the same at -O1, -O2 and -O3 on Clang trunk (24.0.0git, libstdc++ 17) and Clang 23.1.0 (libstdc++ 15).
Each cell is allocations / bytes.

| variant | GCC, every level | Clang, -O0 | Clang, -O1 to -O3 |
|---|---|---|---|
| generator by value, `std::array<int, 4>` | 1 / 144 | 1 / 128 | 0 / 0 |
| `views::enumerate`, `std::array<int, 4>` | 0 / 0 | 0 / 0 | 0 / 0 |
| generator by value, vector of 1000 | 2 / 4152 | 2 / 4136 | 1 / 4000 |
| generator by `const&`, vector of 1000 | 1 / 136 | 1 / 120 | 0 / 0 |
| generic generator, vector copied in | 2 / 4168 | 2 / 4152 | 1 / 4000 |
| generic generator, `views::all(vector)` (borrows) | 1 / 152 | 1 / 136 | 0 / 0 |
| `views::enumerate`, vector of 1000 | 0 / 0 | 0 / 0 | 0 / 0 |
| index loop, vector of 1000 | 0 / 0 | 0 / 0 | 0 / 0 |

For the `std::array` the copy lives inside the coroutine frame, so there is no second allocation. The
4000 bytes are the vector's copy and the rest is the frame. On Clang the frame stops appearing in the
count from -O1, which fits frame elision. I read no assembly on either compiler.

### A counting trap

With plain `unsigned` counter reads, GCC 16.2 at -O1 and -O2 reported 3 allocations and 4216 bytes for
"generator by value, vector of 1000", where -O0 and -O3 reported 2 and 4152. Adding
`-fno-assume-sane-operators-new-delete` at -O2 gave 2 and 4152. The extra 64 bytes match the
`std::vector<Result>` built just before (2 x 32 bytes), so the "before" snapshot was taken too early.
GCC 15 added that flag and enabled it by default (https://gcc.gnu.org/gcc-15/changes.html). The example
reads its counters through a `[[gnu::noinline]]` function, and its output matches the flag run at every
level. Keep `-Wall` off the embed: at -O2 it prints `-Wmismatched-new-delete` pointing into `<generator>`,
which I did not investigate.

## Time (time_compare.cpp, `-std=c++23 -O2`)

One million `int`s. Each run is a separate Compiler Explorer execution with a different `-DSALT=n`, and
each number is the best of five passes inside that run, in nanoseconds per element. I made two trial runs
first, then these eight. None were dropped. The by-value column includes copying the 4 MB vector on every
pass. The generic column borrows through `views::all` and copies nothing, so it isolates the suspend and
resume cost.

GCC 16.2 (g162):

| run (SALT) | generator, by value | generic, `views::all` | `views::enumerate` | index loop | by value : enumerate | generic : enumerate |
|---|---|---|---|---|---|---|
| 11 | 3.73 | 2.85 | 0.480 | 0.495 | 7.8 | 5.9 |
| 12 | 6.46 | 5.90 | 0.595 | 0.710 | 10.9 | 9.9 |
| 13 | 3.14 | 2.76 | 0.466 | 0.476 | 6.7 | 5.9 |
| 14 | 3.18 | 2.73 | 0.453 | 0.476 | 7.0 | 6.0 |
| 15 | 3.18 | 2.75 | 0.476 | 0.476 | 6.7 | 5.8 |
| 16 | 3.31 | 2.73 | 0.453 | 0.479 | 7.3 | 6.0 |
| 17 | 3.19 | 2.76 | 0.480 | 0.501 | 6.6 | 5.7 |
| 18 | 4.12 | 2.94 | 0.522 | 0.480 | 7.9 | 5.6 |

Clang trunk (clang_trunk):

| run (SALT) | generator, by value | generic, `views::all` | `views::enumerate` | index loop | by value : enumerate | generic : enumerate |
|---|---|---|---|---|---|---|
| 11 | 6.17 | 4.90 | 0.456 | 0.373 | 13.5 | 10.8 |
| 12 | 2.92 | 2.66 | 0.312 | 0.303 | 9.4 | 8.5 |
| 13 | 3.66 | 2.71 | 0.325 | 0.304 | 11.3 | 8.3 |
| 14 | 2.95 | 2.64 | 0.307 | 0.302 | 9.6 | 8.6 |
| 15 | 3.05 | 2.74 | 0.310 | 0.304 | 9.9 | 8.8 |
| 16 | 3.70 | 3.09 | 0.310 | 0.304 | 11.9 | 10.0 |
| 17 | 2.87 | 2.63 | 0.308 | 0.302 | 9.3 | 8.5 |
| 18 | 6.49 | 5.33 | 0.350 | 0.305 | 18.5 | 15.2 |

Summary of the eight runs per compiler:

| ratio to `views::enumerate` | GCC 16.2 | Clang trunk |
|---|---|---|
| generator by value | 6.6 to 10.9, median 7.2 (7 of 8 runs from 6.6 to 7.9) | 9.3 to 18.5, median 10.6 (7 of 8 runs from 9.3 to 13.5) |
| generic, `views::all` | 5.6 to 9.9, median 5.9 (7 of 8 runs from 5.6 to 6.0) | 8.3 to 15.2, median 8.7 (7 of 8 runs from 8.3 to 10.8) |

The absolute numbers move by about a factor of two between runs on the shared machine. `views::enumerate`
and the index loop were within 0.03 ns of each other in six of eight runs on each compiler. The exceptions
are runs 12 and 18 on GCC and runs 11 and 18 on Clang. I did not read assembly, so there is no explanation
here for the gap between the generator and the other two.

## Library support (support.cpp)

| compiler | standard library | `__cpp_lib_generator` | `__cpp_lib_ranges_enumerate` | `<generator>` |
|---|---|---|---|---|
| GCC 16.2.0 (g162), `-std=c++23` | libstdc++ 16 | 202207L | 202302L | found |
| GCC trunk 17.0.0 20261102 (gsnapshot), `-std=c++23` | libstdc++ 17 | 202207L | 202302L | found |
| Clang trunk 24.0.0git (clang_trunk), `-std=c++23` | libstdc++ 17 | 202207L | 202302L | found |
| Clang trunk, `-std=c++23 -stdlib=libc++` | libc++ 240000 | undefined | 202302L | not found |
| Clang 23.1.0 (clang2310), `-std=c++23` | libstdc++ 15 | 202207L | 202302L | found |
| Clang 23.1.0, `-std=c++23 -stdlib=libc++` | libc++ 230100 | undefined | 202302L | not found |
| MSVC 19.51.36246 (vcpp_v19_latest_x64), `/std:c++latest` | MSVC STL 145 | 202207L | 202302L | found |

libc++'s own status page (https://libcxx.llvm.org/Status/Cxx23.html, read 2 November 2026) lists P2502R2
`std::generator` with no version and P2164R9 `views::enumerate` as complete in 23.

## Lifetime (lifetime_probe.cpp, dangling.cpp)

`lifetime_probe.cpp`, `-std=c++23` (`/std:c++latest` on MSVC). "Live probes at the first resume" for each
case, `const&` parameter first, by-value parameter second, as `range-for / stored`.

| compiler | `__cpp_range_based_for` | `const&`, range-for | `const&`, stored first | by value, range-for | by value, stored first |
|---|---|---|---|---|---|
| GCC 14.2 (g142) | 201603 | 0 | 0 | 1 | 1 |
| GCC 15.1 (g151) | 202211 | 1 | 0 | 2 | 1 |
| GCC 16.2 (g162) | 202211 | 1 | 0 | 2 | 1 |
| GCC trunk (gsnapshot) | 202211 | 1 | 0 | 2 | 1 |
| Clang 19.1.0 (clang1910) | 202211 | 1 | 0 | 1 | 1 |
| Clang 20.1.0 (clang2010) | 202211 | 1 | 0 | 1 | 1 |
| Clang trunk, Clang 23.1.0 | 202211 | 1 | 0 | 1 | 1 |
| MSVC 19.51.36246 | 202211 | 0 | 0 | 1 | 1 |

A zero means the temporary was already destroyed when the generator body started. Clang 18.1.0 on
Compiler Explorer has no `<generator>`, so it was not run. Clang's own status page
(https://clang.llvm.org/cxx_status.html) lists P2718R0 under Clang 20, while Clang 19.1.0 printed 1 here.
MSVC reports 202211 and printed 0 for the `const&` range-for case; I did not find out why. GCC's manual
describes `-frange-for-ext-temps`: with `-std=c++23` and above the extension is part of the language
(https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Dialect-Options.html).

`dangling.cpp` with `-std=c++23 -fsanitize=address -g`, exit code and first ASan line:

| compiler and flags | case | result |
|---|---|---|
| g162 | `-DSHOW_REF_IN_RANGE_FOR` | exit 0, prints `1 2 3` |
| g162 `-fno-range-for-ext-temps` | `-DSHOW_REF_IN_RANGE_FOR` | exit 1, `stack-use-after-scope`, read in `by_const_ref` |
| g162 | `-DSHOW_REF_STORED` | exit 1, `stack-use-after-scope`, read in `by_const_ref` |
| clang_trunk | `-DSHOW_REF_IN_RANGE_FOR` | exit 0, prints `1 2 3` |
| clang_trunk | `-DSHOW_REF_STORED` | exit 1, `stack-use-after-scope`, read in `by_const_ref` |
| g142 (GCC 14.2) | `-DSHOW_REF_IN_RANGE_FOR` | exit 1, `stack-use-after-scope`, read in `by_const_ref` |

Compiled with `-std=c++23 -Wall -Wextra` and both macros defined, no compiler warned about either
dangling call: GCC 16.2 at -O0 and -O2, GCC trunk at -O2, Clang trunk at -O0 and -O2, Clang 23.1.0 at
-O2. GCC 16.2 and GCC trunk at -O2 print `-Wmismatched-new-delete` for each generator function, pointing
at `std::__gen::_Promise_alloc<void>::operator delete`. The file has no replaced `operator new`, so the
warning is not caused by the counting harness. I did not investigate it.

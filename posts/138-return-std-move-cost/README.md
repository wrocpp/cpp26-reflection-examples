# What `return std::move(x)` costs, measured

Companion to the wro.cpp post "Moving a const object returns a copy: 163 page faults per call on a 400 KB vector".
Three programs, no macros, all exit 0 on GCC 16.2 (g162) and, for the first, Clang 22.1 (clang2210):

- `examples/return_shapes.cpp`: which constructor each return shape runs (static_asserts), then time and minor page faults per call.
  Compiler Explorer (g162, `-std=c++26 -O2`): https://godbolt.org/z/McYPK18h6
- `examples/by_value.cpp`: heap allocations per call for a struct with a `std::string`, by value and by `const&`, with a counting
  `operator new`, then nanoseconds per call. Compiler Explorer (g162, `-std=c++26 -O2`): https://godbolt.org/z/xdhjxrvdc
- `examples/lint_shapes.cpp`: seven ways to write `std::move` around a return, to see which tool reports what.
  Compiler Explorer (g162, `-std=c++26 -O2`, no warning flags): https://godbolt.org/z/Kjch9sMzE

Runs were made on Compiler Explorer on 2026-11-17 (the machine is shared; times vary, counts do not). Ranges below are the lowest and
highest value over the seven in-process runs of every execution listed. Each execution is one new process (a different program argument
defeats Compiler Explorer's result cache).

## return_shapes.cpp, g162 `-std=c++26 -O2`, 3 executions of the default mode

500 calls per run. Faults are minor page faults from `getrusage`, per call.

| shape | microseconds per call | faults per call | `-Wall -Wextra`, GCC 16.2 and Clang 22.1 |
|---|---|---|---|
| `std::array` 400 KB, `return book` | 6.0 to 10.9 | 0.00 to 0.20 | none |
| `std::array` 400 KB, `return std::move(book)` | 20.7 to 38.7 | 0.00 to 0.20 | `-Wpessimizing-move` on both |
| vector 400 KB, `return book` | 6.0 to 12.0 | 0.00 to 0.39 | none |
| vector 400 KB, `return std::move(book)` | 6.0 to 11.1 | 0.00 | `-Wpessimizing-move` on both |
| const vector 400 KB, `return std::move(book)` | 198 to 1354 | 162.87 to 163.00 | `-Wpessimizing-move` on both |
| const vector 100 KB, `return std::move(book)` | 3.9 to 6.7 | 0.00 to 0.03 | `-Wpessimizing-move` on both |
| 400 KB copied into a reused buffer | 8.6 to 11.9 | 0.00 | not a return |

Clang 22.1 (2 executions, same options): `std::array` plain 6.0 to 8.2, moved 14.1 to 15.0; vector plain 6.0 to 6.6, moved 6.0 to 6.1;
const vector 400 KB 194.5 to 1187.2 us with 162.87 to 163.00 faults; const 100 KB 3.9 to 4.0 us.

At run time: `return local` is built in place, `return std::move(local)` runs the move constructor, a const local with `std::move` runs
the copy constructor (g162 and clang2210). In constant evaluation GCC 16.2 runs the move constructor for the plain return, so that
`static_assert` only says it is not the copy constructor; elision of a named local is permitted, not required.

### Faults per call under four malloc settings (g162, `-std=c++26 -O2`)

| row | default (3 executions) | `heap` (2) | `fixed` (2) | `keep` (2) |
|---|---|---|---|---|
| vector 400 KB, plain or moved | 0.00 to 0.39 | 66.00 to 66.06 | 98.00 | 0.00 to 0.20 |
| const vector 400 KB, moved | 162.87 to 163.00 | 163.00 | 196.00 | 0.00 to 0.19 |
| const vector 100 KB, moved | 0.00 to 0.03 | 17.00 | 0.00 to 0.10 | 0.00 |

`heap` = `mallopt(M_MMAP_THRESHOLD, 16 MiB)`; `fixed` = `mallopt(M_TRIM_THRESHOLD, 16 MiB)`; `keep` = both. Setting any of them turns
off glibc's dynamic threshold (mallopt(3), https://man7.org/linux/man-pages/man3/mallopt.3.html). Times in `keep`: const vector 400 KB
14.8 to 20.9 us, plain vector 6.0 to 8.8 us. I did not trace system calls, so the default row does not say whether the memory returns
to the kernel by `munmap` or by trimming the heap.

## lint_shapes.cpp, `-std=c++26 -O2 -Wall -Wextra`, clang-tidy trunk on Compiler Explorer

| function | GCC 16.2 | Clang 22.1 | clang-tidy `performance-move-const-arg` |
|---|---|---|---|
| `local_moved` | `-Wpessimizing-move` | `-Wpessimizing-move` | silent |
| `const_local_moved` | `-Wpessimizing-move` | `-Wpessimizing-move` | fires |
| `const_local_stored` (`Levels out = std::move(book)`) | silent | silent | fires |
| `const_value_param` | `-Wredundant-move` | `-Wredundant-move` | fires |
| `const_ref_param` | `-Wredundant-move` | silent | fires |
| `Book::take_const` (`std::move(levels)` in a const member) | silent | silent | fires |
| `Book::take` | silent | silent | silent |

Clang with `-Weverything` (minus `-Wc++98-compat`, `-Wc++20-compat`, `-Wmissing-prototypes`, `-Wpadded`) adds only
`-Wpre-c++20-compat-pedantic` on the designated initialisers. GCC with `-Wpedantic -Wconversion -Wshadow -Wold-style-cast` adds nothing.
`performance-no-automatic-move` printed nothing for these shapes or for `const Levels book(...); return book;`.

## by_value.cpp, g162 `-std=c++26 -O2`

```text
length    by value    const&   sink value  sink const&
4                0         0            0            0
15               0         0            0            0
16               1         0            1            2
28               1         0            1            2
```

The sink columns pass a temporary to a function that stores it with `push_back`. Nanoseconds per call over 3 executions, 1,000,000
calls per run: by value 4.3 to 8.4 (4 characters) and 12.3 to 17.0 (28 characters); `const&` 2.6 to 4.1 and 2.6 to 3.6. The timing
includes the cost of the counting `operator new`. The 15-character boundary is libstdc++'s small-string buffer.

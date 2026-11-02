# Five spellings of a 64-byte compare

Companion to a wro.cpp post that starts from u/Sopel97's r/cpp thread
https://www.reddit.com/r/cpp/comments/1xlyti8/catastrophic_misoptimization_of_byterange/
(an early-exit loop over two `std::array<char, 64>` that GCC, Clang and MSVC do not turn into a clean
vector compare). Everything below was measured on Compiler Explorer (x86-64) on 2 November 2026.

- `examples/compare.cpp`: six functions (early exit, accumulate, `==`, `memcmp`, `std::equal`,
  `std::ranges::equal`) and a `main` that checks every one against 64 single-byte differences and one
  equal pair. It prints one line per function and exits 0.
  Compiler Explorer (g162, `-std=c++23 -O2`, runs): https://godbolt.org/z/rvTj4q5nz
- `examples/diagnose.cpp`: the early-exit loop and two controls that differ only in what the function
  returns. No `main`; it is for the dumps and remarks below.

Expected output of `compare.cpp` (identical at `-O0`, `-O2` and `-O3 -march=x86-64-v3` on g162, and at `-O2`
on gsnapshot, clang_trunk and clang 23.1):

```
eq_early_exit    equal=1 mismatches found=64 of 64
eq_accumulate    equal=1 mismatches found=64 of 64
eq_operator      equal=1 mismatches found=64 of 64
eq_memcmp        equal=1 mismatches found=64 of 64
eq_std_equal     equal=1 mismatches found=64 of 64
eq_ranges_equal  equal=1 mismatches found=64 of 64
```

## Assembly views

`-DCOMPARE_NO_MAIN` keeps `main` out of the assembly pane. Each link has the stated pair of options side by
side. The two trunk links compile whatever trunk is current when they are opened.

| compiler | version on 2 November 2026 | options | link |
|---|---|---|---|
| g162 | GCC 16.2.0, libstdc++ 16 | `-std=c++23 -O2` and `-O3 -march=x86-64-v3` | https://godbolt.org/z/cTs74GT73 |
| gsnapshot | GCC 17.0.0 20261102, libstdc++ 17 | same | https://godbolt.org/z/1P5ajfYzh |
| clang_trunk, clang2310 | Clang 24.0.0git dd61e451bba8, 23.1.0 ea7d852a70e8; both with libstdc++ (17 and 15 headers) | same | https://godbolt.org/z/Eve6x9fe1 |
| vcpp_v19_latest_x64 | MSVC 19.51.36246 (`_MSC_FULL_VER` 195136246), MSVC STL 145 update 202604 | `/O2 /std:c++latest` and `/O2 /std:c++latest /arch:AVX2` | https://godbolt.org/z/s5rYjaWj1 |

## Instruction counts

The count is the body of one function: no labels, no assembler directives, no comments, and (MSVC only) no
`npad` padding pseudo-instruction. `ret` and tail jumps count. Compiled with `-DCOMPARE_NO_MAIN`.
"v3" is `-O3 -march=x86-64-v3` (GCC, Clang) or `/arch:AVX2` (MSVC); the other column is `-O2` or `/O2`.
`==`, `memcmp`, `std::equal` and `std::ranges::equal` compile to the same instructions in all ten builds
(only label names differ), so they share a column.

| build | early exit | accumulate | `==` / `memcmp` / `std::equal` |
|---|---|---|---|
| GCC 16.2 | 12, scalar | 25, xmm loop | 30, scalar 8-byte words |
| GCC 16.2 v3 | 79, ymm plus scalar paths | 11, ymm | 18, ymm |
| GCC 17 snapshot | 12, scalar | 25, xmm loop | 30, scalar 8-byte words |
| GCC 17 snapshot v3 | 67, ymm plus scalar paths | 11, ymm | 18, ymm |
| Clang trunk | 13, scalar | 19, xmm | 7, call `bcmp` |
| Clang trunk v3 | 13, scalar | 9, ymm | 9, ymm |
| Clang 23.1 | 11, scalar | 19, xmm | 7, call `bcmp` |
| Clang 23.1 v3 | 11, scalar | 9, ymm | 9, ymm |
| MSVC | 11, scalar | 110, scalar | 7, call `memcmp` |
| MSVC AVX2 | 11, scalar | 110, scalar | 7, call `memcmp` |

"ymm plus scalar paths" means the vector loop is guarded by an alignment test and the function also holds a
scalar prologue, a scalar tail and a fully scalar loop for pointers that are not equally aligned.
The Clang rows were also built with `-stdlib=libc++` at both option sets: the same counts.

## Diagnostics (diagnose.cpp)

```
g++    -std=c++23 -O2 -fdump-tree-vect-details=stderr          # also -O3 -march=x86-64-v3, -fopt-info-vec-all
clang++ -std=c++23 -O2 -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize
clang++ -std=c++23 -O2 -mllvm -print-before=loop-vectorize \
        -mllvm -filter-print-funcs=_Z13eq_early_exitRKSt5arrayIcLm64EES2_
clang++ -std=c++23 -O2 -g0 -mllvm -print-changed=diff-quiet -mllvm -filter-print-funcs=<same name>
clang++ -std=c++23 -O2 -mllvm -enable-early-exit-vectorization=false <remark options>
```

Clang at `-O2`, trunk and 23.1:

| function | result type | vectorized |
|---|---|---|
| `eq_early_exit` | `bool`, `false` / `true` | no |
| `eq_early_exit_int` | `int`, `0` / `1` | yes (width 16; 32 at v3) |
| `first_mismatch` | `int`, `i` / 64 | yes (width 16; 32 at v3) |

Other `int` pairs tried in a throwaway file with the same loop: `i` / -1, `i` / 0, 7 / 9. All vectorize.
With `-mllvm -enable-early-exit-vectorization=false` the two controls stop vectorizing ("Auto-vectorization of
loops with uncountable early exit is not enabled") and the remarks for `eq_early_exit` do not change.

GCC treats the three functions alike: at `-O2` none vectorizes (12, 12 and 9 instructions on g162), and at v3 all
three vectorize behind alignment checks (79, 79 and 76 instructions on g162).

## Sources read

- LLVM `llvm/lib/Transforms/Vectorize/LoopVectorizationLegality.cpp` at dd61e451bba8b20ff019bffd054f7e3d989a3e7d:
  `isVectorizableEarlyExitLoop` (line 1709) reports "Latch block does not have a countable exit condition"
  at line 1776; lines 1862 to 1864 carry a TODO for read-only loops with combined exit conditions;
  "Cannot vectorize uncountable loop" is at line 2038.
- The same file at ea7d852a70e8bdfaf601d6626a760f9771b2c4b4 (23.1): line 1917 rejects any uncountable loop that
  has a single exiting block, before the early-exit code is tried.
- GCC `gcc/tree-vect-data-refs.cc` at 7eb405c3d119d4fc84b31f21d6499069fbfedad8: `vect_compute_data_ref_alignment`
  raises the required alignment for early-break loops (lines 1387 to 1414, the dump message "alignment increased
  due to early break to N bytes"); `vect_enhance_data_refs_alignment` relaxes it only for a non-constant
  vectorization factor (lines 2331 to 2353). `gcc/tree-if-conv.cc`, `idx_within_array_bound`, lines 794 to 796,
  refuses array references that might have a flexible size. I read these lines; I did not step through GCC.
- Reports, status as of 2 November 2026 (see the post): GCC PR 128384, GCC PR 120352, LLVM issue 182371.

## Not run

Rust, AArch64 or any non-x86-64 target, `-mavx512*`, MSVC with `/arch:AVX512`, any timing, MSVC execution of
the demo, and the collapsed replies of the Reddit thread. The LLVM flag `-enable-early-exit-vectorization` is
described here by its observed behaviour only: I did not find its definition in the LLVM source.

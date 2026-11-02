# Early exit and vectorization: cutting the loop into chunks

Companion to a wro.cpp post that follows `posts/102-byte-compare`. That post showed what GCC, Clang and MSVC make of
five spellings of a 64-byte compare; this one tries ways to keep an early exit and still get vector code. The loop
comes from u/Sopel97's r/cpp thread
https://www.reddit.com/r/cpp/comments/1xlyti8/catastrophic_misoptimization_of_byterange/.
Everything below was measured on Compiler Explorer (x86-64) on 2 November 2026.

- `examples/blocked_compare.cpp`: the early-exit loop, the accumulate loop (both as in `102-byte-compare`),
  `eq_chunked<8>`, `eq_chunked<16>`, `eq_chunked<32>` (an early exit between chunks, none inside one) and, with
  `-DUSE_STD_SIMD`, `eq_simd` (`std::simd` from libstdc++). A `main` checks every function against one equal pair
  and 64 single-byte changes, prints one line each and exits 0.
  Compiler Explorer (g162, `-std=c++26 -O2 -DUSE_STD_SIMD`, runs): https://godbolt.org/z/bfEq3q5Pb

Expected output (identical on g162 at `-O0`, `-O2`, `-O3 -march=x86-64-v3` and with `-fsanitize=undefined,address`,
on gsnapshot at `-O2`; the five functions without `eq_simd` on clang_trunk and clang 23.1 at `-O2`):

```
eq_early_exit    equal=1 mismatches found=64 of 64
eq_accumulate    equal=1 mismatches found=64 of 64
eq_chunked<8>    equal=1 mismatches found=64 of 64
eq_chunked<16>   equal=1 mismatches found=64 of 64
eq_chunked<32>   equal=1 mismatches found=64 of 64
eq_simd          equal=1 mismatches found=64 of 64
```

## Assembly views

`-DCOMPARE_NO_MAIN` keeps `main` out of the assembly pane. Each link has the stated options side by side. The
trunk links compile whatever trunk is current when they are opened.

| compiler | version on 2 November 2026 | options | link |
|---|---|---|---|
| g162 | GCC 16.2.0 | `-std=c++26 -DUSE_STD_SIMD` with `-O2` and with `-O3 -march=x86-64-v3` | https://godbolt.org/z/KoPPKeG94 |
| gsnapshot | GCC 17.0.0 20261102 | same | https://godbolt.org/z/nb8qE4M6E |
| clang_trunk, clang2310 | Clang 24.0.0git dd61e451bba8 (`-std=c++26`), 23.1.0 ea7d852a70e8 (`-std=c++23`) | `-O2` and `-O3 -march=x86-64-v3`, no `-DUSE_STD_SIMD` | https://godbolt.org/z/oM48Gh73K |

## Instruction counts

Counted as in `102-byte-compare`: the body of one function, no labels, directives or comments; `ret` and tail
jumps count. "v3" is `-O3 -march=x86-64-v3`, the other column is `-O2`. g162 and gsnapshot give the same counts for
every function here except the early-exit loop at v3 (79 and 67).

| function | g162 | g162 v3 | clang_trunk | clang_trunk v3 | clang 23.1 | clang 23.1 v3 |
|---|---|---|---|---|---|---|
| `eq_early_exit` | 12 | 79 | 13 | 13 | 11 | 11 |
| `eq_accumulate` | 25 | 11 | 19 | 9 | 19 | 9 |
| `eq_chunked<8>` | 24 | 99 | 10 | 9 | 10 | 9 |
| `eq_chunked<16>` | 26 | 19 | 27 | 23 | 27 | 23 |
| `eq_chunked<32>` | 47 | 12 | 23 | 13 | 23 | 13 |
| `eq_simd` | 19 | 10 | - | - | - | - |

What the instructions are (read from the assembly views above):

- GCC, `eq_chunked<16>` and `<32>` at v3: `vpxor` plus `vptest` on 16- or 32-byte registers, one test per chunk,
  unaligned loads, no alignment test. `<8>` at v3 does eight 8-byte steps with shifts
  (99 instructions). At `-O2` the 16- and 32-byte versions combine the bytes with shift-and-or sequences, not `pmovmskb`.
- Clang, `eq_chunked<16>`: `pcmpeqb` plus `pmovmskb` per chunk at `-O2`, `vpxor` plus `vptest` at v3.
  `eq_chunked<32>` tests two 16-byte halves at once at `-O2` and one `ymm` register at v3.
  `eq_chunked<8>` is a call to `memcmp` at `-O2`, and at v3 it has no early exit: two `ymm` loads, `vpor`,
  `vptest`. I did not find out why.

## GCC's own report for the chunk loops

`-fopt-info-vec-optimized` on g162 and gsnapshot (the file with `-DUSE_STD_SIMD -DCOMPARE_NO_MAIN`, same output on
both): at `-O2` the inner loop of `eq_chunked` (line 37, `diff |= ...`) is reported "loop vectorized" with 8-byte
vectors once and 16-byte vectors three times, once per instantiation. It is a counted loop with no exit, so the
early-break path is not involved. At v3 two of those inner loops are vectorized with 32-byte vectors and the
rest are reported as "basic block part vectorized" (8- and 16-byte vectors) on line 38. The only line that
reports "loop versioned for vectorization to enhance alignment" is the early-exit loop (line 17), at v3.

## std::simd

`<simd>` is present in g162 and gsnapshot with `-std=c++26` (with `-std=c++23` the include fails to build).
`__cpp_lib_simd` is not defined, on either. On clang_trunk (libstdc++ headers, `-std=c++26`) the include is accepted but
`std::simd` is not found ("no member named 'simd' in namespace 'std'"), so `eq_simd` is built for GCC only. I did not try it on
clang 23.1.

## Not run

Rust, AArch64 or any non-x86-64 target, `-mavx512*`, MSVC, any timing, `std::simd` on Clang with libc++, and a
version of the chunks written with intrinsics.

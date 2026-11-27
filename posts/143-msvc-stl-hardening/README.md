# MSVC STL hardening: an out-of-bounds read that stops, and the loop it costs

Companion to the wro.cpp news short "MSVC 14.52 is a long-term release, and the 14.53 preview fixes a hardening ticket" (1 December 2026).
`_MSVC_STL_HARDENING=1` adds a bounds check to `std::vector::operator[]`, `std::span::operator[]` and other accessors.
The examples contain no macros and read none: the switch `/D_MSVC_STL_HARDENING=1` appears only in the build line, as
`-D_GLIBCXX_ASSERTIONS` does for the hardened-stdlib example.

- `examples/oob_stop.cpp`: an out-of-bounds read on `std::vector` and on `std::span`, chosen by `argv`. The default run reads in
  bounds and exits 0. Compiler Explorer (`vcpp_v19_51_VS18_6_x64`, `/std:c++latest /EHsc /O2 /D_MSVC_STL_HARDENING=1`):
  https://godbolt.org/z/Px4P18r4z
- `examples/hot_loop.cpp`: a gather loop over `std::span::operator[]`, for reading the assembly. The link opens two
  compiler panes on the same file, hardened and plain (`vcpp_v19_51_VS18_6_x64`, `/std:c++latest /EHsc /O2` with and without
  `/D_MSVC_STL_HARDENING=1`): https://godbolt.org/z/39q46ns5a

Compiler: "x64 msvc v19.51 VS18.6", 14.51.36246, the newest MSVC on Compiler Explorer on 2026-11-28. It has no 14.52 or 14.53.
All runs were taken through the Compiler Explorer API on 2026-11-28.

## oob_stop.cpp

| options | argument | exit code | output |
|---|---|---|---|
| hardened | none | 0 | `vector[3] = 4`, `span[3] = 4` |
| hardened | `vector` | 3221226505 (0xC0000409) | `reading vector[4]`, then nothing |
| hardened | `span` | 3221226505 (0xC0000409) | `vector[3] = 4`, `reading span[4]`, then nothing |
| plain | `vector` | 0 | `vector[4] = 0` |
| plain | `span` | 0 | `span[4] = 0` |

The plain out-of-bounds reads are undefined behaviour. They printed 0 on these runs; another build may print anything.
0xC0000409 is the exception code that the `__fastfail` page of Microsoft Learn gives for a user-mode fast fail
(https://learn.microsoft.com/cpp/intrinsics/fastfail). The hardened build also compiles clean with `/W4`.

## hot_loop.cpp

Hardened, the loop body is:

```text
$LL4@sum_at:
        mov     rdx, QWORD PTR [rax]
        cmp     rdx, r10
        jae     SHORT $LN15@sum_at
        mov     rcx, QWORD PTR [r9]
        add     rax, 8
        add     r8d, DWORD PTR [rcx+rdx*4]
        cmp     rax, r11
        jne     SHORT $LL4@sum_at
$LN3@sum_at:
        mov     eax, r8d
        ret     0
$LN15@sum_at:
        mov     ecx, 5
        int     41                                        ; 00000029H
```

One element per iteration, one `cmp`/`jae` per element. The failure block sits after the `ret`, so the loop falls through on
the in-bounds path. `int 41` is `int 0x29`, the x64 fast-fail instruction on the `__fastfail` page, with code 5 in `ecx`.

Plain, the same function is unrolled four elements at a time and adds them in SSE registers (`movd`, `punpckldq`, `paddd`).
The listing has 25 lines that use an `xmm` register; the hardened one has none.

This is one function on one compiler. It does not reproduce the 14.53 ticket "MSVC fails to cold-path `_MSVC_STL_HARDENING`
bounds checks"; on this function 14.51 already places the failure after the return. No timings were taken.

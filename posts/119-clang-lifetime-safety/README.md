# Clang -Wlifetime-safety against GCC 16.2 on seven dangling cases

Companion to the wro.cpp short "Clang 23.1 -Wlifetime-safety against GCC 16.2". Eight small programs, one case each, no macros.
Every file compiles with every option set below and exits 0 when run without an argument. With the argument `bug` the
seven dangling cases run the dangling access (undefined behaviour); case 8 is a correct program that Clang flags.
The warning flags are compiler options, not source switches.

All runs: Compiler Explorer, 2026-11-17. Compilers: `clang2310` (Clang 23.1.0), `clang_trunk`, `g162` (GCC 16.2).
Sources: Clang 23.1.0 release notes (https://releases.llvm.org/23.1.0/tools/clang/docs/ReleaseNotes.html) and the Clang
LifetimeSafety page (https://clang.llvm.org/docs/LifetimeSafety.html).

Option sets: **A** `clang2310 -std=c++26 -Wall -Wextra`; **B** `clang2310 ... -Wlifetime-safety`; **C** `clang2310 ... -Wlifetime-safety-all`;
**T** `clang_trunk ... -Wlifetime-safety-all -Wlifetime-safety-experimental-invalidation`; **G** `g162 -std=c++26 -O2 -Wall -Wextra -Wdangling-pointer=2 -Wdangling-reference`.

| case | A (Clang, no lifetime flag) | B / C (Clang 23.1 lifetime-safety) | T (trunk) | G (GCC 16.2) |
|---|---|---|---|---|
| 1 `string_view` returned from a local `std::string` | `-Wreturn-stack-address` | adds `stack memory associated with local variable 'text' is returned [-Wlifetime-safety-return-stack-addr]` | same as B | silent |
| 2 `string_view` of a by-value parameter | `-Wreturn-stack-address` | adds `stack memory associated with parameter 'owner' is returned [-Wlifetime-safety-return-stack-addr]` | same as B | silent |
| 3 reference field bound to a temporary through a constructor | silent | C only: `parameter in intra-TU constructor should be marked [[clang::lifetimebound]] [-Wlifetime-safety-intra-tu-constructor-suggestions]` (a suggestion, not a dangling report) | same as C | `using a dangling pointer to an unnamed temporary [-Wdangling-pointer=]` and `may be used uninitialized [-Wmaybe-uninitialized]`, both located in `bits/basic_string.h:238` |
| 4 lambda capturing a local by reference, returned | silent | `stack memory associated with local variable 'local' is returned [-Wlifetime-safety-return-stack-addr]` | same as B | `'local' may be used uninitialized [-Wmaybe-uninitialized]` (O2 only, silent at O0) |
| 5 reference into a vector across `push_back` | silent | B and C: `local variable 'v' is later invalidated [-Wlifetime-safety-invalidation]` | silent with B or C; with the added flag: `... [-Wlifetime-safety-experimental-invalidation]` | silent |
| 6 `string_view` of a temporary | `object backing the pointer will be destroyed at the end of the full-expression [-Wdangling-gsl]` | adds `temporary object does not live long enough [-Wlifetime-safety-use-after-scope]` | same as B | silent |
| 7 `string_view` field set from a by-value parameter | silent | `stack memory associated with parameter 's' escapes to the field 'name' which will dangle [-Wlifetime-safety-dangling-field]` | same as B | silent |
| 8 correct: `reserve` then reference then `push_back` | not run | `local variable 'v' is later invalidated [-Wlifetime-safety-invalidation]` (false positive) | not run | silent |

`-Wlifetime-safety-invalidation` exists on `clang2310` and is unknown on `clang_trunk`; `-Wlifetime-safety-experimental-invalidation`
is the reverse (`unknown warning option ... did you mean '-Wlifetime-safety-invalidation'` on 23.1).

## Under AddressSanitizer (`-std=c++26 -O1 -g -fsanitize=address`, argument `bug`)

| case | g162 | clang2310 |
|---|---|---|
| 1 | `heap-use-after-free`, READ of size 1, exit 1 | `heap-use-after-free`, READ of size 1, exit 1 |
| 5 | `heap-use-after-free`, READ of size 4, exit 1 | `heap-use-after-free`, READ of size 4, exit 1 |

## Godbolt (g162, `-std=c++26 -O2 -Wall -Wextra -Wdangling-pointer=2 -Wdangling-reference`, safe path, exit 0)

Byte-compared with the files on 2026-11-17.

1. https://godbolt.org/z/nf1jEMb7T
2. https://godbolt.org/z/hKejx7nTK
3. https://godbolt.org/z/5b5zfP735
4. https://godbolt.org/z/ErezznhP6
5. https://godbolt.org/z/vEo7s4M4K
6. https://godbolt.org/z/TYG6q9YGz
7. https://godbolt.org/z/s87d1db3z
8. https://godbolt.org/z/5nK8aG11b

Not run: MSVC, `-Wlifetime-safety-strict` on cases 1 to 7, case 4 under AddressSanitizer.

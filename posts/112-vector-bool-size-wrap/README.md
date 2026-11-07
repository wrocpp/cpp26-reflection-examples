# A `vector<bool>` size that wraps in the word count

Companion to the wro.cpp post "A `vector<bool>` of `SIZE_MAX` bits builds on GCC 16.2 and throws on trunk"
(slug `vector-bool-size-wraps-to-zero`).
It is about libstdc++ CVE-2026-114691: some `std::vector<bool>` constructors passed a user-provided size to
the word-count function, whose addition wraps for sizes close to `SIZE_MAX`. The fix is libstdc++ commit
2fd8866591 (written 26 October 2026, committed to master on 29 October).

- `examples/vector_bool_size_wrap.cpp`: builds `std::vector<bool>(n)` for three sizes at and below
  `SIZE_MAX`, then a guarded builder that checks the size against `max_size()` first. The default build
  prints what each size does and exits 0; nothing reads an element. Compiler Explorer, four panes (GCC 16.2
  and GCC trunk, each default and with `-O1 -fsanitize=address -DTOUCH_ELEMENT`): https://godbolt.org/z/4PhW19ezd

Build it yourself with any GCC:

```
g++ -std=c++23 -O2 vector_bool_size_wrap.cpp
g++ -std=c++23 -O1 -fsanitize=address -DTOUCH_ELEMENT vector_bool_size_wrap.cpp
```

Use AddressSanitizer only together with `-DTOUCH_ELEMENT`. Without it the sanitizer's allocator aborts the run
at the third size (`SIZE_MAX - 63`), where the plain build throws `std::bad_alloc`. With the macro the program
stops at the first size, and the program asks ASan for exit code 0 so that Compiler Explorer shows the report.

## Measured on 6 November 2026 (Compiler Explorer, x86-64 Linux)

GCC 16.2.0 and GCC trunk `17.0.0 20261106`. The first three rows come from `vector_bool_size_wrap.cpp` and were
identical for `-std=c++23` and `-std=c++26` at `-O0` and `-O2`; the fourth row comes from a separate probe at
`-std=c++23 -O0`. Exit code 0 unless stated.

| size passed to `std::vector<bool>(n)` | GCC 16.2.0 | GCC trunk 20261106 |
|---|---|---|
| `SIZE_MAX` | constructed, `size()` = 18446744073709551615, `capacity()` = 0 | `std::length_error` |
| `SIZE_MAX - 62` | constructed, `size()` = 18446744073709551553, `capacity()` = 0 | `std::length_error` |
| `SIZE_MAX - 63` | `std::bad_alloc` | `std::length_error` |
| `max_size()` + 1 | `std::bad_alloc` | `std::length_error` |

With `-DTOUCH_ELEMENT`, reading element 0 of the vector GCC 16.2 accepted:

| build of GCC 16.2.0 | result |
|---|---|
| plain | prints 0, exit 0 |
| `-fsanitize=undefined` | prints 0, exit 0 |
| `-D_GLIBCXX_ASSERTIONS` | prints 0, exit 0 |
| `-fhardened` | prints 0, exit 0 |
| `-fsanitize=address` | `heap-buffer-overflow`, READ of size 8, 0 bytes after a 1-byte region |

`std::vector<int>(SIZE_MAX)` throws `std::length_error` ("cannot create std::vector larger than max_size()")
on both compilers.

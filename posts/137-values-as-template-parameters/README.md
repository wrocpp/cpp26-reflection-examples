# Values as template parameters, and the SQL name moved onto the enumerator

Companion to the wro.cpp short "The enumerator can carry its SQL name, and the build says which one is missing". It builds on a
LinkedIn post by Leonardo Rossi about values as template parameters. The examples contain no macros. All runs were made on
Compiler Explorer on 2026-11-17, and all four programs exit 0.

- `examples/chain.cpp`: the C++17 chain of `if constexpr` with a `static_assert` in the last branch. Compiler Explorer (g162,
  `-std=c++17`): https://godbolt.org/z/8ccohzqsP
- `examples/names.cpp`: the name as an annotation on the enumerator, a completeness and uniqueness check, a reflection as a template
  argument. Compiler Explorer (g162, `-std=c++26 -freflection`): https://godbolt.org/z/1YTaar6aW
- `examples/structural.cpp`: a class with public members against one with a private member, as template arguments. Compiler
  Explorer (g162, `-std=c++26`): https://godbolt.org/z/bMeW9W3v3
- `examples/getter.cpp`: a member function pointer as a template argument against a run-time argument, for the assembly view.
  Compiler Explorer (g162, `-std=c++26 -O2`, assembly pane): https://godbolt.org/z/onM8oG1jv

## Output

`chain.cpp` on g162 and clang2210 (`-std=c++17`): `name wavelength_nm intensity`.

`names.cpp` on g162 (`-std=c++26 -freflection`):

```text
name wavelength_nm intensity
problem(Draft) = Draft::Raw has no [[=sql{...}]] annotation
problem(Twice) = Twice::B repeats the SQL name 'x'
```

clang_bb_p2996 (`-std=c++26 -freflection-latest -stdlib=libc++`) does not build it: its library has no `annotations_of_with_type`
(error: no matching function for call to `annotation_of_type`).

`structural.cpp` on g162 and clang2210 (`-std=c++26`): `Open: 1, Hidden: 0`. `getter.cpp` on g162: `2.5 2.5`.

## Failing variants

Each is `names.cpp` or `chain.cpp` with one line appended, built with the options above and no other change.

| appended line | g162 says |
|---|---|
| `static_assert(sqlnames::problem<Draft>().empty(), sqlnames::problem<Draft>());` | `error: static assertion failed: Draft::Raw has no [[=sql{...}]] annotation` |
| `static_assert(sqlnames::problem<Twice>().empty(), sqlnames::problem<Twice>());` | `error: static assertion failed: Twice::B repeats the SQL name 'x'` |
| `static_assert(sqlnames::problem<Draft>().empty());` (no message) | `error: static assertion failed` |
| `constexpr auto bad = sqlnames::of_value<static_cast<Column>(7)>;` | `error: uncaught exception '(const char*)"this value is not an enumerator"'` |
| `constexpr auto bad = sqlnames::of_value<Draft::Raw>;` (no completeness check) | `error: call to non-'constexpr' function 'void std::__glibcxx_assert_fail(...)'` at `stl_vector.h:1253`, from indexing an empty vector |
| `chain.cpp` with a fourth enumerator `Raw` and `constexpr auto bad = sqlName<Column::Raw>();` | g162: `error: static assertion failed: Map the new Column here.`; clang2210: `error: static assertion failed due to requirement '(Column)3 == Column::Intensity': Map the new Column here.` |
| a class with a private member as `template <Pt P>` | g162: `'Pt' is not a valid type for a template non-type parameter because it is not structural` with the note `'Pt::v_' is not public`; clang2210: `type 'Pt' of non-type template parameter is not a structural type` |

## Assembly of getter.cpp at -O2

| compiler and options | `viaTemplate` (`readFrom<&Sensor::intensity>`) | `viaPointer` (pointer passed at run time) |
|---|---|---|
| g162 `-std=c++26 -O2` | `movsd xmm0, QWORD PTR [rdi+8]`, `ret` | `add`, `test sil, 1`, `je`, two loads, `jmp rsi` |
| clang2210 `-std=c++26 -O2` | `movsd xmm0, qword ptr [rdi + 8]`, `ret` | `add`, `test`, `je`, two loads, `jmp rsi` |
| vcpp_v19_latest_x64 `/std:c++latest /O2` | `movsd xmm0, QWORD PTR [rcx+8]`, `ret 0` | `rex_jmp rdx` |

`Sensor::intensity` is defined in the class, so its body is visible to the compiler. The listing says nothing about a getter
defined in another translation unit.

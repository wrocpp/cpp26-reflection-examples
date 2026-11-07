# When a std::generator reference parameter dangles

Companion to the wro.cpp short `generator-const-reference-lifetime`. It goes deeper on the lifetime
question the `generator-enumerate-by-value` short (examples in
`posts/101-generator-enumerate`, `lifetime_probe.cpp` and `dangling.cpp`) raised: a coroutine that takes
`const&` and is called with a temporary. The probe from 101 is reused as is and is not copied here.

- `examples/destruction_order.cpp`: a `Probe` that prints when it is constructed and destroyed, and a
  generator that prints how many probes are alive when its body first runs. Temporary in a range-for, and
  generator stored first. Compiler Explorer (g162, `-std=c++23`): https://godbolt.org/z/Kaahhv5Gv
- `examples/plain_function_control.cpp`: the same temporary passed to an ordinary function and to a
  coroutine, printing the probes alive in the loop body. Compiler Explorer (MSVC 19.51,
  `/std:c++latest`): https://godbolt.org/z/5ETxecdqb
- `examples/unsafe_signature.cpp`: a `const&` generator called two ways, behind `-DSHOW_RANGE_FOR` and
  `-DSHOW_STORED`. The default build runs clean. Used for the sanitizer, `-O2` and warning runs.
- `examples/signature_lint.cpp`: a reflection check that fails a `static_assert` naming a function that
  returns `std::generator` and takes a reference parameter. `-DSHOW_ERROR` adds one. Compiler Explorer
  (g162, `-std=c++26 -freflection`, default build passes): https://godbolt.org/z/PbGr9T94j

This is an example, not a library.

Everything ran on Compiler Explorer (x86-64) on 2 November 2026. Compiler ids and versions: g142 GCC 14.2.0,
g151 GCC 15.1.0, g162 GCC 16.2.0, gsnapshot GCC 17.0.0 20261102, clang1910 Clang 19.1.0, clang2010
Clang 20.1.0, clang2310 Clang 23.1.0, clang_trunk Clang 24.0.0git, vcpp_v19_latest_x64 MSVC 19.51.36246.
Options are `-std=c++23` (`/std:c++latest` on MSVC) unless a row says otherwise.

## Live probes at the first resume, and in the loop body (destruction_order.cpp)

| compiler | `__cpp_range_based_for` | temporary in the range-for: at first resume / in loop | generator stored first: at first resume / in loop |
|---|---|---|---|
| GCC 14.2 | 201603 | 0 / 0 | 0 / 0 |
| GCC 15.1 | 202211 | 1 / 1 | 0 / 0 |
| GCC 16.2 | 202211 | 1 / 1 | 0 / 0 |
| GCC 16.2 `-fno-range-for-ext-temps` | 201603 | 0 / 0 | 0 / 0 |
| GCC trunk | 202211 | 1 / 1 | 0 / 0 |
| Clang 19.1 | 202211 | 1 / 1 | 0 / 0 |
| Clang 20.1 | 202211 | 1 / 1 | 0 / 0 |
| Clang 23.1 | 202211 | 1 / 1 | 0 / 0 |
| Clang trunk | 202211 | 1 / 1 | 0 / 0 |
| MSVC 19.51 | 202211 | 0 / 0 | 0 / 0 |

Where the temporary lives on, "Probe destroyed" prints after "body resumes". Where it does not, it prints
before "body starts".

## MSVC control (plain_function_control.cpp)

Probes alive in the loop body for an ordinary function taking `const Probe&`, then a coroutine:
MSVC 19.51 `/std:c++latest` 0 and 0, `/std:c++23preview` 0 and 0. GCC 14.2 0 and 0. GCC 16.2 and Clang
trunk 1 and 1. `/std:c++20` does not build the file (it includes `<generator>`); I did not read why.

## Sanitizer and optimizer (unsafe_signature.cpp)

AddressSanitizer, `-std=c++23 -fsanitize=address -g`. Every failing row's first report line is
`ERROR: AddressSanitizer: stack-use-after-scope`, a read inside `numbers` (line 11).

| compiler and flags | case | exit | result |
|---|---|---|---|
| g162 | `-DSHOW_RANGE_FOR` | 0 | prints `10 20 30` |
| clang_trunk | `-DSHOW_RANGE_FOR` | 0 | prints `10 20 30` |
| g142 | `-DSHOW_RANGE_FOR` | 1 | stack-use-after-scope |
| g162 `-fno-range-for-ext-temps` | `-DSHOW_RANGE_FOR` | 1 | stack-use-after-scope |
| g162 | `-DSHOW_STORED` | 1 | stack-use-after-scope |
| clang_trunk | `-DSHOW_STORED` | 1 | stack-use-after-scope |

At `-O2` without a sanitizer all of these exit 0 and the dangling cases print different numbers each run
(for example `179973 0 1665218085`, `103640 0 310023509` on GCC trunk, `-1748022942 5 -1283950995` on Clang trunk).

## Warnings (unsafe_signature.cpp, both macros defined, compile only)

No diagnostic at all: GCC 14.2 `-O2`, GCC 16.2 `-O0` and `-O2` with `-Wall -Wextra`, GCC 16.2 `-O2` with
`-Wall -Wextra -Wpedantic -Wdangling-pointer=2 -Wdangling-reference`, GCC trunk `-O2`, Clang trunk `-O0`
and `-O2` with `-Wall -Wextra`, Clang trunk `-O2` with `-Wdangling -Wdangling-gsl -Wreturn-stack-address`,
Clang 23.1 `-O2`, MSVC `/W4` and `/W4 /analyze`. Clang trunk `-Weverything` gave two
`-Wmissing-prototypes` warnings, for `numbers` and `make_values`, and nothing about lifetime.
Not run: clang-tidy, cppcheck, any other static analyzer.

## The lint (signature_lint.cpp, `-std=c++26 -freflection`)

Default build: passes on g162 and gsnapshot. `-DSHOW_ERROR`: both fail with
`static assertion failed: generator function takes a reference parameter: by_reference`.
On Clang trunk the build fails with `no member named 'meta' in namespace 'std'` (the compiler reports `-freflection` as unused), so the lint was not run there.

Variants of the declaration inside `namespace api`, built on g162 with the same lint:

| declaration | flagged |
|---|---|
| `generator<int> f(const vector<int>&)` | yes |
| `generator<int> f(vector<int>&&)` | yes |
| `generator<char> f(std::string_view)` | no |
| `generator<int> f(std::span<const int>)` | no |
| `generator<int> f(const int*)` | no |
| a function template taking `const T&` | no |
| a member function of a struct in `api` | no |
| a lambda stored in a variable | no |
| the flagged function declared after the `static_assert` | no |

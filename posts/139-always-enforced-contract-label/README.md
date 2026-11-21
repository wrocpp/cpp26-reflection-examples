# The always-enforced contract label of P4451R2

Experiments behind the wro.cpp post `buzios-always-enforced-contract-label`. P4451R2 "Early Adoption of an Always-Enforced Label"
(Joshua Berne, 2026-11-19, http://isocpp.org/files/papers/P4451R2.pdf) was adopted at the Buzios meeting as motion CWG3,
https://github.com/cplusplus/draft/issues/9399. All runs below were taken on Compiler Explorer on 2026-11-21, one request at a time.
The examples contain no macros. Three files need a branch build and carry `// verify: ce-only` (`enforced_label.cpp`, `name_lookup.cpp`, `bare_name.cpp`); `unlabelled.cpp` is `gcc-only`.

| File | What it shows |
|---|---|
| `examples/enforced_label.cpp` | six assertions, three labelled with `std::contracts::controls::enforced`: which predicates run under `observe` and `ignore`, and which call terminates |
| `examples/unlabelled.cpp` | the same program without the labels, on a released compiler (GCC 16.2, `-fcontracts`) |
| `examples/name_lookup.cpp` | four spellings of the label: qualified name, namespace alias, using-directive, using-declaration |
| `examples/bare_name.cpp` | the spelling that does not compile (fails on purpose, so it is not a default build that exits 0) |

Compiler Explorer links (executor sessions, arguments pane visible, no argument is the run that exits 0):

| Program | Compiler id | Options | Link |
|---|---|---|---|
| `enforced_label.cpp` | `gcc_notadragon_contracts_p3850` | `-std=c++26 -fcontracts-p4451 -fcontract-evaluation-semantic=ignore` | https://godbolt.org/z/TW817e44P |
| `enforced_label.cpp` | `gcc_notadragon_contracts_p3850` | `-std=c++26 -fcontracts-p4451 -fcontract-evaluation-semantic=observe` | https://godbolt.org/z/Kahexdhae |
| `enforced_label.cpp` | `clang_notadragon_contracts_p3850` | `-std=c++26 -fcontracts-p4451 -fcontract-evaluation-semantic=ignore` | https://godbolt.org/z/Waz7b645T |
| `unlabelled.cpp` | `g162` | `-std=c++26 -fcontracts -fcontract-evaluation-semantic=ignore` | https://godbolt.org/z/jbYdP48Er |

Each link source was compared byte for byte with the file through `GET https://godbolt.org/api/shortlinkinfo/<id>`.

## enforced_label.cpp

`f` has three unlabelled and three labelled assertions. `check(name, holds)` prints `evaluated <name>` and returns `holds`, so the
output lists the predicates that ran. The argument selects the call: none is `f(51)`, which breaks the three unlabelled assertions
and no labelled one; `pre` is `f(-2)`, `assert` is `f(7)`, `post` is `f(21)` (its result 42 is the forbidden value). Each of the
last three breaks one labelled assertion.

Predicates evaluated before the program ended (`gcc_notadragon_contracts_p3850` and `clang_notadragon_contracts_p3850`, identical
counts and stdout on both, `-std=c++26 -fcontracts-p4451`):

| call | `observe` | `ignore` | exit, both semantics |
|---|---|---|---|
| none, `f(51)` | 6 | 3 | 0, prints `f(51) = 102` |
| `pre`, `f(-2)` | 2 | 1 | 134, labelled precondition |
| `assert`, `f(7)` | 4 | 2 | 134, labelled `contract_assert` |
| `post`, `f(21)` | 6 | 3 | 134, labelled postcondition |

Under `ignore` the three evaluated predicates of the default call are the labelled ones: `pre positive`, `assert not forbidden`,
`post not forbidden`. Under `observe` all six run and the three unlabelled violations are reported with `semantic: observe,
terminating: no`. The violated labelled assertion is reported by the GCC branch as `semantic: enforce, terminating: yes` in every
run, under `observe` and under `ignore` (GCC branch, `pre`, `ignore`):

    evaluated pre positive
    contract violation in function int {anonymous}::f(int) at /app/example.cpp:43: check("pre positive", x > 0)
    [assertion_kind: pre, semantic: enforce, mode: predicate_false, terminating: yes]
    Program terminated with signal SIGABRT (6)

The last line comes from the Compiler Explorer harness. The Clang branch prints
`/app/example.cpp:43: pre(check("pre positive", x > 0)) failed` instead of the three report lines.

A first run that sent several requests at once returned the `post` output for an `observe` run with argument `pre`. Every number
above is from the sequential rerun.

## unlabelled.cpp (g162)

`-std=c++26 -fcontracts` with three semantics, the four calls of the labelled program:

| semantic | none | `pre` | `assert` | `post` |
|---|---|---|---|---|
| `enforce` (default) | 134, 1 predicate | 134, 2 | 134, 1 | 134, 1 |
| `observe` | 0, 6 | 0, 6 | 0, 6 | 0, 6 |
| `ignore` | 0, 0 | 0, 0 | 0, 0 | 0, 0 |

Under `ignore` the program prints only the `f(...) = ...` line, so `f(-2)` returns -4 with no check: the build alone decides.

## Which compilers accept the label (`enforced_label.cpp`)

| compiler id | options | result |
|---|---|---|
| `gcc_notadragon_contracts_p3850` | `-std=c++26 -fcontracts-p4451` | accepted, see above |
| `clang_notadragon_contracts_p3850` | `-std=c++26 -fcontracts-p4451` | accepted, see above |
| `g162` | `-std=c++26 -fcontracts` | `error: expected '(' before '<' token` at the first label; the unlabelled file builds |
| `gsnapshot` | `-std=c++26 -fcontracts` | the same error |
| `clang_trunk` | `-std=c++26` | `error: expected function body after function declarator` at the first `pre` |
| `clang_trunk` | `-std=c++26 -fcontracts-p4451` | `clang++: error: unknown argument: '-fcontracts-p4451'` |
| `clang2310` | `-std=c++26` | `fatal error: 'contracts' file not found` |
| `vcpp_v19_51_VS18_6_x64` | `/std:c++latest /EHsc /Zc:__cplusplus` | `fatal error C1083: Cannot open include file: 'contracts'` |
| `edg-notadragon-contracts-p3850-gcc` | `--c++26 --contracts_p4451 --contract_evaluation_semantic=ignore` | `error: assertion-control-specifier must name std::contracts::labels::terminating; no declaration of it precedes this point` |
| `edg-notadragon-contracts-p3850-gcc` | `--c++26 --contracts_p3400 --contract_evaluation_semantic=ignore` | builds; no predicate runs and no call terminates, exit 0 for all four calls |

EDG options: `-std=c++26` is `eccp: unknown option`, and `--contract_evaluation_semantic=observe` is rejected
(`this configuration supports only the ignore, quick_enforce, ... semantics`). Without `--c++26` the header declares nothing and
`std::contracts::controls` is not a namespace. The paper's own EDG link for section 4 (https://godbolt.org/z/bP8a777rx) uses the
same flags in two panes. Its source, run through the API as published, gives the error above for `--contracts_p4451` and, for
`--contracts_p3400`, `checked(51) returned 102` and `checked(-2) returned -4`, where the link's comment expects a trap.

`name_lookup.cpp` on both branch compilers with `-std=c++26 -fcontracts-p4451`: four lines `qualified name: 1`, `namespace alias: 1`,
`using-directive: 1`, `using-declaration: 1`, exit 0. `bare_name.cpp` fails: GCC
`error: assertion-control-specifier must name 'std::contracts::controls::enforced' or 'std::contracts::controls::defaulted'`, Clang the
same text followed by `; name lookup finds nothing`. The diagnostics name a second label, `defaulted`, that the paper does not mention.

## Feature-test macros (probe, not in the examples: it needs `#ifdef`)

| compiler id | options | `__cpp_contracts` | `__cpp_lib_contracts` |
|---|---|---|---|
| `gcc_notadragon_contracts_p3850` | `-std=c++26 -fcontracts-p4451` | 202611 | 202611 |
| `gcc_notadragon_contracts_p3850` | `-std=c++26 -fcontracts-p3850` | 202611 | 202611 |
| `clang_notadragon_contracts_p3850` | `-std=c++26 -fcontracts-p4451` | 202611 | 202611 |
| `g162` | `-std=c++26 -fcontracts` | 202502 | 202502 |
| `gsnapshot` | `-std=c++26 -fcontracts` | 202502 | 202502 |

The paper's wording changes both macros from 202502L to 202611L.

## Not run

Redeclaration with and without the label (paper wording, 9.4.1 paragraph 5), a dependent label in a template, caller-side checking
(`-fcontracts-client-check=pre`, paper sections 2.8 and 4; a counting program gave 1 evaluation of a labelled precondition under
`ignore` and 2 under `observe`, which fits the allowed sequences of section 2.8, and it is not in this directory), run-time cost, and
MSVC and Clang 23.1 beyond the missing header.

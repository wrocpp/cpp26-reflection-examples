# Where a contract check stops being const

Experiments behind the wro.cpp post `contracts-check-that-acts`. It answers a LinkedIn post by Leonardo Rossi
about C++26 contracts: their rule is that a check never acts, and they ask whether to use contracts as they
are and whether to use the `ignore` semantic in a release build. The migration example is reworded (other names
and numbers) from theirs. Everything below ran on Compiler Explorer on 2026-11-06, x86-64,
GCC 16.2.0 (`g162`) and GCC trunk (`gsnapshot`, 17.0.0 20261106, commit 935a032c), `-std=c++26 -fcontracts`.
Each result was identical on both compilers at `-O0` and `-O2`, unless a row says otherwise.

| File | Question |
|---|---|
| `examples/migration.cpp` | What do enforce, observe, ignore and quick_enforce do to a skipped migration? |
| `examples/what_acts.cpp` | Which predicates can change state, and how often did the change happen? `-DROW=101..107` builds the ones that do not compile |
| `examples/by_value_post.cpp` | What does P2900R14 3.4.4 (value parameters named in a postcondition) look like on GCC? `-DSHOW_ERROR=1..5` |
| `examples/ignore_odr.cpp` | What does `ignore` leave in place? `-DNO_DEFINITION` removes a predicate's definition |
| `examples/hardening.cpp` | Does the contract semantic change libstdc++ hardening? `-DSHOW_ERROR` indexes past the end |

Compiler Explorer (executor, g162, `-std=c++26 -fcontracts -O2 -fcontract-evaluation-semantic=observe`):
`migration.cpp` https://godbolt.org/z/xqrM15j7r and `what_acts.cpp` https://godbolt.org/z/zd1cYMs3Y.

## Flag spelling (GCC 16.2, from `g++ --help=c++`)

`-fcontract-evaluation-semantic=[ignore|observe|enforce|quick_enforce]`, default enforce. Also present:
`-fcontracts-client-check=`, `-fcontracts-definition-check=`, `-fcontracts-conservative-ipa`.

## migration.cpp (revision 6, target 9)

| semantic | exit | output |
|---|---|---|
| enforce (default) | 134 (SIGABRT) | `start`; report on stderr; no more output |
| observe | 0 | `start`; `revision is now 9`; the report on stderr |
| ignore | 0 | `start`; `revision is now 9` |
| quick_enforce | 134 | `start` only, no report |

Enforce report (observe: `semantic: observe`, `terminating: no`, no terminate line):

    contract violation in function void {anonymous}::migrateTo(Registry&, uint32_t) at /app/example.cpp:28: target == registry.revision() + 1
    [assertion_kind: pre, semantic: enforce, mode: predicate_false, terminating: yes]
    terminate called without an active exception

`-DSHOW_ERROR` (a precondition that calls the non-const `advance()`):

    error: passing 'const {anonymous}::Registry' as 'this' argument discards qualifiers [-fpermissive]

## what_acts.cpp: the const-name matrix

P2900R14 section 3.4.2 against what GCC did. "Acts" means the counter rose by 1 for one call under enforce
and observe and stayed 0 under ignore, at `-O0` and `-O2`. Section 3.5.8 allows zero, one or many
evaluations, so the count is GCC's choice.

| Predicate | Paper | GCC 16.2 and trunk |
|---|---|---|
| non-const member on a reference parameter (row 101) | error | `error: passing 'const {anonymous}::Registry' as 'this' argument discards qualifiers [-fpermissive]` |
| non-const member on `*this` (row 102) | error | same message |
| `contract_assert` calling a non-const member on a local (row 106) | error | same message |
| assignment through a reference parameter (row 103) | error | `error: assignment of read-only location '(const int)reference'` |
| increment of a namespace-scope variable (row 104) | error | `error: increment of read-only location '(const int){anonymous}::g_counter'` |
| increment of a by-value parameter (row 105) | error | `error: increment of read-only location '(const int)value'` |
| non-const member through a pointer parameter | allowed | compiles, acts |
| `mutable` member named directly | allowed | compiles, acts |
| const member, `mutable` counter | allowed | compiles, acts |
| const member, static counter | allowed | compiles, acts |
| call to a function with a side effect | allowed | compiles, acts |
| `const_cast` on a global | allowed | compiles, acts |
| `const_cast` on a reference parameter | allowed | compiles, acts |
| lambda that increments a global | error (3.4.2 example) | compiles, acts (differs from the paper) |
| lambda with its own local | not const | compiles, no outside effect |
| lambda capturing a reference parameter (row 107) | allowed | `error: use of parameter outside function body before '.' token` (a GCC limitation) |

Row 101 with `-fpermissive`: the error becomes a warning (`warning: passing 'const ...' as 'this' argument
discards qualifiers [-fpermissive]`), the program runs, and `revision after one call` is 1 under enforce and
observe and 0 under ignore.

Three Clang forks on Compiler Explorer, `-std=c++26 -fcontracts -O2`, `what_acts.cpp`:

| Fork | Result |
|---|---|
| `clang_ericwf_contracts` | the whole table builds and runs, every acting row prints 1; row 104 (namespace-scope increment) also builds |
| `clang_cppa_p4324` | does not build: `cannot assign to variable 'g_counter' because it is considered 'const' inside of a contract` (line 44, the lambda row); rows 101, 103, 104, 105 are rejected with the same kind of message |
| `clang_notadragon_contracts_p3850` | the same as `clang_cppa_p4324` |

I did not check which design each fork implements.

## by_value_post.cpp (P2900R14 3.4.4)

| `-DSHOW_ERROR` | Source | GCC 16.2 and trunk |
|---|---|---|
| unset | `const` on both declarations | builds, `twice(4) = 8` |
| 1 | `int value` named in `post` | `error: a value parameter used in a postcondition must be const` |
| 2 | `const` on the first declaration, not on the definition | `error: value parameter 'value' used in a postcondition must be const` (note: previous declaration here) |
| 3 | `const` on both, a later redeclaration `int twice(int value);` | the same message |
| 4 | array parameter `const int values[]` | `error: a value parameter used in a postcondition must be const` |
| 5 | coroutine definition, `const int value` named in `post` | builds and runs (the paper calls it ill-formed) |

A redeclaration may not add a contract assertion to a function that had none
(`error: declaration adds contracts to 'int twice(int)'`), so "const only on the definition, no postcondition
on the first declaration" cannot be written.

## ignore_odr.cpp (what ignore keeps)

`isEven` is declared in the file; `-DNO_DEFINITION` removes the definition. `-O0` and `-O2`, both compilers:

| semantic | result |
|---|---|
| ignore | builds, links, `half(8) = 4` (GCC warns `used but never defined` only for the other three) |
| observe, enforce, quick_enforce | `undefined reference to '(anonymous namespace)::isEven(int)'` |

P2900R14 3.5.4 and 3.6.7 say the predicate is "potentially evaluated but not evaluated": its names are
odr-used. cppreference, `cpp/language/definition` (One Definition Rule), says a non-inline function that is
odr-used needs a definition and that the compiler is not required to diagnose a missing one. So the link
under ignore is GCC behaviour, not a guarantee. The post `contracts-that-prove-something` (line 62, "links
because the predicates are never evaluated") reports the GCC result correctly and gives a reason that the
standard does not support.

## hardening.cpp (libstdc++ 16.2)

With `-DSHOW_ERROR` and `-D_GLIBCXX_ASSERTIONS`, `-O2`, `vector<int>::operator[]` past the end:

    bits/stl_vector.h:1272: constexpr ... operator[](size_type) const : Assertion '__n < this->size()' failed.
    Program terminated with signal SIGABRT (6)

The same output (exit 134) under enforce, observe, ignore, quick_enforce, and without `-fcontracts`. Without
`-D_GLIBCXX_ASSERTIONS` at `-O2` the read printed `value 0` and exited 0; at `-O0` the assertion is on by
default (libstdc++ manual: "Defined by default when compiling with no optimization"). `-D_GLIBCXX_NO_ASSERTIONS`
at `-O0` turns it off. `-D_GLIBCXX_HARDEN=1|2|3` had no effect at `-O2`, and `-E -dM -O2 -D_GLIBCXX_HARDEN=3` on `<vector>` defines
no `_GLIBCXX_ASSERTIONS`: the 16.2 headers do not react to it. The preprocessed `<vector>` has 0 uses of `contract_assert` and 75 calls to `std::__glibcxx_assert_fail`.

P4433R0 note 6 (under P3878R1 a library is not hardened if it evaluates hardened preconditions with the
observe semantic) is a statement about the standard's library specification. It does not describe libstdc++
16.2, which does not express its assertions as contract assertions.

## A side result: contract_assert as the only statement of a loop body (GCC 16.2, -O0)

Run once, on `g162` and `gsnapshot`, `-std=c++26 -fcontracts -O0`, default (enforce) semantic, outside the
example files:

    int g = 0;
    int main() { for (int n = 0; n < 3; ++n) { contract_assert(g == 99); } std::puts("end"); }

GCC 16.2 printed `end` and exited 0, so the false, enforced assertion was not evaluated. GCC trunk terminated
with the violation report (exit 134). With one more statement in the loop body (`std::puts("iteration");`
before the assertion) 16.2 terminated too. I ran only `-O0` and did not look for the cause. It is not in the
post.

## Not verified

- Only GCC 16.2 and trunk on x86-64 Compiler Explorer; the Clang forks ran only `what_acts.cpp`.
- libc++ and the Microsoft library were not tested.

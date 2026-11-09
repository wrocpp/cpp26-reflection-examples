# Three nets under a forgotten clone() override

Companion to episode 3 of the wro.cpp series "clone() and smart pointers: from 2014 to C++26".
Each file takes the mixin of episode 2 and adds one net that catches a class which forgets its hook overrides.
`Tile : Square` overrides `copy_` and `move_`. `ForgetfulTile : Square` forgets both, and sits next to it in the same file.
There are no macros: each net is checked against `ForgetfulTile` with inverted assertions, so the file builds and exits 0
exactly when the net catches the forgetful class and accepts the correct ones.

- `examples/contract_assert_net.cpp`: a mixin that carries a `contract_assert` before the downcast, and a replacement contract
  violation handler that counts violations. Compiler Explorer (g162, `-std=c++26 -fcontracts -fcontract-evaluation-semantic=observe -O1`):
  https://godbolt.org/z/jfPbsaP5r
- `examples/constexpr_net.cpp`: `static_assert`s over a constant-evaluated round trip, and a concept that probes whether
  cloning through the derived type is a constant expression. Compiler Explorer (g162, `-std=c++26 -O1`): https://godbolt.org/z/ef3d1YxK7
- `examples/reflection_net.cpp`: reflection lists every concrete `Figure` in a namespace, reports the ones without overrides,
  and runs a clone round trip on each. Compiler Explorer (g162, `-std=c++26 -freflection -O1`): https://godbolt.org/z/sGnT3E6os

All results were taken on Compiler Explorer on 2026-11-09.

## Net 1: contract_assert (g162)

The predicate is `typeid(made_ref) == typeid(self)`: the object the hook made has the dynamic type of the source. It sits in
`clone()` because GCC 16.2 rejects a contract on a virtual function (`contracts cannot be added to virtual functions`), and before the
`static_cast`, because a postcondition on `clone()` would run after the cast. The program clones a `Square`, a `Tile`, and a
`ForgetfulTile` once through its own type and once through a `Figure&`, then prints the violations the handler counted.

| `-fcontract-evaluation-semantic=` | exit | what is printed |
|---|---|---|
| `observe` | 0 | two lines `contract violation: typeid(made_ref) == typeid(self)`, then `violations recorded: 2` |
| `ignore` | 0 | `violations recorded: 0` |
| `ignore` with `-fsanitize=undefined` | 0 | `violations recorded: 0`, and UBSan: `downcast of address ... which does not point to an object of type 'ForgetfulTile'` |
| `enforce` | 134, SIGABRT | the handler runs, then the process terminates |
| `quick_enforce` | 134, SIGABRT | no handler call, no message |
| none of `-fcontracts` and the flag | 134, SIGABRT | the file defines the handler, so it links; the default semantic is `enforce` |

Without a replacement handler and without `-fcontracts`, GCC 16.2 compiles `contract_assert` but fails to link, with
`undefined reference to handle_contract_violation`. Clang 22.1.0 and clang trunk do not define `__cpp_contracts`
(a probe printed `__cpp_contracts is not defined`) and have no `contract_assert`, so this file is for GCC.

## Net 2: constexpr

`clone_keeps_type<T>()` default-constructs a `T`, clones it through a `const Figure&`, and compares the two dynamic types.

- `static_assert(clone_keeps_type<Square>())` and `<Tile>` pass, and `static_assert(!clone_keeps_type<ForgetfulTile>())` passes.
- The concept `clones_in_constant_evaluation<T>` puts `T{}.clone()->area()` in a template argument. It is true for `Tile` and
  false for `ForgetfulTile`, because the compiler rejects the undefined downcast during constant evaluation. Both
  `static_assert`s on it pass.

The file builds and runs on g162 and clang2210 (`area 1`).

## Net 3: reflection (g162 `-freflection`)

`concrete_figures` walks `members_of(^^shapes)` and keeps classes that are non-abstract and derive from `shapes::Figure`.
`lacking_hooks` keeps those that do not declare an override of both `copy_` and `move_`. Two `static_assert`s check that the
lint finds exactly `ForgetfulTile`. `template for` then runs the round trip of net 2 on each class.

```text
lint: 1 of 3 concrete figures lack an override
  ForgetfulTile does not override copy_ and move_
test: Square clone keeps its type: true
test: Tile clone keeps its type: true
test: ForgetfulTile clone keeps its type: false
```

The program returns the number of results that differ from what the lint predicts, so it exits 0. g162 and clang_bb_p2996
(`-std=c++26 -freflection-latest -stdlib=libc++`) print the same lines.

Limits: the lint sees one namespace of one translation unit, and a generated test needs a default constructor.

# A clone() with deducing this, as a mixin

Companion to episode 2 of the wro.cpp series "clone() and smart pointers: from 2014 to C++26".
`Cloneable` is a mixin with no template parameters. One member function template with an explicit object parameter returns
`std::unique_ptr<Derived>`, and replaces `object::clone`, the `base_type` alias, the friend template and the `cloneable<T>`
base of the 2014 article. Two private virtual hooks return `std::unique_ptr<Cloneable>`, so they are not covariant and no raw
owning pointer crosses a virtual call. The examples contain no macros.

- `examples/deducing_this_clone.cpp`: the mixin, its static types, the move from an rvalue, and the assignment rules.
  Compiler Explorer (g162, `-std=c++26 -O1 -Wall -Wextra -Wpedantic`): https://godbolt.org/z/Wfzb9afYW.
  Clang 22.1.0 builds and runs it with the same output.
- `examples/covariant_hook.cpp`: GCC PR126324, a covariant hook in constant evaluation. It is the other mixin: a raw pointer
  hook, `Square* copy() const override`. Compiler Explorer (gsnapshot, `-std=c++26 -O1`): https://godbolt.org/z/64G9KT8fY

## The default program (g162 and clang2210, identical)

```text
history left in the donor after clone of an rvalue: 0
history left in the const source after clone of a const rvalue: 1
areas 16 16 16 16
assigned area 16, row size 2
```

| call | static type of the result | hook that runs |
|---|---|---|
| `square.clone()` | `std::unique_ptr<Square>` | `copy_` |
| `figure.clone()`, a `Figure&` bound to a `Square` | `std::unique_ptr<Figure>` | `copy_` |
| `frozen.clone()`, a `const Square` | `std::unique_ptr<Square>` | `copy_` |
| `Square{...}.clone()` | `std::unique_ptr<Square>` | `move_` |
| `std::move(donor).clone()` | `std::unique_ptr<Square>` | `move_`; the donor's `history` is empty afterwards on libstdc++ |
| `std::move(frozen).clone()`, a const rvalue | `std::unique_ptr<Square>` | `copy_`; the source keeps its `history` |

A moved-from `std::vector` is valid but unspecified by the standard. Empty is what libstdc++ leaves.

## Cases that must not compile, without a macro

The file states them as `static_assert`s on type traits, so a passing build proves they are rejected:

- `!std::is_assignable_v<Figure&, const Figure&>`: the base's copy assignment is protected and defaulted.
  `Figure` re-declares the operations, because the implicit ones of a derived class are public.
- `std::is_copy_assignable_v<Square>`: a derived class stays assignable.
- `!std::is_copy_assignable_v<DeletedDerived>`: a base that declares `operator=(const DeletedBase&) = delete("assigning
  through a base slices; use clone()")` makes the assignment of every derived class deleted.

## GCC PR126324 (`covariant_hook.cpp`)

The file calls a covariant hook through a `Figure&` inside a `static_assert` and stores the result in a `Figure*`.
The unique-pointer hook of `deducing_this_clone.cpp` has a constant-evaluated `static_assert` too, and it builds on g162 and clang2210.

| compiler | `covariant_hook.cpp` |
|---|---|
| g131 to g134 (GCC 13.1 to 13.4), `-std=c++23` | internal compiler error in `cxx_eval_store_expression` (`constexpr.cc:6156` to `6197`) |
| g141 to g144 (GCC 14.1 to 14.4), `-std=c++23` | internal compiler error in `cxx_eval_store_expression` (`constexpr.cc:6579` to `6674`) |
| g151 (GCC 15.1) | internal compiler error, `constexpr.cc:6800` |
| g161 (GCC 16.1) | internal compiler error, `constexpr.cc:8256` |
| g162 (GCC 16.2) | internal compiler error, `constexpr.cc:8265` |
| gsnapshot (trunk, 2026-11-09) | builds and runs, exit 0 |

An earlier version of this file passed the call result straight to a `std::unique_ptr` constructor instead of storing it in a
`Figure*`. It compiled on g151, g161, g162 and gsnapshot. The Bugzilla entry lists 13.1.0 as failing and 12.5.0 as working.
All results were taken on Compiler Explorer on 2026-11-09.

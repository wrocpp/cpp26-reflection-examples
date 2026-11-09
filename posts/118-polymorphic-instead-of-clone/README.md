# std::polymorphic and a hookless mixin instead of clone() hooks

Companion to episode 4 of the wro.cpp series "clone() and smart pointers: from 2014 to C++26".
Two designs copy a polymorphic object with no clone code in the classes: `std::polymorphic<Figure>`, and `AutoCloneable`, a
mixin with no template parameters whose deducing-this `clone()` uses reflection to find the copy to make. The examples contain
no macros.

- `examples/polymorphic_instead_of_clone.cpp`: copies, size, the valueless state after a move, and the conversion that does not
  exist. Compiler Explorer (g162, `-std=c++26 -O1`): https://godbolt.org/z/vo61oq7sr
- `examples/reflected_mixin.cpp`: the hookless mixin. Compiler Explorer (g162, `-std=c++26 -freflection -O1`): https://godbolt.org/z/Ybrjx5n3M
- `examples/copy_cost.cpp`: allocations and element size for four designs, counted by a replaced `operator new`.
  Compiler Explorer (g162, `-std=c++26 -freflection -O1`): https://godbolt.org/z/vhcx9Tr5c

All results were taken on Compiler Explorer on 2026-11-09. All three programs exit 0.

## polymorphic_instead_of_clone.cpp

| compiler | result |
|---|---|
| g162 and gsnapshot (libstdc++) | `copies with the same dynamic type: 3 of 3, distinct objects: 3 of 3`; `sizeof(std::polymorphic<Figure>) = 8, sizeof(std::unique_ptr<Figure>) = 8`; `source valueless after the move: yes, the moved-to value holds area 4` |
| clang_trunk with `-stdlib=libc++` | does not compile: `no template named 'polymorphic' in namespace 'std'; did you mean 'is_polymorphic'?` |

The 8 bytes are libstdc++ 16.2's layout, not a requirement of the standard. A `static_assert` copies a `std::polymorphic<Figure>`
that holds a `Tile` in constant evaluation. Two more state facts as type traits:
`!std::is_convertible_v<std::polymorphic<Square>, std::polymorphic<Figure>>` and
`std::is_convertible_v<std::unique_ptr<Square>, std::unique_ptr<Figure>>`.

## reflected_mixin.cpp

```text
dynamic types kept: 1 1 1
history left in the donor after clone of an rvalue: 0
clone of a class outside the namespace throws clone_unavailable: no searched class has the dynamic type N9elsewhere5StrayE
```

`tile.clone()` is a `std::unique_ptr<Tile>`, `through_base.clone()` a `std::unique_ptr<Figure>` that holds a `Tile`, and
`std::move(tile).clone()` moves. The set of classes is those of one namespace that are complete where `clone()` is
instantiated: `elsewhere::Stray`, derived from `shapes::Figure` in another namespace, makes `clone()` throw `clone_unavailable`,
a `std::logic_error` whose message names the dynamic type. The failure is loud, where an empty pointer would be silent. Builds on g162 and
clang_bb_p2996 with the same output.

## copy_cost.cpp

A row of three figures (`Square`, `Circle`, `Tile`, default-constructed so none owns a heap buffer) is copied once. The variant is
built from the namespace by `std::meta::substitute(^^std::variant, ...)`.

| design | allocations per row copy | element size |
|---|---|---|
| `vector<unique_ptr<Figure>>`, hand-written hooks | 4 | 8 bytes |
| `vector<unique_ptr<Figure>>`, hookless reflection mixin | 4 | 8 bytes |
| `vector<polymorphic<Figure>>` | 4 | 8 bytes |
| `vector<variant<Square, Circle, Tile>>` | 1 | 56 bytes |

The same numbers come out on g162 at `-O1` and `-O2` and on gsnapshot at `-O1`. The file needs reflection and `std::polymorphic`,
so clang_bb_p2996 does not build it. With `-Wall`, GCC 16.2 reports `-Wmismatched-new-delete` on the replaced operators although
the pair is matched, so the link above has no `-Wall`. The four allocations are the vector's buffer plus one per figure.

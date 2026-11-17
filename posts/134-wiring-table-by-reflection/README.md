# A signal-to-slot wiring table that sits on the slots

Companion to the wro.cpp post on wiring tables read from annotations. It builds on a design from a LinkedIn post by
Leonardo Rossi: a class lists its connections once, in a `static constexpr auto wiring()` that returns a tuple of pairs of
member pointers, and a `toggleWiring<bool Enable>` connects or disconnects all of them with `std::apply`. The question here
is whether the table can sit on the slots themselves, as annotations read by reflection.

This is not Qt. The file has a small `Signal<Args...>` of its own (a vector of connections) and Qt's `connect` was not run.

- `examples/wiring_table.cpp`: both forms next to each other, over the same signal type. Compiler Explorer (g162,
  `-std=c++26 -freflection -Wall -Wextra`): https://godbolt.org/z/1ffczz9z1

All runs were taken on Compiler Explorer on 2026-11-17. The program has no macros and exits 0.

## Output

```text
before connect: tuple=0 annotated=0
connections: failed=4 timedOut=3 succeeded=2
after connect: tuple=1/1/1 annotated=1/1/1 fanout=3
after disconnect: connections=0 tuple=1/1/1 annotated=1/1/1 fanout=3
```

`failed` has four connections because the tuple form, the annotated form and the two slots of `Fanout` all listen to it.
The same output, built without warnings, came from `g162`, `gsnapshot` and `clang_bb_p2996` (options
`-std=c++26 -freflection-latest -stdlib=libc++ -Wall -Wextra` on the fork). One source serves all three: the loop over
`annotations_of` that compares `remove_const(type_of(annotation))` with `^^on` works on both. With `type_of(annotation) ==
^^on` alone, GCC 16.2 finds no annotation (the table had size 0) and the fork finds them, so the type of an annotation
carries `const` on GCC 16.2 (the table size was the observed symptom; the `const` itself was not printed).
`std::meta::annotations_of_with_type` exists on GCC 16.2 and does not exist on the fork
(`no member named 'annotations_of_with_type' in namespace 'std::meta'; did you mean 'annotation_of_type'?`).

## Size of the two forms

Non-blank lines in the file: the tuple form (`wiring()` and `toggleWiring`) is 17. The annotated form is 59 lines for the
reusable part (`on`, `Edge`, `edges_of`, `table_of`, the checks, `toggleWiring`), of which 24 are checks that the tuple
form does not have. Adding a slot costs one line in the tuple and zero extra lines with annotations (the annotation sits
on the slot). The tuple form needs no checks because the member pointers are typed.

## Mistakes, with the compiler output

The annotated form checks its table with `static_assert(table_is_valid<Owner>(), "...")` and the unit `static_assert`s on
the table size. Variants of `Procedure` were compiled with the options above.

| variant | g162 | clang_bb_p2996 |
|---|---|---|
| slot takes `double`, signal is `Signal<int>` | `error: static assertion failed: a wiring annotation names something that is not a signal of Step, repeats an edge, or its slot takes other argument types than the signal` | `error: static assertion failed due to requirement 'table_is_valid<wiring::Procedure>()': a wiring annotation names ...` |
| annotation names the type `Step` | same message | same message |
| annotation names `Procedure::failures`, a member of another class | same message | same message |
| the same edge twice on one slot | `static assertion failed` with `the comparison reduces to '(4 == 3)'` on the size check, and the same message | `expression evaluates to '4 == 3'`, and the same message |
| a typo, `^^Step::failled` | `error: 'wiring::Step::failled' has not been declared` | `error: use of undeclared identifier 'failled'` |

The message does not say which slot. With the `static_assert` on `table_is_valid` removed, the slot of the wrong type gives:

```text
g162:   error: no matching function for call to 'wiring::Signal<int>::connect(wiring::Procedure*, void (wiring::Procedure::*)(double))'
clang_bb_p2996:   error: no matching member function for call to 'connect'
```

One signal on two slots (`Fanout::onEither` and `Fanout::onFailed` both listen to `failed`) is two connections. The table has
3 edges, `static_assert`ed. The same signal on the same slot twice is rejected.

## Not measured

Compile time, Qt's `connect` and `QMetaObject`, a class hierarchy (a slot declared in a base class), and a signal declared
in a different class from `Step`.

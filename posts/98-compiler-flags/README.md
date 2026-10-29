# Compiler flags: a flag that only moves code, and what -DNDEBUG removes

Companion to two wro.cpp posts. Part 1 is about `-Wa,-mbranches-within-32B-boundaries`, part 2 about
`-DNDEBUG` and `contract_assert`.

- `examples/jcc_boundary.cpp`: one scan loop emitted at 32 offsets modulo 32 (x86-64). Compiler Explorer
  permalink with the flag on: https://godbolt.org/z/vTMnYcP9d
- `examples/count_straddling_pairs.py`: compiles it on Compiler Explorer (g162) with and without the flag and
  counts adjacent cmp/test + conditional-jump pairs that cross or end on a 32-byte boundary.

## Measured (GCC 16.2, x86-64, `-O2`)

| build | cmp/test + jcc pairs | crossing or ending on a 32-byte boundary |
|---|---|---|
| default | 96 | 5 |
| `-Wa,-mbranches-within-32B-boundaries` | 96 | 0 |

All five are function-entry `test rsi,rsi` / `jg` pairs. The in-loop pairs never straddled: at `-O2` GCC
aligns loop heads to 16 bytes, which cancels the padding. So this shows what the flag does to layout, and
it does not reproduce the loop crossing found in reflect-moc (docs/measurements.md there). It does not
measure time either.

## Part 2: `-DNDEBUG` against `contract_assert` (x86-64 GCC 16.2, Compiler Explorer g162)

`examples/ndebug_contract.cpp` has one `assert` and one `contract_assert`, both false at run time.
Permalink (observe semantic, exits 0): https://godbolt.org/z/3W5TMj4sq

| flags | `assert(x != 0)` | `contract_assert(x != 0)` | exit |
|---|---|---|---|
| `-fcontracts -O2` | aborts | not reached | 134 |
| `-fcontracts -O2 -DNDEBUG` | removed | violation, enforce, terminates | 134 |
| `-fcontracts -O3 -flto -DNDEBUG` | removed | violation, enforce, terminates | 134 |
| `-fcontracts -O2 -DNDEBUG -fcontract-evaluation-semantic=ignore` | removed | not evaluated | 0 |
| `-fcontracts -O2 -DNDEBUG -fcontract-evaluation-semantic=observe` | removed | violation reported, continues | 0 |
| `-O2 -DNDEBUG` (no `-fcontracts`) | removed | link error: undefined reference to `handle_contract_violation(std::contracts::contract_violation const&)` | build fails |

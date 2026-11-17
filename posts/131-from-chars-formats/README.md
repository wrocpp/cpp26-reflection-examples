# What std::from_chars does with each chars_format

Companion to a wro.cpp short about `std::from_chars` for floating point. For fourteen inputs and the four `std::chars_format`
values (`general`, `fixed`, `scientific`, `hex`) the program prints how many characters are consumed, the `errc` and the value
left in the variable. The expected results sit in a `constexpr` table; the program exits 0 only when every cell matches. The
example contains no macros.

- `examples/from_chars_formats.cpp`. Compiler Explorer (g162, `-std=c++26 -O2`): https://godbolt.org/z/cenobP93f

All results were taken on Compiler Explorer on 2026-11-17. The program exits 0 on all three configurations below.

| configuration | result |
|---|---|
| g162 (libstdc++) | table below, exit 0 |
| clang2210 with libstdc++ | identical to g162, exit 0 |
| clang2210 with `-stdlib=libc++` | identical except the value left after `result_out_of_range`, exit 0 |

## Output (g162, `-std=c++26 -O2`)

Each cell is `format letter:consumed/errc/value`. `inv` is `invalid_argument`, `rng` is `result_out_of_range`. The variable starts at
-7, so a failed call shows -7.

```text
1.5  g:3/ok/1.5  f:3/ok/1.5  s:0/inv/-7  h:3/ok/1.3125
1e3  g:3/ok/1000  f:1/ok/1  s:3/ok/1000  h:3/ok/483
1.5e  g:3/ok/1.5  f:3/ok/1.5  s:0/inv/-7  h:4/ok/1.3671875
0x1p4  g:1/ok/0  f:1/ok/0  s:0/inv/-7  h:1/ok/0
1p4  g:1/ok/1  f:1/ok/1  s:0/inv/-7  h:3/ok/16
+1  g:0/inv/-7  f:0/inv/-7  s:0/inv/-7  h:0/inv/-7
 1  g:0/inv/-7  f:0/inv/-7  s:0/inv/-7  h:0/inv/-7
inf  g:3/ok/inf  f:3/ok/inf  s:3/ok/inf  h:3/ok/inf
nan  g:3/ok/nan  f:3/ok/nan  s:3/ok/nan  h:3/ok/nan
1,5  g:1/ok/1  f:1/ok/1  s:0/inv/-7  h:1/ok/1
-0  g:2/ok/-0  f:2/ok/-0  s:0/inv/-7  h:2/ok/-0
1e999  g:5/rng/-7  f:1/ok/1  s:5/rng/-7  h:5/ok/125337
1e-999  g:6/rng/-7  f:1/ok/1  s:6/rng/-7  h:2/ok/30
99999999...  g:400/rng/-7  f:400/rng/-7  s:0/inv/-7  h:400/rng/-7
strtod " +1" -> 1, consumed 3
strtod "0x1p4" -> 16, consumed 5
```

The last row is 400 nines. `hex` reads its input as hexadecimal digits without a `0x` prefix, so `1e3` is 0x1e3 = 483 and
`1.5` is 1.3125.

## libc++

With `-stdlib=libc++` (clang 22.1 on Compiler Explorer) every consumed count and every `errc` is the same. Only the value in
the variable after a `result_out_of_range` differs: libstdc++ leaves the -7, libc++ writes inf (for `1e999` and the 400 nines) or 0
(for `1e-999`). [charconv.from.chars] says the value is unmodified in that case. The program does not assert the value of an
out-of-range cell, so it exits 0 on both libraries.

```text
1e999  g:5/rng/inf  f:1/ok/1  s:5/rng/inf  h:5/ok/125337
1e-999  g:6/rng/0  f:1/ok/1  s:6/rng/0  h:2/ok/30
99999999...  g:400/rng/inf  f:400/rng/inf  s:0/inv/-7  h:400/rng/inf
```

## Not covered

Locale behaviour (`from_chars` has none by definition; not run), `float` and `long double`, the `std::chars_format::general`
default overload, timing, and MSVC.

# magic-numbers-in-tests: bare numbers in test comparisons

Companion to episode 5 of the wro.cpp series on clang-tidy checks for the mistakes AI coding agents keep making.
The check is `wrocpp-magic-numbers-in-tests` from https://github.com/wrocpp/tidy-agent (commit 7c64b6c), a query check
(no plugin). Neither file contains a macro, and both build and exit 0 with the default command.

- `examples/before.cpp`: eight comparisons in a hand-written `check()` helper. Four are thresholds that deserve a name,
  one is a status code compared twice, two are sizes. Run on Compiler Explorer (g162, `-std=c++20 -O1 -Wall -Wextra`):
  https://godbolt.org/z/v99exxjxY. Check on Compiler Explorer: https://godbolt.org/z/rsTscEWz3
- `examples/after.cpp`: the four thresholds named in `constexpr` constants. Run: https://godbolt.org/z/rv4rKdjTY.
  Check: https://godbolt.org/z/WK17aPjev

The check links open `x86-64 clang (trunk)` with `-std=c++20 -fsyntax-only` and the tool `clang-tidy (trunk)`.
The tool arguments are the check's query from `checks/query/magic-numbers-in-tests.yaml` passed with
`--experimental-custom-checks`, with one change: the `isExpansionInFileMatching("(^|/)tests?/")` clause is removed,
because the Compiler Explorer file is `<source>`, not a path under `test/`. The shortlinks were compared byte for byte
with the files. Runs on 2026-11-17.

## Program output

Both programs print the same eight lines and a summary, exit 0, no compiler warnings (g162 and g++ 16):

```text
ok  : latency below 250 ms
ok  : at most 5 retries
ok  : last backoff within 5000 ms
ok  : first backoff at least 50 ms
ok  : status is 200
ok  : status below 300
ok  : three body bytes
ok  : three backoff steps
8 checked, 0 failed
```

## before.cpp: nine warnings

Nine warnings for eight comparisons, because the index in `kBackoffMs[2] <= 5000` is also a literal inside a comparison:

```text
<source>:37:33: warning: bare number in a test comparison; ... [custom-wrocpp-magic-numbers-in-tests]   (250)
<source>:38:31 (5)   <source>:39:22 (index 2)   <source>:39:28 (5000)   <source>:40:28 (50)
<source>:41:30 (200)  <source>:42:29 (300)   <source>:43:33 (size 3)   <source>:44:32 (size 3)
```

## after.cpp: five warnings remain

The four thresholds are gone. The index, the status code (twice) and the two sizes are still reported, and `after.cpp`
leaves them bare on purpose: renaming `200` or the number of elements in a literal list adds noise, not information.

```text
<source>:48:22 (index 2)  <source>:50:30 (200)  <source>:51:29 (300)  <source>:52:33 (size 3)  <source>:53:32 (size 3)
```

## Local run

Homebrew clang-tidy 23.1.5, `clang-tidy --experimental-custom-checks --config-file=dist/tidy-agent.clang-tidy
--checks='-*,custom-wrocpp-magic-numbers-in-tests' test/before.cpp -- -std=c++20`, with the files copied under a `test/`
directory so the path pattern matches. Same nine and five warnings at the same lines. Program built with g++ 16
(`-std=c++20 -O1 -Wall -Wextra`): exit 0, the output above.

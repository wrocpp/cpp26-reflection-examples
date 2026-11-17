# A unit in an integer name: what the check flags and what it cannot see

Companion to episode 2 of the wro.cpp series on clang-tidy checks for the mistakes AI coding agents keep making.
The check is `wrocpp-unit-in-integer-name` from https://github.com/wrocpp/tidy-agent, a query check (no plugin). Neither file
contains a macro, and both build and exit 0 with the default command.

- `examples/unit_in_integer_name.cpp`: a TTL of 30 seconds stored into a timeout in milliseconds, as raw integers and as
  `std::chrono` durations. Run + check on Compiler Explorer: https://godbolt.org/z/MsY1oKG1T
- `examples/names_the_check_misses.cpp`: three members the check flags and six it does not. Check on Compiler Explorer:
  https://godbolt.org/z/soqefM48M

Both links open `x86-64 clang (trunk)` with `-std=c++20 -fsyntax-only` and the tool `clang-tidy (trunk)`, which reported
`LLVM version 24.0.0git` on 2026-11-17. The tool arguments are the check's query from `checks/query/unit-in-integer-name.yaml`
passed with `--experimental-custom-checks`. The shortlinks were compared byte for byte with the files.

## unit_in_integer_name.cpp

Program output (g162, `-std=c++20 -O1`, 2026-11-17, exit 0):

```text
plain: timeoutMs = 30 (meant 30000)
typed: timeout = 30000 ms
```

clang-tidy, two warnings, both on the `plain` struct and none on the `typed` one:

```text
unit_in_integer_name.cpp:15:5: warning: a unit that lives only in the name is not checked; use a std::chrono duration or a units type [custom-wrocpp-unit-in-integer-name]
   15 |     std::int64_t ttl_sec;
unit_in_integer_name.cpp:16:5: warning: a unit that lives only in the name is not checked; use a std::chrono duration or a units type [custom-wrocpp-unit-in-integer-name]
   16 |     std::int64_t timeoutMs;
```

Two `static_assert`s state the type facts: `seconds` converts to `milliseconds`, and `milliseconds` does not convert to `seconds`.

## names_the_check_misses.cpp

Program output (g162, `-std=c++20 -O1`, exit 0): `members: 9 (sizeof 72)`.

clang-tidy flags `deadlineMs`, `retry_sec` and `chunk_bytes` and nothing else. The six members it does not flag:

| member | why the check stays silent |
|---|---|
| `timeout` (comment says milliseconds) | the unit is in a comment |
| `delay` of alias type `Millis` | the unit is in a type alias, and the name does not carry it |
| `period` (comment says seconds) | the unit is in a comment |
| `lifetimeMilliseconds` | `Milliseconds` is not among the configured suffixes |
| `windowMinutes` | `Minutes` is not among the configured suffixes |
| `ms` | the pattern needs a character before `Ms`/`ms` suffixes |

## Local run

The same two files with Homebrew clang-tidy (LLVM 23.1.5) and `dist/tidy-agent.clang-tidy` from the repo:
`clang-tidy --experimental-custom-checks --config-file=dist/tidy-agent.clang-tidy --checks='-*,custom-wrocpp-unit-in-integer-name' FILE -- -std=c++20`.
Same warnings, same lines. Run 2026-11-17.

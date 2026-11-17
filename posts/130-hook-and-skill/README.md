# hook-and-skill: the tidy-agent hook on a seeded header comment

Companion to the wro.cpp post about the `PostToolUse` hook and the `mistake-to-check` skill of tidy-agent, episode 9 of 9.
The C++ files contain no macros.

- `examples/events_before.hpp`: a header with three comments that `wrocpp-header-comment-policy` reports (one git pointer, two narrations of history) and three that it must leave alone. Syntax-checked only.
- `examples/events_after.hpp`: the same header with the three comments rewritten as contracts. The check reports nothing.
- `examples/main.cpp`: uses `events_after.hpp`, prints three lines, exits 0.
- `examples/run-hook.sh`: runs the hook script by hand on `events_before.hpp` in a scratch project. It installs nothing and touches no settings file.

Compiler Explorer cannot take a program with a local include, and the check is a plugin that Compiler Explorer cannot load, so there are no Godbolt links. Everything ran locally on 2026-11-17.

## Program output

`g++ -std=c++26 -Wall -Wextra main.cpp` (g++-16 Homebrew GCC 16.2.0, no warnings) prints and exits 0:

```text
debug -> debug severe=0
error -> error severe=1
bogus -> info severe=0
```

`g++ -std=c++26 -fsyntax-only -x c++ events_before.hpp` also exits 0: the flagged header is valid C++.

## The hook, run by hand

Setup: LLVM and clang-tidy 23.1.5 (Homebrew), plugin `libwrocpp-tidy.so` built from tidy-agent commit 7c64b6c against that LLVM, Python 3.

The hook is a Claude Code `PostToolUse` hook. `hooks.json` in the plugin registers it for the matcher `Edit|Write|MultiEdit`. Claude Code runs `tidy_hook.py` and sends the event as JSON on stdin. The script reads `tool_input.file_path` and skips any file that is not C or C++. It then runs clang-tidy on that one file with the project's `.clang-tidy` and `compile_commands.json`.

Command (the script is in this directory):

```text
examples/run-hook.sh <tidy-agent checkout> /opt/homebrew/opt/llvm/bin/clang-tidy <tidy-agent checkout>/build/libwrocpp-tidy.so <scratch dir> hard
```

The scratch project has `.clang-tidy` with `Checks: '-*,wrocpp-header-comment-policy'`. The hook reads `WarningsAsErrors` from it.

### Blocking: `WarningsAsErrors: 'wrocpp-*'`

```text
event: {"hook_event_name":"PostToolUse","tool_name":"Edit","tool_input":{"file_path":"<scratch>/hook-hard/include/events.hpp"}}
clang-tidy findings that this project treats as errors; fix them before continuing:
include/events.hpp:13: header comment points at version control; that belongs in the commit message [wrocpp-header-comment-policy]
include/events.hpp:21: header comment narrates the code's history; that belongs in the commit message [wrocpp-header-comment-policy]
include/events.hpp:31: header comment narrates the code's history; that belongs in the commit message [wrocpp-header-comment-policy]
exit=2
```

The hook exits 2 and writes the findings to stderr, which Claude Code hands back to the agent. The log `.tidy-agent/findings.jsonl` got six lines for three findings: each finding appears once under `wrocpp-header-comment-policy` and once under `-warnings-as-errors`, the second tag that clang-tidy appends to a finding that is promoted to an error. That is a duplicate in the log and I did not fix it.

### Advisory: `WarningsAsErrors: ''` (the `soft` argument)

```text
event: {"hook_event_name":"PostToolUse","tool_name":"Edit","tool_input":{"file_path":"<scratch>/hook-soft/include/events.hpp"}}
{"hookSpecificOutput": {"hookEventName": "PostToolUse", "additionalContext": "clang-tidy findings in the file you just edited:\ninclude/events.hpp:13: header comment points at version control; that belongs in the commit message [wrocpp-header-comment-policy]\ninclude/events.hpp:21: header comment narrates the code's history; that belongs in the commit message [wrocpp-header-comment-policy]\ninclude/events.hpp:31: header comment narrates the code's history; that belongs in the commit message [wrocpp-header-comment-policy]"}}
exit=0
```

The hook exits 0 and prints JSON with `additionalContext`. The log has three lines.

### After the agent fixes the comments

With `events_after.hpp` in place of the header, the hook prints nothing and exits 0.

## Hook tests

`python3 -m unittest` in `plugins/tidy-agent/hooks/tests` of the tidy-agent checkout: 9 tests, OK, with `TIDY_AGENT_CLANG_TIDY=/opt/homebrew/opt/llvm/bin/clang-tidy` and PyYAML installed in a scratch virtual environment. Without PyYAML the two end-to-end tests fail on `import yaml`.

## The check on a real header history

cloudevents-sdk-cpp (https://github.com/filipsajdak/cloudevents-sdk-cpp). Commit 7ef3e13 says that 1,091 lines of prose left `include/`. I ran `wrocpp-header-comment-policy` with its default patterns, one header at a time:

```text
clang-tidy -load=libwrocpp-tidy.so --checks='-*,wrocpp-header-comment-policy' <header> -- -x c++-header -std=c++23 -Iinclude
```

| Revision | Headers analysed | Locations reported |
| --- | --- | --- |
| 7ef3e13~1 (before the cut) | 24 | 1 (`format/typed_payload.hpp:62`, "It used to return ...") |
| 7ef3e13 | 24 | 0 |
| a414e8d (head) | 24 | 0 |

At 7ef3e13~1, 16 of the 24 headers stopped at a missing third-party include (ctre and others, not installed), so the count is a lower bound for comments after the first failing include. The default patterns are narrow: they match pointers at git and a few narration phrases, not prose in general.

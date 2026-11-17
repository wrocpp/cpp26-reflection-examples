#!/bin/sh
# run-hook.sh - run the tidy-agent PostToolUse hook by hand on events_before.hpp, as Claude Code would after an Edit.
#
# Usage: run-hook.sh <tidy-agent checkout> <clang-tidy> <libwrocpp-tidy> <scratch dir> [hard|soft]
#   hard: wrocpp-* is in WarningsAsErrors, so the hook exits 2 and prints the findings on stderr.
#   soft: WarningsAsErrors is empty, so the hook exits 0 and prints JSON additionalContext.
# It touches nothing outside the scratch dir: no plugin install, no settings file.
set -eu
repo=$1 tidy=$2 plugin=$3 work=$4 mode=${5:-hard}
here=$(cd "$(dirname "$0")" && pwd)

rm -rf "$work"
mkdir -p "$work/include" "$work/src" "$work/build"
git -C "$work" init -q
cp "$here/events_before.hpp" "$work/include/events.hpp"
printf '#include "events.hpp"\n\nint main() { return 0; }\n' > "$work/src/main.cpp"

if [ "$mode" = hard ]; then errors="wrocpp-*"; else errors=""; fi
printf "Checks: '-*,wrocpp-header-comment-policy'\nWarningsAsErrors: '%s'\n" "$errors" > "$work/.clang-tidy"

sdk=$(xcrun --show-sdk-path 2>/dev/null || true)
printf '[{"directory":"%s","file":"%s/src/main.cpp","arguments":["%s","-std=c++23","-isysroot","%s","-I%s/include","-c","src/main.cpp"]}]\n' \
  "$work" "$work" "${tidy%clang-tidy}clang++" "$sdk" "$work" > "$work/build/compile_commands.json"

event=$(printf '{"hook_event_name":"PostToolUse","tool_name":"Edit","tool_input":{"file_path":"%s/include/events.hpp"}}' "$work")
echo "event: $event"

set +e
printf '%s\n' "$event" | TIDY_AGENT_CLANG_TIDY=$tidy TIDY_AGENT_PLUGIN=$plugin \
  python3 "$repo/plugins/tidy-agent/hooks/tidy_hook.py"
echo "exit=$?"
echo "--- .tidy-agent/findings.jsonl"
cat "$work/.tidy-agent/findings.jsonl"

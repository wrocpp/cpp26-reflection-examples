#!/usr/bin/env bash
# Run a single .cpp file under ThreadSanitizer.
# Used by the wro.cpp toolset's `sanitizers-2026` page DockerRun blocks.
#
# Usage (typically via docker run):
#   containers/scripts/run-tsan.sh <path/to/source.cpp>
#
# Expected: non-zero exit when TSan reports a data race -- that is the demo.
set -uo pipefail

src="${1:?usage: run-tsan.sh <source.cpp>}"
bin="$(mktemp)"
trap 'rm -f "$bin"' EXIT

clang++ -std=c++26 -stdlib=libc++ \
    -fsanitize=thread \
    -fno-omit-frame-pointer \
    -O1 -g \
    "$src" -o "$bin"

TSAN_OPTIONS=halt_on_error=1 "$bin"

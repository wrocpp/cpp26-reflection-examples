#!/usr/bin/env bash
# Run a single .cpp file under UndefinedBehaviorSanitizer.
# Used by the wro.cpp toolset's `sanitizers-2026` page DockerRun blocks.
#
# Usage (typically via docker run):
#   containers/scripts/run-ubsan.sh <path/to/source.cpp>
#
# Expected: non-zero exit when UBSan finds UB -- that is the demo.
set -uo pipefail

src="${1:?usage: run-ubsan.sh <source.cpp>}"
bin="$(mktemp)"
trap 'rm -f "$bin"' EXIT

clang++ -std=c++26 -stdlib=libc++ \
    -fsanitize=undefined \
    -fno-sanitize-recover=all \
    -fno-omit-frame-pointer \
    -O1 -g \
    "$src" -o "$bin"

UBSAN_OPTIONS=print_stacktrace=1 "$bin"

#!/usr/bin/env bash
# Run a single .cpp file under AddressSanitizer.
# Used by the wro.cpp toolset's `sanitizers-2026` page DockerRun blocks.
#
# Usage (typically via docker run):
#   containers/scripts/run-asan.sh <path/to/source.cpp>
#
# Expected: non-zero exit when ASan finds a bug -- that is the demo.
set -uo pipefail   # NOTE: no -e; the program is supposed to crash.

src="${1:?usage: run-asan.sh <source.cpp>}"
bin="$(mktemp)"
trap 'rm -f "$bin"' EXIT

clang++ -std=c++26 -stdlib=libc++ \
    -fsanitize=address \
    -fno-omit-frame-pointer \
    -O1 -g \
    "$src" -o "$bin"

ASAN_OPTIONS=symbolize=1:print_stacktrace=1 "$bin"

#!/usr/bin/env bash
# Compile with clang-p2996 from the container, C++26 + reflection enabled.
# Usage: ./cpp hello_reflection.cpp -o hello_reflection
set -euo pipefail

docker run --rm \
    -v "$(pwd):/work" \
    -w /work \
    clang-p2996:latest \
    clang++ -std=c++26 -freflection-latest -stdlib=libc++ "$@"

#!/usr/bin/env bash
# Compile & run every example under posts/*/examples/*.cpp (and the dated news
# shorts under posts/00-news/*/examples/*.cpp) in the clang-p2996 container.
# Reports pass/fail per example and overall status.
#
# Files tagged "// verify: gcc-only" in their header are skipped here: they use
# features the pinned clang-p2996 fork does not implement (e.g. P2900
# contracts) and are verified on Compiler Explorer's GCC 16.1 instead.
set -uo pipefail  # NOTE: no -e; we want to keep going after a single failure.

cd "$(dirname "$0")"

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

pass=0
fail=0
skip=0
failed=()

# Scratch dir lives inside the mounted workspace so the container can see it.
scratch=".verify-scratch"
mkdir -p "${scratch}"
trap 'rm -rf "${scratch}"' EXIT

shopt -s nullglob
for example in posts/*/examples/*.cpp posts/00-news/*/examples/*.cpp; do
    name=${example#posts/}
    bin="${scratch}/$(echo "${name}" | tr '/' '_' | sed 's/\.cpp$//')"

    printf "  %-60s " "${name}"
    if grep -qE '^//[[:space:]]*verify:[[:space:]]*gcc-only' "${example}"; then
        printf "${YELLOW}SKIP${NC} (gcc-only)\n"
        skip=$((skip + 1))
        continue
    fi
    # Examples that need a Compiler Explorer library (ce-libs, e.g. stdexec) or
    # an extra CE-supplied file (ce-file, e.g. an #embed asset) are not built in
    # this container; they are compiled + run on Compiler Explorer instead.
    if grep -qE '^//[[:space:]]*verify:[[:space:]]*ce-(libs|file|only)' "${example}"; then
        printf "${YELLOW}SKIP${NC} (ce-only)\n"
        skip=$((skip + 1))
        continue
    fi
    if ./cpp "${example}" -o "${bin}" >/dev/null 2>&1 && ./run "${bin}" >/dev/null 2>&1; then
        printf "${GREEN}PASS${NC}\n"
        pass=$((pass + 1))
    else
        printf "${RED}FAIL${NC}\n"
        fail=$((fail + 1))
        failed+=("${name}")
    fi
done

echo
echo "Summary: ${pass} passed, ${fail} failed, ${skip} skipped"

if [ "${fail}" -gt 0 ]; then
    printf "\n${YELLOW}Failed examples:${NC}\n"
    for f in "${failed[@]}"; do
        echo "  - ${f}"
    done
    echo
    echo "Rerun a single failing example with visible output:"
    echo "  ./cpp ${failed[0]} -o /tmp/debug && ./run /tmp/debug"
    exit 1
fi

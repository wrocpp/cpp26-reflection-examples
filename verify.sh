#!/usr/bin/env bash
# Compile & run every example under posts/*/examples/*.cpp in the container.
# Reports pass/fail per example and overall status.
set -uo pipefail  # NOTE: no -e; we want to keep going after a single failure.

cd "$(dirname "$0")"

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

pass=0
fail=0
failed=()

# Scratch dir lives inside the mounted workspace so the container can see it.
scratch=".verify-scratch"
mkdir -p "${scratch}"
trap 'rm -rf "${scratch}"' EXIT

shopt -s nullglob
for example in posts/*/examples/*.cpp; do
    name=${example#posts/}
    bin="${scratch}/$(echo "${name}" | tr '/' '_' | sed 's/\.cpp$//')"

    printf "  %-60s " "${name}"
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
echo "Summary: ${pass} passed, ${fail} failed"

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

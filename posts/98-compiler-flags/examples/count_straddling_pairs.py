#!/usr/bin/env python3
"""Count fused compare-and-jump pairs that cross or end on a 32-byte boundary.

Compiles jcc_boundary.cpp on Compiler Explorer (x86-64 GCC 16.2) twice, once with and once without
-Wa,-mbranches-within-32B-boundaries, reads the disassembly with addresses and opcode lengths, and counts
adjacent cmp/test + conditional-jump pairs that cross a 32-byte boundary or end exactly on one.
Only cmp/test followed by a conditional jump is counted; the erratum also covers other jumps, calls and
returns, which this script does not look at.

Usage: python3 -I count_straddling_pairs.py jcc_boundary.cpp
"""
import json
import re
import sys
import urllib.request

BOUNDARY = 32
COMPILER = "g162"
OPTIONS = "-O2 -std=c++26"


def compile_on_ce(source: str, extra: str) -> list:
    payload = {
        "source": source,
        "compiler": COMPILER,
        "options": {
            "userArguments": f"{OPTIONS} {extra}".strip(),
            "filters": {"binaryObject": True, "intel": True, "labels": True,
                        "directives": True, "commentOnly": True},
            "compilerOptions": {},
        },
        "lang": "c++",
    }
    request = urllib.request.Request(
        f"https://godbolt.org/api/compiler/{COMPILER}/compile",
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json", "Accept": "application/json"},
    )
    body = json.load(urllib.request.urlopen(request, timeout=120))
    if body.get("code") != 0:
        sys.exit(f"compile failed: {body.get('code')}")
    return body.get("asm", [])


def straddling_pairs(asm: list) -> tuple[int, int]:
    instructions = [line for line in asm if "opcodes" in line and "address" in line]
    pairs = straddling = 0
    for first, second in zip(instructions, instructions[1:]):
        a, b = first["text"].split(), second["text"].split()
        if a and b and a[0] in ("cmp", "test") and re.fullmatch(r"j(?!mp)\w+", b[0]):
            start = first["address"]
            end = second["address"] + len(second["opcodes"])
            pairs += 1
            if start // BOUNDARY != (end - 1) // BOUNDARY or end % BOUNDARY == 0:
                straddling += 1
    return pairs, straddling


if __name__ == "__main__":
    source = open(sys.argv[1]).read()
    for label, extra in (("default", ""),
                         ("-Wa,-mbranches-within-32B-boundaries", "-Wa,-mbranches-within-32B-boundaries")):
        pairs, bad = straddling_pairs(compile_on_ce(source, extra))
        print(f"{label:40s} pairs: {pairs:3d}   crossing or ending on a 32-byte boundary: {bad:3d}")

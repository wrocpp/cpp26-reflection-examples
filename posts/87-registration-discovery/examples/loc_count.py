#!/usr/bin/env python3
"""Prints the registration statement for one type, hand-written and reflected, and counts its lines.
One call per line, as in the post. usage: python3 loc_count.py [N ...]   (default 5 20 50)"""
import sys

def hand(n):
    lines = ["entt::meta_factory<S>{ctx}", '    .type("S")']
    lines += [f'    .data<&S::m{i}>("m{i}")' for i in range(n)]
    lines += ['    .func<&S::poke>("poke");']
    return lines

def reflected(_n):
    return ["rqt::register_type<S>(ctx);"]

for n in [int(a) for a in sys.argv[1:]] or [5, 20, 50]:
    print(f"{n} members: hand-written {len(hand(n))} lines, reflected {len(reflected(n))} line")

#!/usr/bin/env python3
"""Summarise results-<tag>.csv as the markdown tables used in the post.

Usage: python3 analyze.py results-mac-m2max.csv
Each cell: median (min-max) in wall nanoseconds per increment per thread, then the
coefficient of variation (sample standard deviation / mean) of the runs.
A second table gives each layout's median divided by the median of "padded to HDIS".
"""
import csv
import statistics
import sys
from collections import defaultdict

ORDER = ["packed", "padded_hdis", "padded_128", "padded_64", "shared_one", "thread_local", "stack_local"]
NAMES = {
    "packed": "packed",
    "padded_hdis": "padded to HDIS",
    "padded_128": "padded to 128",
    "padded_64": "padded to 64",
    "shared_one": "one shared counter",
    "thread_local": "thread_local",
    "stack_local": "stack local",
}


def fmt(x):
    return f"{x:.0f}" if x >= 10 else f"{x:.2f}"


def load(path):
    cells, bad = defaultdict(list), 0
    for row in csv.DictReader(open(path)):
        bad += row["checksum_ok"] != "1"
        cells[(row["flavour"], row["variant"], int(row["threads"]))].append(float(row["ns_per_increment"]))
    return cells, bad


def main():
    cells, bad = load(sys.argv[1])
    ts = sorted({k[2] for k in cells})
    print(f"{sys.argv[1]}: {bad} checksum failures; runs per cell {sorted({len(v) for v in cells.values()})}\n")
    for flavour in ("atomic", "plain"):
        present = [v for v in ORDER if (flavour, v, ts[0]) in cells]
        print(f"### {flavour}: median (min-max) ns per increment, CV\n")
        print("| Layout | " + " | ".join(f"{t} thr" for t in ts) + " |")
        print("|---|" + "---|" * len(ts))
        for v in present:
            row = []
            for t in ts:
                xs = cells[(flavour, v, t)]
                cv = statistics.stdev(xs) / statistics.mean(xs) * 100
                row.append(f"{fmt(statistics.median(xs))} ({fmt(min(xs))}-{fmt(max(xs))}), {cv:.0f}%")
            print(f"| {NAMES[v]} | " + " | ".join(row) + " |")
        print(f"\n### {flavour}: median divided by the median of padded to HDIS\n")
        print("| Layout | " + " | ".join(f"{t} thr" for t in ts) + " |")
        print("|---|" + "---|" * len(ts))
        for v in present:
            row = [f"{statistics.median(cells[(flavour, v, t)]) / statistics.median(cells[(flavour, 'padded_hdis', t)]):.2f}" for t in ts]
            print(f"| {NAMES[v]} | " + " | ".join(row) + " |")
        print()


if __name__ == "__main__":
    main()

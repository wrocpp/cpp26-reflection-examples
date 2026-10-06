#!/usr/bin/env python3
"""Summarise bench_logs/bench_<variant>_run<k>.csv: per run the median over samples; across runs the median of medians; min-max over all samples."""
import csv, glob, statistics as st, sys, os
os.chdir(os.path.dirname(os.path.abspath(__file__)))
order = ["aos32_OrderBad", "aos24_OrderGood", "aos24_reordered_OrderBad", "aos24_reordered_OrderGood_control", "split_hot24_cold1", "soa"]
for variant in ("O2", "O2_novec"):
    files = sorted(glob.glob(f"bench_logs/bench_{variant}_run*.csv"))
    if not files: continue
    cells = {}
    for f in files:
        for r in csv.DictReader(open(f)):
            key = (r["kind"], int(r["n"]), r["layout"])
            c = cells.setdefault(key, {"runs": {}, "bytes": float(r["bytes_per_elem"])})
            c["runs"].setdefault(f, []).append((float(r["wall_ns_per_elem"]), float(r["cpu_ns_per_elem"])))
    print(f"\n### variant {variant}  ({len(files)} processes x samples; wall = steady_clock, cpu = thread CPU time)\n")
    for kind in ("if", "mask", "all"):
        for n in sorted({k[1] for k in cells if k[0] == kind}):
            print(f"\n**pass `{kind}`, N = {n}**\n")
            print("| layout | B/elem | wall ns/elem median (per-process medians) | min-max (all samples) | cpu ns/elem median | GB/s (wall) | vs aos32 |")
            print("|---|---|---|---|---|---|---|")
            base = None
            for lay in order:
                c = cells.get((kind, n, lay))
                if not c: continue
                meds = [st.median(w for w, _ in v) for v in c["runs"].values()]
                allw = [w for v in c["runs"].values() for w, _ in v]
                cpu = st.median(cc for v in c["runs"].values() for _, cc in v)
                med = st.median(meds)
                if base is None: base = med
                gbs = c["bytes"] / med
                print(f"| {lay} | {c['bytes']:g} | {med:.3f} ({', '.join(f'{m:.3f}' for m in meds)}) | {min(allw):.3f}-{max(allw):.3f} | {cpu:.3f} | {gbs:.1f} | {med / base:.2f}x |")

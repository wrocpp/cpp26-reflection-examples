#!/bin/zsh
# run_bench.sh - full benchmark: 2 flag sets x 3 separate processes x 7 samples x N in {1e3,1e5,1e7}; records machine load around each batch.
# Usage: ./run_bench.sh   (writes bench_logs/bench_<variant>_run<k>.csv/.meta; summarise with summarize.py)
cd "${0:A:h}" || exit 1
G=${CXX:-g++}
SAMPLES=${SAMPLES:-7}; MIN_MS=${MIN_MS:-50}; SIZES=${SIZES:-"1000 100000 10000000"}
load1() { sysctl -n vm.loadavg | awk '{print $2}'; }
busy() { ps -Ao pcpu= | awk '$1>20{n++} END{print n+0}'; }
sysctl hw.l1dcachesize hw.l2cachesize hw.cachelinesize hw.perflevel0.l2cachesize hw.perflevel0.cpusperl2 hw.memsize machdep.cpu.brand_string hw.ncpu > bench_logs/machine.txt 2>&1
$G --version | head -1 >> bench_logs/machine.txt
for variant in O2 O2_novec; do
  flags="-O2"; [[ $variant == O2_novec ]] && flags="-O2 -fno-tree-vectorize"
  $G -std=c++26 -freflection ${=flags} -Wall -Wextra -Werror bench.cpp -o /tmp/s7_bench_$variant || exit 1
  for k in 1 2 3; do
    tries=0; label=quiet
    while (( $(echo "$(load1) > 4" | bc) )); do
      (( tries++ ))
      if (( tries > 3 )); then label="NOISY(load>4 after 3 retries)"; break; fi
      sleep 30
    done
    out=bench_logs/bench_${variant}_run$k
    echo "variant=$variant flags=\"$flags\" run=$k label=$label" > $out.meta
    echo "before: load=$(load1) busy_procs(>20%cpu)=$(busy) total_procs=$(ps -A | wc -l | tr -d ' ') time=$(date +%T)" >> $out.meta
    /tmp/s7_bench_$variant $SAMPLES $MIN_MS ${=SIZES} > $out.csv
    echo "rc=$?" >> $out.meta
    echo "after: load=$(load1) busy_procs(>20%cpu)=$(busy) total_procs=$(ps -A | wc -l | tr -d ' ') time=$(date +%T)" >> $out.meta
  done
done

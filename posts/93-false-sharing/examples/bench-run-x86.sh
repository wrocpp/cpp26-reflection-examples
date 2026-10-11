#!/bin/sh
# False-sharing benchmark for the x86-64 CI job (see README.md). Run from the repository root
# after ci/host-info.sh and ci/wait-quiet.sh: sh bench/run.sh [runs] [n]
# Everything is written under results/ (the artifact): the binary, results-<name>.csv (raw, never edited),
# results-<name>.filtered.csv (noisy repetitions removed), noise-<name>.csv, stdout-<name>.txt and run.txt.
# BENCH_CPUS (default 0-7) is the CPU mask; configurations pin with `taskset -c` to subsets of it:
#   first4   the first 4 CPUs of the mask, threads 1,2,4
#   all      the whole mask, threads 1,2,4,8
#   percore4 four CPUs on four different physical cores (only when the mask holds SMT siblings)
# Exit 0 only if every repetition ran and passed its checksum; 3 if the machine is not x86-64.
set -u

arch=$(uname -m)
if [ "$arch" != "x86_64" ]; then
  mkdir -p results
  echo "WRONG MACHINE: uname -m prints '$arch', expected x86_64. No benchmark was run." | tee results/run.txt >&2
  exit 3
fi

runs=${1:-11}          # planned repetitions per configuration
n=${2:-10000000}
MIN_QUIET=7            # fewer quiet repetitions than this: extend
MAX_REPS=25            # ... but never beyond this many in total
NOISE_MARGIN=1         # noisy when loadavg1_after > own thread count + NOISE_MARGIN
BUDGET_S=720           # extension repetitions stop this many seconds after bench/run.sh starts
start_s=$(date +%s)

here=$(cd "$(dirname "$0")/.." && pwd)
cd "$here" || exit 1
mkdir -p results
log_file=results/run.txt
flags="-std=c++23 -O2 -pthread"
: > "$log_file"
log() { echo "$@" | tee -a "$log_file"; }
section() { log ""; log "== $*"; }

mask=${BENCH_CPUS:-0-7}
# expand "0-3,6" into one CPU number per line
cpu_list=$(echo "$mask" | awk -F, '{for(i=1;i<=NF;i++){k=split($i,a,"-"); if(k==2) for(c=a[1];c<=a[2];c++) print c; else print $i}}')
ncpu=$(echo "$cpu_list" | grep -c .)
join() { paste -sd, -; }
# threads_for <count>: 1,2,4,... up to count
threads_for() { awk -v k="$1" 'BEGIN{t=1; s=""; while(t<=k){s=s (s==""?"":",") t; t*=2} print s}'; }

section "bench/run.sh"
log "BENCH_CPUS mask: $mask ($ncpu CPUs); affinity of this shell: $(taskset -cp $$ 2>&1)"
log "thread_siblings_list is already in host-info.txt; read again here only to choose percore4"

section "build"
g++ --version | head -1 | tee -a "$log_file"
log "flags: $flags"
g++ $flags bench/bench.cpp -o results/bench-x86 2>&1 | tee -a "$log_file"
[ -x results/bench-x86 ] || { log "BUILD FAILED"; exit 1; }
g++ $flags -S -o results/bench-x86.s bench/bench.cpp
log "lock-prefixed adds in generated code: $(grep -c -E 'lock (xadd|add|incq)' results/bench-x86.s); ldadd: $(grep -c ldadd results/bench-x86.s)"
log "planned repetitions=$runs n=$n min_quiet=$MIN_QUIET max_reps=$MAX_REPS noise_rule='loadavg1_after > own_threads + $NOISE_MARGIN'"

fail=0
# one_rep <name> <cpus> <threads> <rep>: one repetition (a round over every variant and thread count);
# appends its rows to the raw CSV, records the noise, sets rep_noisy to 1 or 0.
one_rep() {
  name=$1; cpus=$2; threads=$3; rep=$4
  raw=results/results-$name.csv
  noise=results/noise-$name.csv
  own=$(echo "$threads" | tr ',' '\n' | sort -n | tail -1)
  l_before=$(cut -d' ' -f1 /proc/loadavg); r_before=$(cut -d' ' -f4 /proc/loadavg | cut -d/ -f1)
  taskset -c "$cpus" results/bench-x86 --threads "$threads" --runs 1 --no-warmup --run-base "$rep" --n "$n" --pin \
    --out results/rep.tmp >> "results/stdout-$name.txt" 2>&1
  rc=$?
  l_after=$(cut -d' ' -f1 /proc/loadavg); r_after=$(cut -d' ' -f4 /proc/loadavg | cut -d/ -f1)
  if [ "$rc" -ne 0 ] || [ ! -s results/rep.tmp ] || ! awk -F, 'NR>1 && $8!=1{bad=1} END{exit bad}' results/rep.tmp; then
    log "CONFIG $name rep $rep: FAILED (exit $rc, checksum or output)"; fail=1
  fi
  if [ ! -s "$raw" ]; then head -1 results/rep.tmp > "$raw"; fi
  tail -n +2 results/rep.tmp >> "$raw"
  rm -f results/rep.tmp
  if awk -v a="$l_after" -v t="$own" -v m="$NOISE_MARGIN" 'BEGIN{exit !(a > t + m)}'; then rep_noisy=1; else rep_noisy=0; fi
  echo "$name,$rep,$l_before,$l_after,$r_before,$r_after,$own,$rep_noisy" >> "$noise"
  log "  $name rep $rep: loadavg1 $l_before -> $l_after, running $r_before -> $r_after, own threads $own, noisy=$rep_noisy"
}

# run_cfg <name> <cpu list> <thread list>
run_cfg() {
  name=$1; cpus=$2; threads=$3
  section "config $name: taskset -c $cpus, threads $threads"
  log "loadavg at start: $(cat /proc/loadavg)"
  : > "results/stdout-$name.txt"
  rm -f "results/results-$name.csv" "results/results-$name.filtered.csv"
  echo "config,rep,loadavg1_before,loadavg1_after,running_before,running_after,own_threads,noisy" > "results/noise-$name.csv"
  # one discarded warm-up round per configuration
  taskset -c "$cpus" results/bench-x86 --threads "$threads" --runs 0 --n "$n" --pin --out results/warmup.tmp >> "results/stdout-$name.txt" 2>&1 \
    || { log "CONFIG $name: warm-up FAILED"; fail=1; }
  rm -f results/warmup.tmp
  rep=0; noisy=0; quiet=0
  while :; do
    if [ "$rep" -ge "$runs" ]; then
      [ "$quiet" -ge "$MIN_QUIET" ] && break
      [ "$rep" -ge "$MAX_REPS" ] && break
      now_s=$(date +%s)
      if [ $((now_s - start_s)) -ge "$BUDGET_S" ]; then log "  time budget ${BUDGET_S}s reached, no more extension repetitions"; break; fi
      log "  only $quiet quiet repetitions of $rep: extending"
    fi
    rep=$((rep + 1))
    one_rep "$name" "$cpus" "$threads" "$rep"
    noisy=$((noisy + rep_noisy)); quiet=$((rep - noisy))
  done
  # filtered CSV: same schema, rows of noisy repetitions removed; the raw CSV is never edited
  noisy_list=$(awk -F, 'NR>1 && $8==1{print $2}' "results/noise-$name.csv" | paste -sd, -)
  awk -F, -v bad=",$noisy_list," 'NR==1 || index(bad, "," $4 ",")==0' "results/results-$name.csv" > "results/results-$name.filtered.csv"
  log "CONFIG $name: $rep repetitions, $noisy marked noisy, $quiet remain (noisy repetitions: ${noisy_list:-none})"
  if [ "$quiet" -lt "$MIN_QUIET" ]; then log "INSUFFICIENT_QUIET_REPS $name"; fi
  log "loadavg at end: $(cat /proc/loadavg)"
}

if [ "$ncpu" -ge 8 ]; then first4=$(echo "$cpu_list" | head -4 | join); run_cfg first4 "$first4" 1,2,4; fi
if [ "$ncpu" -ge 1 ]; then run_cfg all "$(echo "$cpu_list" | join)" "$(threads_for "$ncpu")"; fi
if [ "$ncpu" -lt 8 ] && [ "$ncpu" -ge 4 ]; then log "mask has fewer than 8 CPUs: first4 skipped, 'all' covers it"; fi

# percore4: four CPUs of the mask that are the lowest member of their own thread_siblings_list
percore=""
any_sibling=0
for c in $cpu_list; do
  s=$(cat "/sys/devices/system/cpu/cpu$c/topology/thread_siblings_list" 2>/dev/null) || continue
  case "$s" in *,*|*-*) any_sibling=1 ;; esac
  first=$(echo "$s" | sed 's/[,-].*//')
  [ "$first" = "$c" ] && percore="$percore $c"
done
if [ "$any_sibling" = 1 ]; then
  percore=$(echo $percore | tr ' ' '\n' | head -4 | join)
  if [ "$(echo "$percore" | tr ',' '\n' | grep -c .)" -ge 4 ]; then
    run_cfg percore4 "$percore" 4
  else log "fewer than 4 physical cores in the mask; percore4 skipped"; fi
else
  log ""; log "no SMT siblings visible in the mask (or topology unreadable): percore4 skipped"
fi

section "summary"
log "elapsed: $(( $(date +%s) - start_s )) s"
ls -l results | tee -a "$log_file"
if [ "$fail" -eq 0 ]; then log "RESULT: all checksums passed"; exit 0; fi
log "RESULT: FAILURES"; exit 1

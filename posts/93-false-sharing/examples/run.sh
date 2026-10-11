#!/bin/sh
# Build and run the false-sharing benchmark. Usage: sh run.sh mac|docker|tls [threads] [runs]
#   mac    - Homebrew GCC 16.2 on the host (Apple Silicon, no affinity API)
#   docker - gcc:16.2 image on Docker/colima (a Linux VM with virtual CPUs), threads pinned
# Output: results-<tag>.csv (one row per run), env-<tag>.txt (machine, flags, load).
set -eu
mode=${1:?usage: run.sh mac|docker [threads] [runs]}
threads=${2:-}
runs=${3:-11}
here=$(cd "$(dirname "$0")" && pwd)
flags="-std=c++23 -O2 -pthread"

case "$mode" in
mac)
  threads=${threads:-1,2,4,8,12}
  tag=mac-m2max
  out="$here/env-$tag.txt"
  {
    echo "date: $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "cpu: $(sysctl -n machdep.cpu.brand_string)"
    echo "perflevel0 (logical): $(sysctl -n hw.perflevel0.logicalcpu) $(sysctl -n hw.perflevel0.name)"
    echo "perflevel1 (logical): $(sysctl -n hw.perflevel1.logicalcpu) $(sysctl -n hw.perflevel1.name)"
    echo "hw.cachelinesize: $(sysctl -n hw.cachelinesize)"
    echo "hw.l1dcachesize: $(sysctl -n hw.l1dcachesize 2>/dev/null || echo n/a)"
    echo "hw.l2cachesize: $(sysctl -n hw.l2cachesize 2>/dev/null || echo n/a)"
    echo "frequency: not controlled (no DVFS control on macOS)"
    echo "power: $(pmset -g | grep -i -E 'lowpowermode|powermode' | tr -s ' ' | tr '\n' ';')"
    echo "thermal: $(pmset -g therm 2>&1 | tr '\n' ';')"
    echo "uptime before: $(uptime)"
    echo "top processes by cpu before:"; ps -Ao pcpu,comm -r | head -6
    echo "compiler: $(g++-16 --version | head -1)"
    echo "flags: $flags"
  } > "$out"
  g++-16 $flags "$here/bench.cpp" -o "$here/bench-$tag"
  g++-16 $flags -S -o "$here/bench-$tag.s" "$here/bench.cpp"
  echo "atomic add in the generated code: $(grep -o -E 'ldadd[a-z0-9_]*' "$here/bench-$tag.s" | sort | uniq -c | tr '\n' ' ')" >> "$out"
  "$here/bench-$tag" --threads "$threads" --runs "$runs" --out "$here/results-$tag.csv" | tee -a "$out"
  { echo "uptime after: $(uptime)"; } >> "$out"
  ;;
docker)
  threads=${threads:-1,2,4,8}
  tag=docker-aarch64
  { echo "host (macOS) uptime before: $(uptime)"; echo "host cpu: $(sysctl -n machdep.cpu.brand_string)"
    echo "host top processes by cpu before:"; ps -Ao pcpu,comm -r | head -6; } > "$here/env-$tag.txt"
  ncpu=$(docker run --rm gcc:16.2 nproc)
  last=$((ncpu - 1))
  docker run --rm --cpuset-cpus="0-$last" -v "$here":/work -w /work gcc:16.2 sh -c "
    {
      echo date: \$(date -u +%Y-%m-%dT%H:%M:%SZ)
      echo \"image: gcc:16.2 (\$(uname -m), kernel \$(uname -r))\"
      lscpu | grep -E 'Vendor ID|Model name|^CPU\\(s\\)|Thread|Flags' | cut -c1-120
      echo 'frequency: not controlled (virtual CPUs inside a VM)'
      echo \"uptime before: \$(uptime)\"
      echo \"compiler: \$(g++ --version | head -1)\"
      echo 'flags: $flags'
    } >> env-$tag.txt
    g++ $flags bench.cpp -o bench-$tag && g++ $flags -S -o bench-$tag.s bench.cpp
    echo \"atomic add in the generated code: \$(grep -o -E 'ldadd[a-z0-9_]*' bench-$tag.s | sort | uniq -c | tr '\\n' ' ')\" >> env-$tag.txt
    ./bench-$tag --threads $threads --runs $runs --pin --out results-$tag.csv | tee -a env-$tag.txt
    echo \"uptime after: \$(uptime)\" >> env-$tag.txt
  "
  ;;
tls)
  # Where do thread_local counters of live threads sit? (no timings)
  out="$here/tls-addresses.txt"
  { echo "== macOS, $(g++-16 --version | head -1)"; g++-16 $flags "$here/tls_addresses.cpp" -o "$here/tlsa" && "$here/tlsa"
    echo "== macOS, $(/usr/bin/clang++ --version | head -1)"; /usr/bin/clang++ $flags "$here/tls_addresses.cpp" -o "$here/tlsa" && "$here/tlsa"
    echo "== Linux aarch64 (Docker VM), gcc:16.2"
    docker run --rm -v "$here":/work -w /work gcc:16.2 sh -c "g++ $flags tls_addresses.cpp -o tlsa && ./tlsa"
  } | tee "$out"
  ;;
*) echo "unknown mode $mode" >&2; exit 2 ;;
esac

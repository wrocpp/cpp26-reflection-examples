#!/bin/sh
# Builds and checks every demo in this directory. Exit 0 only if each one behaves as the post says.
#   CXX   compiler, default g++ (GCC 16.2 or newer, needs -freflection)
# The benchmark is only compiled and smoke-run here (small N, checksums compared); the timings in the post come from run_bench.sh.
cd "$(dirname "$0")" || exit 2
CXX=${CXX:-g++}
W=$(mktemp -d "${TMPDIR:-/tmp}/layout.XXXXXX"); trap 'rm -rf "$W"' EXIT
CXXFLAGS="-std=c++26 -freflection -Wall -Wextra -Werror"
FAILS=0
ok()  { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; FAILS=$((FAILS+1)); }
expect_out() { if grep -qF -- "$3" "$2"; then ok "$1"; else bad "$1 (missing: $3)"; head -12 "$2" | sed 's/^/        | /'; fi; }
build_run() { n=$1; s=$2; shift 2
  if $CXX $CXXFLAGS "$@" "$s" -o "$W/$n" > "$W/$n.cerr" 2>&1; then "$W/$n" > "$W/$n.out" 2>&1; echo "exit=$?" >> "$W/$n.out"
  else echo COMPILE-ERROR > "$W/$n.out"; head -8 "$W/$n.cerr" | sed 's/^/        | /'; fi; }
expect_compile_error() { n=$1; s=$2; p=$3
  if $CXX $CXXFLAGS -c "$s" -o "$W/$n.o" > "$W/$n.cerr" 2>&1; then bad "$n compiled but must not"; else expect_out "$n compile error" "$W/$n.cerr" "$p"; fi; }

echo "== report, reordered, budgets, split, conversions (demo.cpp, and its single-file form)"
./make_single_files.sh
build_run demo demo.cpp
build_run demo_single demo_single.cpp
for n in demo demo_single; do
  expect_out "$n: OrderBad 32 bytes, 14 padding (7 tail)" "$W/$n.out" "OrderBad: 32 bytes, align 8, 14 padding bytes (7 of them tail)"
  expect_out "$n: OrderGood 24 bytes, 6 padding" "$W/$n.out" "OrderGood: 24 bytes, align 8, 6 padding bytes (6 of them tail)"
  expect_out "$n: reordered<OrderBad> is 24 bytes" "$W/$n.out" "layout::reordered_holder<OrderBad>::type: 24 bytes"
  expect_out "$n: padding totals" "$W/$n.out" "padding_bytes: OrderBad 14, OrderGood 6, reordered<OrderBad> 6"
  expect_out "$n: hot group" "$W/$n.out" "hot group of Cached: 3 members, span 25 [0,25), payload 17"
  expect_out "$n: moves, not copies" "$W/$n.out" "string buffer moved: 1, unique_ptr moved: 1, source name empty: 1"
  expect_out "$n: exit 0" "$W/$n.out" "exit=0"
done

echo "== the reordered type differs from the original (P1112R5)"
build_run p1112 p1112.cpp
expect_out "positional init compiles with other meaning" "$W/p1112.out" "reordered price=100 quantity=66 side=7 active=1"
expect_out "structured bindings read other members" "$W/p1112.out" "reordered b=7 c=66"
expect_out "no operator== on the copy" "$W/p1112.out" "on reordered<OrderCmp>: 0"
expect_out "exit 0" "$W/p1112.out" "exit=0"
expect_compile_error fail_desig fail_desig.cpp "designator order for field"

echo "== budgets fail the build with a readable report"
expect_compile_error fail_budget fail_budget.cpp "layout budget: OrderBad has 14 padding bytes, budget 8"
expect_compile_error fail_group fail_group.cpp "spans 144 bytes [0, 144) with 16 payload bytes in 2 members; budget"

echo "== is descending alignment minimal? (brute force over every ordering and every small multiset)"
build_run brute brute.cpp -O2
expect_out "2,992 multisets, no counterexample" "$W/brute.out" "2992 multisets of 2..5 members, 0 with sorted != minimum"
expect_out "11,613 multisets with over-aligned members, 1,578 counterexamples" "$W/brute.out" "11613 multisets of 2..5 members, 1578 with sorted != minimum"
expect_out "exit 0" "$W/brute.out" "exit=0"

echo "== what the toolkit refuses"
build_run limits limits.cpp
expect_out "bit-field refused" "$W/limits.out" "is a bit-field and moving it would change how the bits pack (refused)"
expect_out "default member initializer refused" "$W/limits.out" "has a default member initializer and define_aggregate drops it (refused)"
expect_out "base class refused" "$W/limits.out" "it has a base class and define_aggregate adds data members only (refused)"
expect_out "exit 0" "$W/limits.out" "exit=0"

echo "== benchmark builds and every layout agrees on the checksum (small N only)"
if $CXX $CXXFLAGS -O2 bench.cpp -o "$W/bench" > "$W/bench.cerr" 2>&1 && "$W/bench" 1 5 1000 100000 > "$W/bench.csv"; then ok "bench smoke"; else bad "bench smoke"; head -8 "$W/bench.cerr" | sed 's/^/        | /'; fi

echo; [ "$FAILS" -eq 0 ] && echo "all checks passed" || echo "$FAILS check(s) failed"; exit "$FAILS"

#!/bin/sh
# Builds and checks every demo in this directory. Exit 0 only if each one behaves as the post says.
#   CXX        compiler, default g++ (GCC 16.2 or newer, needs -freflection)
#   ENTT_DIR   directory that holds entt/entt.hpp; when unset, the single header is downloaded at the
#              pinned commit below into a temporary directory and its SHA-256 is checked
# Build products go to a temporary directory that is removed at the end.
cd "$(dirname "$0")" || exit 2
CXX=${CXX:-g++}
ENTT_COMMIT=e0f4c3763967db04f116863a0ec5e35110808a26
ENTT_SHA256=eedcb83fedffe640334ef69a0647625a186cf1daeb333b21b0a1445f01e89b6c
W=$(mktemp -d "${TMPDIR:-/tmp}/rqt.XXXXXX"); trap 'rm -rf "$W"' EXIT
if [ -z "${ENTT_DIR:-}" ]; then
  mkdir -p "$W/third_party/entt"
  curl -fsS -o "$W/third_party/entt/entt.hpp" \
    "https://raw.githubusercontent.com/skypjack/entt/$ENTT_COMMIT/single_include/entt/entt.hpp" || { echo "download failed"; exit 2; }
  got=$( (sha256sum "$W/third_party/entt/entt.hpp" 2>/dev/null || shasum -a 256 "$W/third_party/entt/entt.hpp") | cut -d' ' -f1)
  [ "$got" = "$ENTT_SHA256" ] || { echo "entt.hpp hash mismatch: $got"; exit 2; }
  ENTT_DIR=$W/third_party
fi
# EnTT is included with -isystem: its own header trips -Wextra -Werror (missing-field-initializers) on GCC 16.2.
CXXFLAGS="-std=c++26 -freflection -Wall -Wextra -Werror -isystem $ENTT_DIR"
FAILS=0
ok()  { echo "  ok    $1"; }
bad() { echo "  FAIL  $1"; FAILS=$((FAILS+1)); }
cc()  { $CXX $CXXFLAGS "$@"; }
expect_out() { if grep -qE "$3" "$2"; then ok "$1"; else bad "$1 (missing: $3)"; head -12 "$2" | sed 's/^/        | /'; fi; }
refute_out() { if grep -qE "$3" "$2"; then bad "$1 (unexpected: $3)"; else ok "$1"; fi; }
build_run() { n=$1; s=$2; shift 2
  if cc "$@" "$s" -o "$W/$n" > "$W/$n.cerr" 2>&1; then "$W/$n" > "$W/$n.out" 2>&1; echo "exit=$?" >> "$W/$n.out"
  else echo COMPILE-ERROR > "$W/$n.out"; head -8 "$W/$n.cerr" | sed 's/^/        | /'; fi; }
expect_compile_error() { n=$1; s=$2; p=$3; shift 3
  if cc "$@" "$s" -o "$W/$n" > "$W/$n.cerr" 2>&1; then bad "$n compiled but must not"; else expect_out "$n compile error" "$W/$n.cerr" "$p"; fi; }

echo "== registration: hand-written entt::meta_factory chain vs rqt::register_type"
build_run entt entt_equal.cpp
expect_out "hand-written and reflected registrations are equal" "$W/entt.out" "A: hand == reflected"
expect_out "exit 0" "$W/entt.out" "exit=0"
cat "$W/entt.out" | sed 's/^/        | /'

echo "== discovery in one translation unit"
build_run one tests_one_tu.cpp
for t in "pass   adds" "pass   by_name" "pass   parses\[n=3\]" "pass   parses\[n=4\]" "xfail  known_bug" "5 cases, 0 failed" "exit=0"; do
  expect_out "$t" "$W/one.out" "$t"; done
refute_out "helper_not_a_test is not collected" "$W/one.out" "helper_not_a_test"
cat "$W/one.out" | sed 's/^/        | /'
build_run onefail tests_one_tu.cpp -DDEMO_FAIL
expect_out "a failing test is reported by name" "$W/onefail.out" "FAIL   fails_on_purpose: f.base is 40, not 0"
expect_out "a failing run exits 1" "$W/onefail.out" "exit=1"
expect_compile_error nofix err_no_fixture.cpp "no fixture function returns this parameter's type"
expect_compile_error ambig err_ambiguous_fixture.cpp "several fixture functions return this parameter's type"
expect_compile_error notag err_no_tag.cpp "no matching function for call to 'rqt::registrar<\^\^tests, \^\^fixtures>::registrar\(\)'"
build_run walk global_walk.cpp
expect_out "walk from the global namespace finds tests in nested namespaces" "$W/walk.out" "TU-wide discovery: 4 tests"

echo "== discovery across translation units: tu_a.cpp, tu_b.cpp and runner.cpp share namespace tests"
for f in tu_a tu_b runner naive_tu_a naive_tu_b; do cc -c $f.cpp -o "$W/$f.o" > "$W/$f.err" 2>&1 || { bad "compile $f"; head -5 "$W/$f.err"; }; done
cc "$W/tu_a.o" "$W/tu_b.o" "$W/runner.o" -o "$W/x1" && "$W/x1" > "$W/x1.out"
expect_out "walking the namespace in the runner finds only the runner's own test" "$W/x1.out" "direct walk of \^\^tests in the runner TU: 1 case"
expect_out "the registry filled by static initialisation has all 7" "$W/x1.out" "registry \(fed by static initialisation\): 7 case"
expect_out "a test from TU B ran" "$W/x1.out" "pass   subtracts"
cat "$W/x1.out" | sed 's/^/        | /'
cc "$W/runner.o" "$W/tu_b.o" "$W/tu_a.o" -o "$W/x2" && "$W/x2" > "$W/x2.out"
expect_out "reversed link order gives the same 7" "$W/x2.out" "registry \(fed by static initialisation\): 7 case"
cc "$W/naive_tu_a.o" "$W/naive_tu_b.o" "$W/runner.o" -o "$W/x3" && "$W/x3" > "$W/x3.out"
expect_out "trap, no tag: it links and still reports 7 cases" "$W/x3.out" "registry \(fed by static initialisation\): 7 case"
refute_out "trap, no tag: TU B's tests never ran" "$W/x3.out" "subtracts"
[ "$(grep -c 'pass   adds' "$W/x3.out")" = 2 ] && ok "trap, no tag: TU A's tests ran twice" || bad "trap, no tag: expected two runs of adds"
cat "$W/x3.out" | sed 's/^/        | /'

echo "== the same tests in a static library"
rm -f "$W/libtests.a"; ar rcs "$W/libtests.a" "$W/tu_a.o" "$W/tu_b.o"
cc "$W/runner.o" "$W/libtests.a" -o "$W/x4" && "$W/x4" > "$W/x4.out"
expect_out "linked as a plain static library the registrars are dropped, 1 case" "$W/x4.out" "registry \(fed by static initialisation\): 1 case"
if [ "$(uname)" = Darwin ]; then WHOLE="-Wl,-force_load,$W/libtests.a"; else WHOLE="-Wl,--whole-archive $W/libtests.a -Wl,--no-whole-archive"; fi
cc "$W/runner.o" $WHOLE -o "$W/x5" && "$W/x5" > "$W/x5.out"
expect_out "with force_load / --whole-archive all 7 are back" "$W/x5.out" "registry \(fed by static initialisation\): 7 case"

echo "== link-time optimisation and ODR warnings"
for f in tu_a tu_b runner naive_tu_a naive_tu_b; do cc -O2 -flto -c $f.cpp -o "$W/$f.lto.o" 2>&1 | head -3; done
LTOF="-O2 -flto -Wodr -Wlto-type-mismatch -Werror"
cc $LTOF "$W/tu_a.lto.o" "$W/tu_b.lto.o" "$W/runner.lto.o" -o "$W/x6" > "$W/x6.log" 2>&1 && "$W/x6" > "$W/x6.out"
[ -s "$W/x6.log" ] && bad "tagged scheme: the LTO link printed something" || ok "tagged scheme: LTO link prints nothing"
expect_out "tagged scheme under LTO: 7 cases" "$W/x6.out" "registry \(fed by static initialisation\): 7 case"
cc $LTOF "$W/naive_tu_a.lto.o" "$W/naive_tu_b.lto.o" "$W/runner.lto.o" -o "$W/x7" > "$W/x7.log" 2>&1 && "$W/x7" > "$W/x7.out"
[ -s "$W/x7.log" ] && bad "naive scheme: LTO warned (the post says it is silent)" || ok "naive scheme: LTO link prints nothing either"
refute_out "naive scheme under LTO: TU B's tests still never ran" "$W/x7.out" "subtracts"

echo "== registration line counts"
python3 loc_count.py 5 20 50

echo
if [ $FAILS -eq 0 ]; then echo "ALL AS EXPECTED"; exit 0; fi
echo "$FAILS check(s) differ from the post"; exit 1

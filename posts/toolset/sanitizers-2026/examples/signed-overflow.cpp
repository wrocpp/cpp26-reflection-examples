// Pattern: signed integer overflow, the most common UndefinedBehaviorSanitizer
// find. Adding to INT_MAX is undefined behavior in C++; the compiler is free
// to assume it never happens, with security consequences (e.g. bounds
// checks that get optimized away).
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-safety:2026-05 \
//     containers/scripts/run-ubsan.sh \
//     posts/toolset/sanitizers-2026/examples/signed-overflow.cpp
//
// Expected: program prints the diagnostic
//   runtime error: signed integer overflow: 2147483647 + 1 cannot be
//   represented in type 'int'
// and (because of -fno-sanitize-recover) exits non-zero.
//
// Why this matters in production: a "for (int i = lo; i <= hi; ++i)" loop
// where hi == INT_MAX silently becomes an infinite loop on optimised builds
// because the compiler proves "i <= INT_MAX" is always true. UBSan catches
// this in CI before the customer does.

#include <climits>
#include <print>

int next(int n) {
    return n + 1;             // <- UB when n == INT_MAX
}

int main() {
    int n = INT_MAX;
    std::println("INT_MAX + 1 = {}", next(n));
}

// Pattern: heap-use-after-free, the most common AddressSanitizer find
// in real-world C++ codebases. Allocate, free, then read.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-safety:2026-05 \
//     containers/scripts/run-asan.sh \
//     posts/toolset/sanitizers-2026/examples/use-after-free.cpp
//
// Expected: program crashes with
//   ==N==ERROR: AddressSanitizer: heap-use-after-free
// pointing at line 19 (the read) with an "freed by" stack at line 17.
//
// The bug: leaked() returns a pointer to data freed at function exit.
// The sanitizer detects the read of that dangling pointer at runtime.
// Without ASan, the program "works" (until the freed memory is reused).

#include <print>
#include <vector>

int* leaked() {
    std::vector<int> v = {1, 2, 3, 4};
    return v.data();          // returns pointer to soon-freed storage
}                              // <- v destroyed here, storage freed

int main() {
    int* p = leaked();
    std::println("read after free: {}", p[0]);   // <- ASan flags this
}

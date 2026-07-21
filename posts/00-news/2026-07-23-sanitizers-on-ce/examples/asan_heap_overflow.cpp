// asan_heap_overflow.cpp  (~18 lines)
// AddressSanitizer catches the read one past the end of a heap array and prints
// the full report: the faulting line, the allocation site, and the shadow-byte
// map. ASan aborts after the first error, so the __asan_default_options hook
// sets exitcode=0 to keep this a runnable demo (the executor rejects a nonzero
// exit). fflush makes the in-bounds line appear before the abort.
//
// Compile (GCC 16.1): g++ -std=c++20 -O1 -g -fsanitize=address asan_heap_overflow.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++20 -O1 -g -fsanitize=address

#include <cstdio>

extern "C" const char* __asan_default_options() { return "exitcode=0:halt_on_error=0"; }

int main() {
    int* a = new int[4];
    for (int i = 0; i < 4; ++i) a[i] = i * i;
    std::printf("in bounds : %d %d %d %d\n", a[0], a[1], a[2], a[3]);
    std::fflush(stdout);
    std::printf("a[4]      : %d\n", a[4]);  // one past the end: ASan fires here
    delete[] a;
    return 0;
}

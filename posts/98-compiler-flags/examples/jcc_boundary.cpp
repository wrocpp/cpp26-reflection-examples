// verify: gcc-only
// Compiler flags, part 1: a flag that only moves code. The same linear scan is emitted 32 times, each
// copy preceded by a different number of one-byte NOPs, so its compare-and-jump pairs land at every
// offset modulo 32. Compile with and without -Wa,-mbranches-within-32B-boundaries and look at the
// addresses: the flag makes the assembler pad so that no fused compare-and-jump crosses or ends on a
// 32-byte boundary. Counting the pairs is count_straddling_pairs.py.
// Build: g++-16 -O2 -std=c++23 -Wall -Wextra -Werror -c jcc_boundary.cpp   (x86-64 only; Compiler Explorer: g162)
#define SCAN(K)                                                                  \
  extern "C" long __attribute__((noinline, aligned(32)))                         \
  scan##K(const int* a, long n, int key) {                                       \
    __asm__ volatile(".fill " #K ", 1, 0x90"); /* K bytes of padding */          \
    for (long i = 0; i < n; ++i)                                                 \
      if (a[i] == key) return i;                                                 \
    return -1;                                                                   \
  }

SCAN(0)  SCAN(1)  SCAN(2)  SCAN(3)  SCAN(4)  SCAN(5)  SCAN(6)  SCAN(7)
SCAN(8)  SCAN(9)  SCAN(10) SCAN(11) SCAN(12) SCAN(13) SCAN(14) SCAN(15)
SCAN(16) SCAN(17) SCAN(18) SCAN(19) SCAN(20) SCAN(21) SCAN(22) SCAN(23)
SCAN(24) SCAN(25) SCAN(26) SCAN(27) SCAN(28) SCAN(29) SCAN(30) SCAN(31)

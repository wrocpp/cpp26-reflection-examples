// libstdcxx_fixes.cpp
// Two libstdc++ fixes from the GCC 17 development branch, one case per macro.
//
// -DCASE_HEAP: erase_if on a __gnu_pbds binary heap priority queue reallocates
//   its storage and, before PR127656 (CVE-2026-102010), kept a pointer to the
//   freed block. AddressSanitizer reports heap-use-after-free on GCC 16.2.
// -DCASE_BITSET: operator>> for std::bitset<10> from a std::wistringstream wrote
//   past an alloca buffer sized in bytes, not in wchar_t (PR124370).
//   AddressSanitizer reports dynamic-stack-buffer-overflow on GCC 16.2.
//
// Expected: GCC 16.2 stops with an AddressSanitizer report in both cases;
// GCC trunk prints the result and exits 0.
//
// Compile: g++ -std=c++20 -O1 -g -fsanitize=address -DCASE_HEAP libstdcxx_fixes.cpp
//          g++ -std=c++20 -O1 -g -fsanitize=address -DCASE_BITSET libstdcxx_fixes.cpp
// verify: ce-only

#include <bitset>
#include <cstdio>
#include <ext/pb_ds/priority_queue.hpp>
#include <sstream>

#if defined(CASE_HEAP)
bool is_odd(int v) { return v & 1; }

int main() {
    __gnu_pbds::priority_queue<int, std::less<int>, __gnu_pbds::binary_heap_tag> q;
    for (int i = 0; i < 64; ++i) q.push(i);
    q.erase_if(&is_odd);
    std::printf("%s\nsize after erase_if: %zu\n", __VERSION__, q.size());
    q.clear();
    return 0;
}
#elif defined(CASE_BITSET)
int main() {
    std::wistringstream input(L"10011011001101");
    std::bitset<10> b;
    input >> b;
    std::printf("%s\nbitset: %s\n", __VERSION__, b.to_string().c_str());
    return 0;
}
#else
#error "define CASE_HEAP or CASE_BITSET"
#endif

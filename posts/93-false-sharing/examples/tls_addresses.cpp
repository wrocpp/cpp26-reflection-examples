// verify: gcc-only (needs GCC 16.2 and <new> interference sizes; bench.cpp runs for minutes)
// Where does a thread_local counter live for each of T live threads?
// Prints the address gaps and whether any two share a 128-byte block. Prints no timings.
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <vector>

thread_local std::atomic<std::uint64_t> counter{0};

int main() {
  constexpr int T = 8;
  std::vector<std::uintptr_t> addr(T);
  std::atomic<int> alive{0};
  std::atomic<bool> done{false};
  std::vector<std::thread> pool;
  for (int t = 0; t < T; ++t)
    pool.emplace_back([&, t] {
      addr[t] = reinterpret_cast<std::uintptr_t>(&counter);
      alive.fetch_add(1);
      while (!done.load()) std::this_thread::yield();
    });
  while (alive.load() < T) std::this_thread::yield();
  std::vector<std::uintptr_t> sorted = addr;
  std::sort(sorted.begin(), sorted.end());
  std::uintptr_t min_gap = ~std::uintptr_t{0};
  for (int i = 1; i < T; ++i) min_gap = std::min(min_gap, sorted[i] - sorted[i - 1]);
  int same_block[2] = {0, 0};  // pairs sharing a 64-byte, a 128-byte block
  const std::uintptr_t block[2] = {64, 128};
  for (int b = 0; b < 2; ++b)
    for (int i = 0; i < T; ++i)
      for (int j = i + 1; j < T; ++j) same_block[b] += (addr[i] / block[b]) == (addr[j] / block[b]);
  std::printf("live threads=%d min address gap=%zu bytes, pairs in one 64-byte block=%d, in one 128-byte block=%d\n",
              T, static_cast<std::size_t>(min_gap), same_block[0], same_block[1]);
  done.store(true);
  for (auto& th : pool) th.join();
}

// verify: gcc-only
// Layout and correctness demo for the false-sharing post. Prints no timings.
// Compiler Explorer: gcc 16.2, -std=c++23 -O2 -pthread. Exits 0 when every check holds.
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <new>
#include <thread>
#include <vector>

constexpr std::size_t kDestructive = std::hardware_destructive_interference_size;
constexpr std::size_t kConstructive = std::hardware_constructive_interference_size;

struct Packed {
  std::atomic<std::uint64_t> a{0};
  std::atomic<std::uint64_t> b{0};
};

struct Padded {
  alignas(kDestructive) std::atomic<std::uint64_t> a{0};
  alignas(kDestructive) std::atomic<std::uint64_t> b{0};
};

struct Fixed128 {
  alignas(128) std::atomic<std::uint64_t> a{0};
  alignas(128) std::atomic<std::uint64_t> b{0};
};

// Packed: both counters are 8 bytes apart, so they share one cache line on any machine
// whose line is 16 bytes or more.
static_assert(sizeof(Packed) == 16 && alignof(Packed) == 8);
static_assert(offsetof(Packed, a) == 0 && offsetof(Packed, b) == 8);

// Padded: each counter starts a new block of the interference size.
static_assert(alignof(Padded) == kDestructive);
static_assert(offsetof(Padded, a) == 0 && offsetof(Padded, b) == kDestructive);
static_assert(sizeof(Padded) == 2 * kDestructive);

static_assert(alignof(Fixed128) == 128 && offsetof(Fixed128, b) == 128 && sizeof(Fixed128) == 256);

// The constants must be at least alignof(max_align_t) ([hardware.interference]).
static_assert(kDestructive >= alignof(std::max_align_t));
static_assert(kConstructive >= alignof(std::max_align_t));

template <class S> bool two_threads_agree(std::uint64_t n) {
  S s;
  std::thread t1([&] { for (std::uint64_t i = 0; i < n; ++i) s.a.fetch_add(1, std::memory_order_relaxed); });
  std::thread t2([&] { for (std::uint64_t i = 0; i < n; ++i) s.b.fetch_add(2, std::memory_order_relaxed); });
  t1.join();
  t2.join();
  return s.a.load() == n && s.b.load() == 2 * n;
}

int main() {
  constexpr std::uint64_t n = 200'000;
  const bool ok = two_threads_agree<Packed>(n) && two_threads_agree<Padded>(n) && two_threads_agree<Fixed128>(n);
  std::printf("destructive=%zu constructive=%zu sizeof(Packed)=%zu sizeof(Padded)=%zu\n", kDestructive,
              kConstructive, sizeof(Packed), sizeof(Padded));
  std::printf("counters agree: %s\n", ok ? "yes" : "NO");
  return ok ? 0 : 1;
}

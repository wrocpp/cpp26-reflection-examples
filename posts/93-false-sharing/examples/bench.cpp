// verify: gcc-only (needs GCC 16.2 and <new> interference sizes; bench.cpp runs for minutes)
// False-sharing write benchmark. T threads each increment a counter N times.
// Variants differ only in where the counters live. Raw per-run rows go to CSV.
// Build: see run.sh. Needs C++20 (<new> interference sizes, <bit>), no libraries.
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <new>
#include <string>
#include <thread>
#include <vector>
#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

namespace {

constexpr std::size_t kMaxThreads = 16;
constexpr std::size_t kDestructive = std::hardware_destructive_interference_size;
constexpr std::size_t kLine64 = 64;
constexpr std::size_t kLine128 = 128;
constexpr std::size_t kNatural = alignof(std::uint64_t);

template <std::size_t A> struct alignas(A) AtomSlot {
  std::atomic<std::uint64_t> v{0};
};
template <std::size_t A> struct alignas(A) PlainSlot {
  std::uint64_t v = 0;
};

struct alignas(kLine128 * 2) EndTime {
  std::uint64_t ns = 0;
};

using Clock = std::chrono::steady_clock;
inline std::uint64_t now_ns() {
  return static_cast<std::uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now().time_since_epoch()).count());
}

// ---- kernels: one increment per iteration, never merged by the compiler ----
// noinline: every variant runs the same machine code, addressed through one pointer register.
[[gnu::noinline]] void atomic_kernel(std::atomic<std::uint64_t>& v, std::uint64_t n) {
  for (std::uint64_t i = 0; i < n; ++i) v.fetch_add(1, std::memory_order_relaxed);
}
// Plain (non-atomic) increment. The asm statement names the variable as an in/out
// memory operand, so each iteration must store it and reload it. Volatile is not used:
// GCC 16.2 on macOS folded a volatile thread_local loop into one add (see README).
[[gnu::noinline]] void plain_kernel(std::uint64_t& v, std::uint64_t n) {
  for (std::uint64_t i = 0; i < n; ++i) {
    ++v;
    asm volatile("" : "+m"(v));
  }
}

thread_local std::atomic<std::uint64_t> tl_atomic{0};
thread_local std::uint64_t tl_plain = 0;

// ---- storage, one static array per layout ----
std::array<AtomSlot<kNatural>, kMaxThreads> a_packed;
std::array<AtomSlot<kDestructive>, kMaxThreads> a_hdis;
std::array<AtomSlot<kLine128>, kMaxThreads> a_128;
std::array<AtomSlot<kLine64>, kMaxThreads> a_64;
AtomSlot<kDestructive> a_shared;
std::array<AtomSlot<kDestructive>, kMaxThreads> merged;  // thread_local results, padded

std::array<PlainSlot<kNatural>, kMaxThreads> p_packed;
std::array<PlainSlot<kDestructive>, kMaxThreads> p_hdis;
std::array<PlainSlot<kLine128>, kMaxThreads> p_128;
std::array<PlainSlot<kLine64>, kMaxThreads> p_64;
std::array<PlainSlot<kDestructive>, kMaxThreads> p_merged;

std::array<EndTime, kMaxThreads> ends;

struct Options {
  std::vector<int> threads{1, 2, 4};
  int runs = 11;
  std::uint64_t n = 10'000'000;
  bool pin = false;
  std::string out = "results.csv";
};

void pin_thread(int tid, bool pin) {
#ifdef __linux__
  if (!pin) return;
  cpu_set_t allowed;
  CPU_ZERO(&allowed);
  if (sched_getaffinity(0, sizeof allowed, &allowed) != 0) return;
  int seen = 0;
  for (int c = 0; c < CPU_SETSIZE; ++c) {
    if (!CPU_ISSET(c, &allowed)) continue;
    if (seen++ == tid % CPU_COUNT(&allowed)) {
      cpu_set_t one;
      CPU_ZERO(&one);
      CPU_SET(c, &one);
      pthread_setaffinity_np(pthread_self(), sizeof one, &one);
      return;
    }
  }
#else
  (void)tid;
  (void)pin;  // macOS has no thread-affinity API
#endif
}

// Start T threads, release them together, return wall ns from release to last finish.
double launch(int T, bool pin, const std::function<void(int)>& body) {
  std::atomic<int> ready{0};
  std::atomic<bool> go{false};
  std::vector<std::thread> pool;
  pool.reserve(T);
  for (int t = 0; t < T; ++t)
    pool.emplace_back([&, t] {
      pin_thread(t, pin);
      ready.fetch_add(1, std::memory_order_acq_rel);
      while (!go.load(std::memory_order_acquire)) {}
      body(t);
      ends[t].ns = now_ns();
    });
  while (ready.load(std::memory_order_acquire) < T) {}
  const std::uint64_t t0 = now_ns();
  go.store(true, std::memory_order_release);
  for (auto& th : pool) th.join();
  std::uint64_t last = 0;
  for (int t = 0; t < T; ++t) last = std::max(last, ends[t].ns);
  return static_cast<double>(last - t0);
}

template <class Arr> void zero_atomic(Arr& a) { for (auto& s : a) s.v.store(0); }
template <class Arr> void zero_plain(Arr& a) { for (auto& s : a) s.v = 0; }
template <class Arr> std::uint64_t sum_atomic(const Arr& a, int T) {
  std::uint64_t s = 0;
  for (int i = 0; i < T; ++i) s += a[i].v.load();
  return s;
}
template <class Arr> std::uint64_t sum_plain(const Arr& a, int T) {
  std::uint64_t s = 0;
  for (int i = 0; i < T; ++i) s += a[i].v;
  return s;
}

struct Result { double ns; std::uint64_t checksum; };

template <class Arr> Result run_atomic_array(Arr& a, int T, std::uint64_t n, bool pin) {
  zero_atomic(a);
  const double ns = launch(T, pin, [&](int t) { atomic_kernel(a[t].v, n); });
  return {ns, sum_atomic(a, T)};
}
template <class Arr> Result run_plain_array(Arr& a, int T, std::uint64_t n, bool pin) {
  zero_plain(a);
  const double ns = launch(T, pin, [&](int t) { plain_kernel(a[t].v, n); });
  return {ns, sum_plain(a, T)};
}
Result run_shared(int T, std::uint64_t n, bool pin) {
  a_shared.v.store(0);
  const double ns = launch(T, pin, [&](int) { atomic_kernel(a_shared.v, n); });
  return {ns, a_shared.v.load()};
}
Result run_tls_atomic(int T, std::uint64_t n, bool pin) {
  zero_atomic(merged);
  const double ns = launch(T, pin, [&](int t) {
    atomic_kernel(tl_atomic, n);
    merged[t].v.fetch_add(tl_atomic.load(std::memory_order_relaxed), std::memory_order_relaxed);
  });
  return {ns, sum_atomic(merged, T)};
}
Result run_tls_plain(int T, std::uint64_t n, bool pin) {
  zero_plain(p_merged);
  const double ns = launch(T, pin, [&](int t) {
    plain_kernel(tl_plain, n);
    p_merged[t].v = tl_plain;
  });
  return {ns, sum_plain(p_merged, T)};
}

Result run_stack_atomic(int T, std::uint64_t n, bool pin) {
  zero_atomic(merged);
  const double ns = launch(T, pin, [&](int t) {
    std::atomic<std::uint64_t> local{0};
    atomic_kernel(local, n);
    merged[t].v.fetch_add(local.load(std::memory_order_relaxed), std::memory_order_relaxed);
  });
  return {ns, sum_atomic(merged, T)};
}
Result run_stack_plain(int T, std::uint64_t n, bool pin) {
  zero_plain(p_merged);
  const double ns = launch(T, pin, [&](int t) {
    std::uint64_t local = 0;
    plain_kernel(local, n);
    p_merged[t].v = local;
  });
  return {ns, sum_plain(p_merged, T)};
}

struct Variant {
  const char* flavour;
  const char* name;
  std::function<Result(int, std::uint64_t, bool)> run;
};

std::vector<Variant> make_variants() {
  return {
      {"atomic", "packed", [](int T, auto n, bool p) { return run_atomic_array(a_packed, T, n, p); }},
      {"atomic", "padded_hdis", [](int T, auto n, bool p) { return run_atomic_array(a_hdis, T, n, p); }},
      {"atomic", "padded_128", [](int T, auto n, bool p) { return run_atomic_array(a_128, T, n, p); }},
      {"atomic", "padded_64", [](int T, auto n, bool p) { return run_atomic_array(a_64, T, n, p); }},
      {"atomic", "shared_one", [](int T, auto n, bool p) { return run_shared(T, n, p); }},
      {"atomic", "thread_local", [](int T, auto n, bool p) { return run_tls_atomic(T, n, p); }},
      {"atomic", "stack_local", [](int T, auto n, bool p) { return run_stack_atomic(T, n, p); }},
      {"plain", "packed", [](int T, auto n, bool p) { return run_plain_array(p_packed, T, n, p); }},
      {"plain", "padded_hdis", [](int T, auto n, bool p) { return run_plain_array(p_hdis, T, n, p); }},
      {"plain", "padded_128", [](int T, auto n, bool p) { return run_plain_array(p_128, T, n, p); }},
      {"plain", "padded_64", [](int T, auto n, bool p) { return run_plain_array(p_64, T, n, p); }},
      {"plain", "thread_local", [](int T, auto n, bool p) { return run_tls_plain(T, n, p); }},
      {"plain", "stack_local", [](int T, auto n, bool p) { return run_stack_plain(T, n, p); }},
  };
}

std::vector<int> parse_list(const char* s) {
  std::vector<int> v;
  for (const char* p = s; *p;) {
    v.push_back(std::atoi(p));
    while (*p && *p != ',') ++p;
    if (*p == ',') ++p;
  }
  return v;
}

void print_layout() {
  std::printf("# layout: sizeof(packed atomic slot)=%zu sizeof(hdis slot)=%zu sizeof(128 slot)=%zu sizeof(64 slot)=%zu\n",
              sizeof(AtomSlot<kNatural>), sizeof(AtomSlot<kDestructive>), sizeof(AtomSlot<kLine128>),
              sizeof(AtomSlot<kLine64>));
  std::printf("# hardware_destructive_interference_size=%zu hardware_constructive_interference_size=%zu\n",
              std::hardware_destructive_interference_size, std::hardware_constructive_interference_size);
  std::printf("# hardware_concurrency=%u\n", std::thread::hardware_concurrency());
}

}  // namespace

int main(int argc, char** argv) {
  Options o;
  for (int i = 1; i < argc; ++i) {
    if (!std::strcmp(argv[i], "--threads") && i + 1 < argc) o.threads = parse_list(argv[++i]);
    else if (!std::strcmp(argv[i], "--runs") && i + 1 < argc) o.runs = std::atoi(argv[++i]);
    else if (!std::strcmp(argv[i], "--n") && i + 1 < argc) o.n = std::strtoull(argv[++i], nullptr, 10);
    else if (!std::strcmp(argv[i], "--out") && i + 1 < argc) o.out = argv[++i];
    else if (!std::strcmp(argv[i], "--pin")) o.pin = true;
    else { std::fprintf(stderr, "unknown argument %s\n", argv[i]); return 2; }
  }
  for (int t : o.threads) if (t < 1 || t > static_cast<int>(kMaxThreads)) { std::fprintf(stderr, "bad thread count\n"); return 2; }
  print_layout();
  std::FILE* f = std::fopen(o.out.c_str(), "w");
  if (!f) { std::perror("fopen"); return 2; }
  std::fprintf(f, "flavour,variant,threads,run,n_per_thread,wall_ns,ns_per_increment,checksum_ok\n");
  const auto variants = make_variants();
  int bad = 0;
  // run 0 is a discarded warm-up round; runs interleave all variants so drift hits each equally
  for (int r = 0; r <= o.runs; ++r)
    for (int T : o.threads)
      for (const auto& v : variants) {
        const Result res = v.run(T, o.n, o.pin);
        const bool ok = res.checksum == static_cast<std::uint64_t>(T) * o.n;
        if (!ok) { ++bad; std::fprintf(stderr, "CHECKSUM MISMATCH %s/%s T=%d\n", v.flavour, v.name, T); }
        if (r == 0) continue;
        std::fprintf(f, "%s,%s,%d,%d,%llu,%.0f,%.4f,%d\n", v.flavour, v.name, T, r,
                     static_cast<unsigned long long>(o.n), res.ns, res.ns / static_cast<double>(o.n), ok ? 1 : 0);
      }
  std::fclose(f);
  std::printf("# done: %d checksum failures\n", bad);
  return bad == 0 ? 0 : 1;
}

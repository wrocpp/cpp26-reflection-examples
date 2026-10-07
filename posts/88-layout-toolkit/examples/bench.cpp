// bench.cpp - AoS 32 B / AoS 24 B / reordered / hot-cold split / SoA, three passes, same data. Usage: bench <samples> <min_sample_ms> <N>...
#include "layout.hpp"
#include "kernels.hpp"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>

using namespace kernels;
struct OrderBad { std::uint64_t price; char side; std::uint64_t quantity; bool active; };
struct OrderGood { std::uint64_t price; std::uint64_t quantity; char side; bool active; };
struct Ann {
  [[=layout::hot]] std::uint64_t price;
  [[=layout::cold]] char side;
  [[=layout::hot]] std::uint64_t quantity;
  [[=layout::hot]] bool active;
};
using Reordered = layout::reordered<OrderBad>;
using GoodControl = layout::reordered<OrderGood>;  // identical layout to OrderGood: the null experiment
using Split = layout::split<Ann>;
static_assert(sizeof(OrderBad) == 32 && sizeof(OrderGood) == 24 && sizeof(Reordered) == 24 && sizeof(GoodControl) == 24);
static_assert(sizeof(Split::hot) == 24 && sizeof(Split::cold) == 1);

static std::uint64_t rng_state = 0x9E3779B97F4A7C15ull;  // fixed seed
static std::uint64_t next() {
  rng_state ^= rng_state >> 12; rng_state ^= rng_state << 25; rng_state ^= rng_state >> 27;
  return rng_state * 0x2545F4914F6CDD1Dull;
}

static double wall_ns() { return std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
static double cpu_ns() { timespec ts; clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts); return ts.tv_sec * 1e9 + ts.tv_nsec; }
static volatile std::uint64_t sink;

static std::size_t samples = 7;
static double min_ms = 50;

template <class F>
static void measure(char const* layout_name, char const* kind, std::size_t n, double bytes_per_elem, std::uint64_t expect, F pass) {
  if (pass() != expect) { std::fprintf(stderr, "CHECKSUM MISMATCH %s %s n=%zu\n", layout_name, kind, n); std::exit(2); }
  std::size_t reps = 1;
  for (;;) {  // warm-up and calibration: double until one sample lasts at least min_ms
    double t0 = wall_ns();
    for (std::size_t r = 0; r < reps; ++r) { sink = pass(); asm volatile("" ::: "memory"); }
    if (wall_ns() - t0 >= min_ms * 1e6) break;
    reps *= 2;
  }
  for (std::size_t s = 0; s < samples; ++s) {
    double w0 = wall_ns(), c0 = cpu_ns();
    for (std::size_t r = 0; r < reps; ++r) { sink = pass(); asm volatile("" ::: "memory"); }
    double w1 = wall_ns(), c1 = cpu_ns();
    std::printf("%s,%s,%zu,%zu,%zu,%g,%.4f,%.4f,%llu\n", layout_name, kind, n, s, reps, bytes_per_elem, (w1 - w0) / (double)(reps * n),
                (c1 - c0) / (double)(reps * n), (unsigned long long)expect);
    std::fflush(stdout);
  }
}

template <class T> static std::vector<T> make_aos(std::vector<OrderBad> const& c) {
  std::vector<T> v;
  v.reserve(c.size());
  for (auto const& o : c) v.push_back(layout::detail::convert<T>(o));  // by name
  return v;
}

int main(int argc, char** argv) {
  if (argc < 4) { std::fprintf(stderr, "usage: bench <samples> <min_sample_ms> <N>...\n"); return 1; }
  samples = std::strtoul(argv[1], nullptr, 10);
  min_ms = std::strtod(argv[2], nullptr);
  std::printf("layout,kind,n,sample,reps,bytes_per_elem,wall_ns_per_elem,cpu_ns_per_elem,checksum\n");
  for (int a = 3; a < argc; ++a) {
    std::size_t n = std::strtoull(argv[a], nullptr, 10);
    rng_state = 0x9E3779B97F4A7C15ull;
    std::vector<OrderBad> canon;
    canon.reserve(n);
    std::uint64_t ref_if = 0, ref_all = 0;
    for (std::size_t i = 0; i < n; ++i) {
      std::uint64_t p = next(), q = next();
      char side = (next() & 1) ? 'B' : 'S';
      bool act = (next() & 1) != 0;  // about 50% true
      canon.push_back(OrderBad{.price = p, .side = side, .quantity = q, .active = act});
      if (act) ref_if += p * q;
      ref_all += p * q + static_cast<std::uint64_t>(side) + static_cast<std::uint64_t>(act);
    }
    auto run_aos = [&]<class T>(char const* name, bool with_all, bool with_if) {
      auto v = make_aos<T>(canon);
      if (with_if) measure(name, "if", n, sizeof(T), ref_if, [&] { return k_if(v.data(), n); });
      if (with_if) measure(name, "mask", n, sizeof(T), ref_if, [&] { return k_mask(v.data(), n); });
      if (with_all) measure(name, "all", n, sizeof(T), ref_all, [&] { return k_all(v.data(), n); });
    };
    run_aos.template operator()<OrderBad>("aos32_OrderBad", true, true);
    run_aos.template operator()<OrderGood>("aos24_OrderGood", true, true);
    run_aos.template operator()<Reordered>("aos24_reordered_OrderBad", true, true);
    run_aos.template operator()<GoodControl>("aos24_reordered_OrderGood_control", false, true);
    {
      layout::split_table<Ann> t;
      t.hot.reserve(n); t.cold.reserve(n);
      for (auto const& o : canon) layout::push(t, o);
      double hb = sizeof(Split::hot), cb = sizeof(Split::cold);
      measure("split_hot24_cold1", "if", n, hb, ref_if, [&] { return k_if(t.hot.data(), n); });
      measure("split_hot24_cold1", "mask", n, hb, ref_if, [&] { return k_mask(t.hot.data(), n); });
      measure("split_hot24_cold1", "all", n, hb + cb, ref_all, [&] { return split_all(t.hot.data(), t.cold.data(), n); });
    }
    {
      std::vector<std::uint64_t> price(n), qty(n);
      std::vector<std::uint8_t> act(n), side(n);
      for (std::size_t i = 0; i < n; ++i) { price[i] = canon[i].price; qty[i] = canon[i].quantity; act[i] = canon[i].active; side[i] = canon[i].side; }
      measure("soa", "if", n, 17, ref_if, [&] { return soa_if(price.data(), qty.data(), act.data(), n); });
      measure("soa", "mask", n, 17, ref_if, [&] { return soa_mask(price.data(), qty.data(), act.data(), n); });
      measure("soa", "all", n, 18, ref_all, [&] { return soa_all(price.data(), qty.data(), side.data(), act.data(), n); });
    }
  }
}

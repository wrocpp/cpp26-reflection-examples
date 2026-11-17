// verify: gcc-only
// Return shapes for a large local (and a reuse row for comparison), and what each one costs.
// Build: g++ -std=c++26 -O2 return_shapes.cpp   (add -Wall -Wextra to see -Wpessimizing-move)
// Run:   ./a.out            prints the table
//        ./a.out fixed      the same table after mallopt(M_TRIM_THRESHOLD, 16 MiB): the mmap threshold stays at
//                           128 KiB and glibc stops adjusting it
//        ./a.out heap       the same table after mallopt(M_MMAP_THRESHOLD, 16 MiB): blocks stay on the heap, the trim
//                           threshold stays at 128 KiB, and glibc stops adjusting both
//        ./a.out keep       the same table after mallopt(M_MMAP_THRESHOLD, 16 MiB) and mallopt(M_TRIM_THRESHOLD, 16 MiB)
// Prints: which constructor each return shape ran (compile-time facts are static_asserts), then per row the
// range over kRuns runs of microseconds per call and minor page faults per call.
// Limits: glibc only (getrusage, mallopt). Times come from a shared machine: read the ranges, not one number.
// Exits 0 when the compile-time facts hold; the timing rows are printed, never asserted.
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <limits>
#include <malloc.h>
#include <sys/resource.h>
#include <type_traits>
#include <utility>
#include <vector>

// ---- which constructor ran, answered at compile time -------------------------------------------
enum class Made { fresh, copied, moved };

struct Probe {
    Made made = Made::fresh;
    Probe() = default;
    constexpr Probe(const Probe&) : made{Made::copied} {}
    constexpr Probe(Probe&&) noexcept : made{Made::moved} {}
};

struct MoveOnly {
    MoveOnly() = default;
    MoveOnly(MoveOnly&&) = default;
};

constexpr Probe make_plain() { Probe local; return local; }
constexpr Probe make_moved() { Probe local; return std::move(local); }
constexpr Probe make_const_moved() { const Probe local; return std::move(local); }

static_assert(make_plain().made != Made::copied, "a plain return never reaches the copy constructor");
static_assert(make_moved().made == Made::moved, "std::move on a local forces the move constructor");
static_assert(make_const_moved().made == Made::copied, "std::move on a const local ends in the copy constructor");
static_assert(std::is_same_v<decltype(std::move(std::declval<const Probe&>())), const Probe&&>);
static_assert(!std::is_constructible_v<MoveOnly, const MoveOnly&&>, "a const rvalue cannot reach a move constructor");

const char* name_of(Made made) { return made == Made::fresh ? "built in place" : made == Made::moved ? "move constructor" : "copy constructor"; }

[[gnu::noinline]] Probe run_plain() { Probe local; return local; }
[[gnu::noinline]] Probe run_moved() { Probe local; return std::move(local); }
[[gnu::noinline]] Probe run_const_moved() { const Probe local; return std::move(local); }

// ---- the table ------------------------------------------------------------------------------------
constexpr std::size_t kBigDoubles = 50'000;    // 400,000 bytes
constexpr std::size_t kSmallDoubles = 12'500;  // 100,000 bytes, below glibc's default 128 KiB mmap threshold
constexpr int kCalls = 500;
constexpr int kRuns = 7;
constexpr double kNsPerUs = 1000.0;
constexpr std::size_t kThresholdRaised = std::size_t{16} * 1024 * 1024;

struct Book { std::array<double, kBigDoubles> levels; };

[[gnu::noinline]] Book array_plain(double x) { Book book; book.levels.fill(x); return book; }
[[gnu::noinline]] Book array_moved(double x) { Book book; book.levels.fill(x); return std::move(book); }
[[gnu::noinline]] std::vector<double> vector_plain(double x) { std::vector<double> book(kBigDoubles, x); return book; }
[[gnu::noinline]] std::vector<double> vector_moved(double x) { std::vector<double> book(kBigDoubles, x); return std::move(book); }
[[gnu::noinline]] std::vector<double> vector_const_moved(double x) { const std::vector<double> book(kBigDoubles, x); return std::move(book); }
[[gnu::noinline]] std::vector<double> small_const_moved(double x) { const std::vector<double> book(kSmallDoubles, x); return std::move(book); }

long faults() { rusage usage{}; getrusage(RUSAGE_SELF, &usage); return usage.ru_minflt; }

volatile double sink;

struct Range { double lo = std::numeric_limits<double>::max(), hi = 0; void add(double v) { lo = v < lo ? v : lo; hi = v > hi ? v : hi; } };

template <class F> void row(const char* name, F call) {
    Range us, flt;
    for (int run = 0; run < kRuns; ++run) {
        const long f0 = faults();
        const auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < kCalls; ++i) sink = sink + call(i);
        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - t0).count();
        us.add(double(ns) / kCalls / kNsPerUs);
        flt.add(double(faults() - f0) / kCalls);
    }
    std::printf("%-44s %8.1f to %8.1f us   %6.2f to %6.2f faults\n", name, us.lo, us.hi, flt.lo, flt.hi);
}

int main(int argc, char** argv) {
    const bool fixed = argc > 1 && std::strcmp(argv[1], "fixed") == 0;
    const bool keep = argc > 1 && std::strcmp(argv[1], "keep") == 0;
    const bool heap = argc > 1 && std::strcmp(argv[1], "heap") == 0;
    if (fixed || keep) mallopt(M_TRIM_THRESHOLD, int(kThresholdRaised));
    if (heap || keep) mallopt(M_MMAP_THRESHOLD, int(kThresholdRaised));
    std::vector<double> reused(kBigDoubles);
    const std::vector<double> source(kBigDoubles, 1.0);
    std::printf("at run time: return local -> %s, return std::move(local) -> %s, const local with std::move -> %s\n",
                name_of(run_plain().made), name_of(run_moved().made), name_of(run_const_moved().made));
    std::printf("per call, %d calls per run, %d runs\n", kCalls, kRuns);
    row("std::array 400 KB, return book", [](int i) { return array_plain(i).levels[kBigDoubles / 2]; });
    row("std::array 400 KB, return std::move(book)", [](int i) { return array_moved(i).levels[kBigDoubles / 2]; });
    row("vector 400 KB, return book", [](int i) { return vector_plain(i)[kBigDoubles / 2]; });
    row("vector 400 KB, return std::move(book)", [](int i) { return vector_moved(i)[kBigDoubles / 2]; });
    row("const vector 400 KB, return std::move(book)", [](int i) { return vector_const_moved(i)[kBigDoubles / 2]; });
    row("const vector 100 KB, return std::move(book)", [](int i) { return small_const_moved(i)[kSmallDoubles / 2]; });
    row("400 KB copied into a reused buffer", [&](int) { reused.assign(source.begin(), source.end()); return reused[kBigDoubles / 2]; });
    return 0;
}

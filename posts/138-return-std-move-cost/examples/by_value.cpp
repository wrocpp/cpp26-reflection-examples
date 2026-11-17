// verify: gcc-only
// A struct that owns a std::string, passed by value and by const&, and the one case where by value is right.
// Build: g++ -std=c++26 -O2 by_value.cpp
// Prints: heap allocations per call by symbol length (counted by a replaced operator new, so the same on every
//         machine), then nanoseconds per call as a range over kRuns runs.
// Limits: the 15-character boundary is libstdc++'s small-string buffer (libc++ and MSVC differ). The timing rows
//         include the cost of the counting operator new. Exits 1 if a count differs from the one the text states.
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <new>
#include <string>
#include <utility>
#include <vector>

std::size_t g_allocations = 0;

void* operator new(std::size_t size) {
    ++g_allocations;
    if (void* block = std::malloc(size)) return block;
    throw std::bad_alloc{};
}
void operator delete(void* block) noexcept { std::free(block); }
void operator delete(void* block, std::size_t) noexcept { std::free(block); }

struct Quote { std::string symbol; double bid, ask; };

constexpr std::size_t kSmallStringCapacity = 15;  // libstdc++: characters that fit without a heap block
constexpr std::size_t kShortSymbol = 4;
constexpr std::size_t kLongSymbol = 28;
constexpr std::size_t kHeapSinkAllocations = 2;  // building the temporary, then copying it into the book
constexpr int kCalls = 1'000'000;
constexpr int kRuns = 7;
constexpr std::size_t kBookCapacity = 16;
constexpr double kBid = 1;
constexpr double kAsk = 2;

[[gnu::noinline]] double by_value(Quote quote) { return quote.bid + double(quote.symbol.size()); }
[[gnu::noinline]] double by_ref(const Quote& quote) { return quote.bid + double(quote.symbol.size()); }

struct Book {
    std::vector<Quote> quotes = std::vector<Quote>{};
    Book() { quotes.reserve(kBookCapacity); }
    [[gnu::noinline]] void add_by_value(Quote quote) { quotes.push_back(std::move(quote)); }
    [[gnu::noinline]] void add_by_ref(const Quote& quote) { quotes.push_back(quote); }
};

Quote make_quote(std::size_t length) { return Quote{.symbol = std::string(length, 'A'), .bid = kBid, .ask = kAsk}; }

template <class F> std::size_t allocations_of(F call) {
    const std::size_t before = g_allocations;
    call();
    return g_allocations - before;
}

struct Counts { std::size_t value, ref, sink_value, sink_ref; };

Counts count_for(std::size_t length) {
    const Quote lvalue = make_quote(length);
    Book book_a, book_b;
    return Counts{
        .value = allocations_of([&] { by_value(lvalue); }),
        .ref = allocations_of([&] { by_ref(lvalue); }),
        .sink_value = allocations_of([&] { book_a.add_by_value(make_quote(length)); }),
        .sink_ref = allocations_of([&] { book_b.add_by_ref(make_quote(length)); }),
    };
}

volatile double sink;

template <class F> void time_row(const char* name, F call) {
    double lo = std::numeric_limits<double>::max(), hi = 0;
    for (int run = 0; run < kRuns; ++run) {
        const auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < kCalls; ++i) sink = sink + call();
        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - t0).count();
        const double per_call = double(ns) / kCalls;
        lo = per_call < lo ? per_call : lo;
        hi = per_call > hi ? per_call : hi;
    }
    std::printf("%-28s %6.1f to %6.1f ns per call\n", name, lo, hi);
}

int main() {
    int status = 0;
    std::printf("heap allocations per call (lvalue argument; sink rows pass a temporary)\n");
    std::printf("%-8s %9s %9s %12s %12s\n", "length", "by value", "const&", "sink value", "sink const&");
    for (const std::size_t length : {kShortSymbol, kSmallStringCapacity, kSmallStringCapacity + 1, kLongSymbol}) {
        const Counts counts = count_for(length);
        std::printf("%-8zu %9zu %9zu %12zu %12zu\n", length, counts.value, counts.ref, counts.sink_value, counts.sink_ref);
        const bool heap = length > kSmallStringCapacity;
        const Counts expected{.value = heap, .ref = 0, .sink_value = heap, .sink_ref = heap ? kHeapSinkAllocations : 0};
        if (counts.value != expected.value || counts.ref != expected.ref || counts.sink_value != expected.sink_value ||
            counts.sink_ref != expected.sink_ref) status = 1;
    }
    const Quote small = make_quote(kShortSymbol);
    const Quote large = make_quote(kLongSymbol);
    time_row("by value, short symbol", [&] { return by_value(small); });
    time_row("const&,   short symbol", [&] { return by_ref(small); });
    time_row("by value, long symbol", [&] { return by_value(large); });
    time_row("const&,   long symbol", [&] { return by_ref(large); });
    return status;
}

// workarounds.cpp  (~30 lines)
// The 2026-07 mailing adds a wave of range adaptors: views::unique (P4291,
// dedup adjacent equal elements), views::take_last / views::drop_last (P4294),
// and views::cycle (P3806, repeat a range endlessly). None ship yet, so this
// file shows what you write TODAY to get the same results with the adaptors
// that already exist -- which is exactly the boilerplate the proposals collapse
// into a single named view. Compiles on GCC 16.1.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 workarounds.cpp

#include <print>
#include <ranges>
#include <vector>

int main() {
    namespace rv = std::views;
    std::vector<int> v{1, 1, 2, 3, 3, 3, 4};

    // views::unique (P4291): today, chunk_by equal-adjacent then take one each.
    auto uniq = v | rv::chunk_by(std::ranges::equal_to{})
                  | rv::transform([](auto g) { return *g.begin(); });
    std::println("unique     : {}", std::vector(std::from_range, uniq));

    // views::take_last(3) (P4294): today, drop all but the last three.
    auto last3 = v | rv::drop(v.size() - 3);
    std::println("take_last 3: {}", std::vector(std::from_range, last3));

    // views::cycle (P3806): today, repeat the range and join, then bound it.
    auto cyc = rv::repeat(v) | rv::join | rv::take(10);
    std::println("cycle x10  : {}", std::vector(std::from_range, cyc));
}

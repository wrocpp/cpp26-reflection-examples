// verify: gcc-only
// Seven ways to write std::move around a return or an initialisation, to see which tool says something.
// Build: g++ -std=c++26 -O2 -Wall -Wextra lint_shapes.cpp   (the warnings are the point; without them it is silent)
// Also:  clang++ -std=c++26 -O2 -Wall -Wextra, clang-tidy --checks=-*,performance-move-const-arg,performance-no-automatic-move
// Prints: how many elements each function returned, so every shape runs and the program exits 0.
// Limits: it shows what GCC 16.2, Clang 22.1 and clang-tidy (trunk, on Compiler Explorer) report for these seven
//         shapes, not what they report for every shape.
#include <cstddef>
#include <cstdio>
#include <utility>
#include <vector>

using Levels = std::vector<double>;

constexpr std::size_t kLevels = 50'000;
constexpr double kPrice = 1;

struct Book {
    Levels levels;
    Levels take() { return std::move(levels); }
    Levels take_const() const { return std::move(levels); }
};

Levels local_moved(double x) { Levels book(kLevels, x); return std::move(book); }
Levels const_local_moved(double x) { const Levels book(kLevels, x); return std::move(book); }
Levels const_local_stored(double x) { const Levels book(kLevels, x); Levels out = std::move(book); return out; }
Levels const_value_param(const Levels in) { return std::move(in); }
Levels const_ref_param(const Levels& in) { return std::move(in); }

int main() {
    const Levels seed(kLevels, kPrice);
    const Book book{.levels = seed};
    Book owner{.levels = seed};
    const std::size_t total = local_moved(kPrice).size() + const_local_moved(kPrice).size() +
                              const_local_stored(kPrice).size() + const_value_param(seed).size() +
                              const_ref_param(seed).size() + owner.take().size() + book.take_const().size();
    std::printf("elements returned: %zu\n", total);
    return 0;
}

// Five spellings of "are these two 64-byte blocks equal?", plus std::ranges::equal, which
// compiles to the same code as std::equal in every build the README lists. Build with -std=c++23.
// -DCOMPARE_NO_MAIN leaves main() out, so an assembly view shows only the six functions.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstring>

constexpr int block_size = 64;
using Block = std::array<char, block_size>;

// The loop from the r/cpp thread (called eq2 there): returns at the first mismatch.
bool eq_early_exit(const Block& lhs, const Block& rhs) {
    for (int i = 0; i < block_size; ++i)
        if (lhs[i] != rhs[i])
            return false;
    return true;
}

// The same loop without the early exit (eq3 in the thread): every byte is compared.
bool eq_accumulate(const Block& lhs, const Block& rhs) {
    bool res = true;
    for (int i = 0; i < block_size; ++i)
        res &= (lhs[i] == rhs[i]);
    return res;
}

bool eq_operator(const Block& lhs, const Block& rhs) { return lhs == rhs; }

bool eq_memcmp(const Block& lhs, const Block& rhs) {
    return std::memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}

bool eq_std_equal(const Block& lhs, const Block& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

bool eq_ranges_equal(const Block& lhs, const Block& rhs) { return std::ranges::equal(lhs, rhs); }

#ifndef COMPARE_NO_MAIN
namespace {

constexpr char fill_value = 'a';
constexpr int name_width = 16;

struct Spelling {
    const char* name;
    bool (*compare)(const Block&, const Block&);
};

constexpr Spelling spellings[] = {
    {.name = "eq_early_exit", .compare = eq_early_exit},
    {.name = "eq_accumulate", .compare = eq_accumulate},
    {.name = "eq_operator", .compare = eq_operator},
    {.name = "eq_memcmp", .compare = eq_memcmp},
    {.name = "eq_std_equal", .compare = eq_std_equal},
    {.name = "eq_ranges_equal", .compare = eq_ranges_equal},
};

Block make_block(char fill) {
    Block block{};
    block.fill(fill);
    return block;
}

// Takes the block by value: the caller keeps its original, and this copy is the one that changes.
Block with_byte_changed(Block block, std::size_t index) {
    ++block[index];
    return block;
}

// How many of the 64 single-byte changes of `base` the spelling reports as different.
std::size_t mismatches_found(const Spelling& spelling, const Block& base) {
    std::size_t found = 0;
    for (std::size_t index = 0; index < base.size(); ++index)
        if (!spelling.compare(base, with_byte_changed(base, index)))
            ++found;
    return found;
}

// `same` is a second object with the same contents, so the equal case never compares a block with itself.
bool agrees(const Spelling& spelling, const Block& base, const Block& same) {
    const bool equal_found = spelling.compare(base, same);
    const std::size_t found = mismatches_found(spelling, base);
    std::printf("%-*s equal=%d mismatches found=%zu of %zu\n", name_width, spelling.name,
                equal_found ? 1 : 0, found, base.size());
    return equal_found && found == base.size();
}

}  // namespace

int main() {
    const Block base = make_block(fill_value);
    const Block same = make_block(fill_value);
    int disagreements = 0;
    for (const Spelling& spelling : spellings)
        if (!agrees(spelling, base, same))
            ++disagreements;
    return disagreements == 0 ? 0 : 1;
}
#endif

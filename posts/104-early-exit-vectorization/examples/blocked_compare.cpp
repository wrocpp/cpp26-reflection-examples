// Comparing two 64-byte blocks: the early-exit loop, a loop with no exit, and the same loop cut into
// chunks that exit between chunks. Build with -std=c++26; add -DUSE_STD_SIMD for the std::simd version
// (GCC 16 and 17 ship <simd>; the Clang builds on Compiler Explorer did not accept it).
// -DCOMPARE_NO_MAIN leaves main() out, so an assembly view shows only the functions.
#include <array>
#include <cstddef>
#include <cstdio>
#ifdef USE_STD_SIMD
#include <simd>
#endif

constexpr int block_size = 64;
using Block = std::array<char, block_size>;

// The loop from the r/cpp thread: returns at the first mismatch.
bool eq_early_exit(const Block& lhs, const Block& rhs) {
    for (int i = 0; i < block_size; ++i)
        if (lhs[i] != rhs[i])
            return false;
    return true;
}

// No early exit: every byte is compared.
bool eq_accumulate(const Block& lhs, const Block& rhs) {
    bool res = true;
    for (int i = 0; i < block_size; ++i)
        res &= (lhs[i] == rhs[i]);
    return res;
}

// Early exit between chunks. Inside a chunk nothing exits: the bytes are combined and tested once.
template <int chunk_size>
bool eq_chunked(const Block& lhs, const Block& rhs) {
    static_assert(block_size % chunk_size == 0);
    for (int base = 0; base < block_size; base += chunk_size) {
        char diff = 0;
        for (int i = 0; i < chunk_size; ++i)
            diff |= static_cast<char>(lhs[base + i] ^ rhs[base + i]);
        if (diff != 0)
            return false;
    }
    return true;
}

template bool eq_chunked<8>(const Block&, const Block&);
template bool eq_chunked<16>(const Block&, const Block&);
template bool eq_chunked<32>(const Block&, const Block&);

#ifdef USE_STD_SIMD
bool eq_simd(const Block& lhs, const Block& rhs) {
    using Vec = std::simd::vec<char, block_size>;
    return std::simd::all_of(std::simd::unchecked_load<Vec>(lhs) == std::simd::unchecked_load<Vec>(rhs));
}
#endif

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
    {.name = "eq_chunked<8>", .compare = eq_chunked<8>},
    {.name = "eq_chunked<16>", .compare = eq_chunked<16>},
    {.name = "eq_chunked<32>", .compare = eq_chunked<32>},
#ifdef USE_STD_SIMD
    {.name = "eq_simd", .compare = eq_simd},
#endif
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

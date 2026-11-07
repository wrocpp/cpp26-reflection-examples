// The early-exit loop next to two controls. Compile with -std=c++23 and the options in the
// README; the file has no main().
#include <array>

constexpr int block_size = 64;
using Block = std::array<char, block_size>;

// The loop from the thread.
bool eq_early_exit(const Block& lhs, const Block& rhs) {
    for (int i = 0; i < block_size; ++i)
        if (lhs[i] != rhs[i])
            return false;
    return true;
}

// Control 1: the same loads and the same exits, but the result is an int.
int eq_early_exit_int(const Block& lhs, const Block& rhs) {
    for (int i = 0; i < block_size; ++i)
        if (lhs[i] != rhs[i])
            return 0;
    return 1;
}

// Control 2: the exits return different values (the index, or the block size).
int first_mismatch(const Block& lhs, const Block& rhs) {
    for (int i = 0; i < block_size; ++i)
        if (lhs[i] != rhs[i])
            return i;
    return block_size;
}

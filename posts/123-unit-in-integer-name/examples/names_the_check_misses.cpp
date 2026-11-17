// verify: gcc-only
// Build: g++ -std=c++20 -O1 names_the_check_misses.cpp -o names_the_check_misses
// Prints: how many members the program declares. The file exists to be given to clang-tidy:
// the first three members have a unit and are flagged, the rest have a unit the name does not
// show to the check, and are not.
// Limits: the names and the comments are the whole point, nothing here runs a timeout. Exits 0.
#include <cstddef>
#include <cstdint>
#include <iostream>

namespace {

using Millis = std::int64_t;
using Count = std::uint32_t;

struct Flagged {
    std::int64_t deadlineMs;
    std::uint32_t retry_sec;
    std::size_t chunk_bytes;
};

struct NotFlagged {
    std::int64_t timeout;       // milliseconds, said only in a comment
    Millis delay;               // the unit is in an alias, and the name does not carry it
    Count period;               // seconds, said only in a comment
    std::int64_t lifetimeMilliseconds;
    std::int64_t windowMinutes;
    std::int64_t ms;
};

constexpr std::size_t kMemberCount = 9;

}  // namespace

int main() {
    std::cout << "members: " << kMemberCount << " (sizeof " << sizeof(Flagged) + sizeof(NotFlagged) << ")\n";
    return 0;
}

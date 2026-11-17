// verify: gcc-only
// Build: g++ -std=c++20 -O1 unit_in_integer_name.cpp -o unit_in_integer_name
// Prints: the effect of a TTL in seconds stored in a timeout in milliseconds, and the same
// code with std::chrono durations, where the same slip converts or does not compile.
// Limits: the program only demonstrates the type difference. The clang-tidy check that finds
// the first form by name is in the README. Exits 0.
#include <chrono>
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace plain {

struct Config {
    std::int64_t ttl_sec;
    std::int64_t timeoutMs;
};

}  // namespace plain

namespace typed {

struct Config {
    std::chrono::seconds ttl;
    std::chrono::milliseconds timeout;
};

}  // namespace typed

namespace {

constexpr std::int64_t kTtlValue = 30;

// The slip: the number is right, the unit is not. It compiles and means 30 ms.
plain::Config plain_config() {
    return {.ttl_sec = kTtlValue, .timeoutMs = kTtlValue};
}

// The same line with durations: seconds convert to milliseconds exactly.
typed::Config typed_config() {
    const std::chrono::seconds ttl{kTtlValue};
    return {.ttl = ttl, .timeout = ttl};
}

using std::chrono::milliseconds;
using std::chrono::seconds;

// A narrowing conversion is refused, so a millisecond value cannot become seconds by accident.
static_assert(std::is_convertible_v<seconds, milliseconds>);
static_assert(!std::is_convertible_v<milliseconds, seconds>);

}  // namespace

int main() {
    const auto p = plain_config();
    const auto t = typed_config();
    std::cout << "plain: timeoutMs = " << p.timeoutMs << " (meant " << p.ttl_sec * 1000 << ")\n";
    std::cout << "typed: timeout = " << t.timeout.count() << " ms\n";
    return 0;
}

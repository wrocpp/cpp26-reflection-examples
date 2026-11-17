// verify: gcc-only
// Build: g++ -std=c++20 -O1 -Wall -Wextra before.cpp
// Prints one line per assertion and a summary; exits 0 (all assertions hold).
// Eight comparisons in a hand-written check() helper (no framework, no macros).
// Four are thresholds that deserve a name; two are a status code, two are sizes.
#include <array>
#include <cstddef>
#include <cstdio>

struct Response {
    int status;
    int latency_ms;
    int retries;
    std::size_t body_size;
};

constexpr std::array<int, 3> kBackoffMs{100, 200, 400};

Response fetch() {
    return Response{.status = 200, .latency_ms = 180, .retries = 2, .body_size = 3};
}

int checked = 0;
int failed = 0;

void check(bool ok, const char* what) {
    ++checked;
    if (!ok) {
        ++failed;
    }
    std::printf("%s: %s\n", ok ? "ok  " : "FAIL", what);
}

int main() {
    const Response response = fetch();

    check(response.latency_ms < 250, "latency below 250 ms");
    check(response.retries <= 5, "at most 5 retries");
    check(kBackoffMs[2] <= 5000, "last backoff within 5000 ms");
    check(kBackoffMs[0] >= 50, "first backoff at least 50 ms");
    check(response.status == 200, "status is 200");
    check(response.status < 300, "status below 300");
    check(response.body_size == 3, "three body bytes");
    check(kBackoffMs.size() == 3, "three backoff steps");

    std::printf("%d checked, %d failed\n", checked, failed);
    return failed;
}

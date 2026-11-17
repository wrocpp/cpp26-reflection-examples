// verify: gcc-only
// Build: g++ -std=c++20 -O1 -Wall -Wextra after.cpp
// Prints the same eight lines and summary as before.cpp; exits 0.
// The four thresholds have names that say where the number comes from. The status
// code and the two sizes stay bare on purpose: the check still reports those.
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

// Service-level objective of the fetch path.
constexpr int kMaxLatencyMs = 250;
// Retry budget of the client configuration.
constexpr int kMaxRetries = 5;
// The backoff schedule must stay inside the request timeout.
constexpr int kRequestTimeoutMs = 5000;
// A first backoff below this hammers the server.
constexpr int kMinFirstBackoffMs = 50;

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

    check(response.latency_ms < kMaxLatencyMs, "latency below 250 ms");
    check(response.retries <= kMaxRetries, "at most 5 retries");
    check(kBackoffMs[2] <= kRequestTimeoutMs, "last backoff within 5000 ms");
    check(kBackoffMs[0] >= kMinFirstBackoffMs, "first backoff at least 50 ms");
    check(response.status == 200, "status is 200");
    check(response.status < 300, "status below 300");
    check(response.body_size == 3, "three body bytes");
    check(kBackoffMs.size() == 3, "three backoff steps");

    std::printf("%d checked, %d failed\n", checked, failed);
    return failed;
}

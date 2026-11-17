// verify: gcc-only
// after.cpp - the same five functions with each value stated once, in a braced expression.
//
// Build: g++ -std=c++26 after.cpp
// Run:   ./a.out   prints the same lines as before.cpp and exits 0.
// Limits: request_for is the text clang-tidy --fix wrote (wrocpp-default-then-assign, LLVM 23.1.5). The rest is by
//         hand: the check has no fix-it for a container or for a class that is not an aggregate, and Headers needed
//         an initializer-list constructor before the braced list could compile.
#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

constexpr int kDefaultTimeoutSeconds = 30;
constexpr int kShortTimeoutSeconds = 5;
constexpr int kFirstDelayMs = 100;
constexpr int kSecondDelayMs = 200;
constexpr int kThirdDelayMs = 400;
constexpr std::size_t kExpectedHeaders = 3;

struct Request {
  std::string path;
  int timeout_seconds;
  bool retry;
};

class Headers {
 public:
  using Entry = std::pair<std::string, std::string>;

  Headers() = default;
  Headers(std::initializer_list<Entry> entries) : entries_(entries) {}

  std::size_t size() const { return entries_.size(); }

 private:
  std::vector<Entry> entries_;
};

// The fix-it reaches Request because it is an aggregate. Headers has a constructor, so it is not one.
static_assert(std::is_aggregate_v<Request>);
static_assert(!std::is_aggregate_v<Headers>);

Request request_for(std::string path) {
  Request request{.path = std::move(path), .timeout_seconds = kDefaultTimeoutSeconds, .retry = true};
  return request;
}

Request request_with_overwrite(std::string path) {
  return {.path = std::move(path), .timeout_seconds = kShortTimeoutSeconds, .retry = false};
}

std::vector<int> retry_delays_ms() { return {kFirstDelayMs, kSecondDelayMs, kThirdDelayMs}; }

Headers default_headers() {
  return {{"accept", "application/json"}, {"user-agent", "tidy-agent-demo"}, {"connection", "close"}};
}

std::vector<int> doubled(const std::vector<int>& input) {
  std::vector<int> out;
  for (int value : input) out.push_back(value * 2);
  return out;
}

}  // namespace

int main() {
  const Request a = request_for("/orders");
  const Request b = request_with_overwrite("/orders");
  const std::vector<int> delays = retry_delays_ms();
  const Headers headers = default_headers();
  const std::vector<int> twice = doubled(delays);
  std::printf("request_for: %s timeout=%d retry=%d\n", a.path.c_str(), a.timeout_seconds, a.retry);
  std::printf("request_with_overwrite: %s timeout=%d retry=%d\n", b.path.c_str(), b.timeout_seconds, b.retry);
  std::printf("delays: %d %d %d\n", delays[0], delays[1], delays[2]);
  std::printf("headers: %zu\n", headers.size());
  std::printf("doubled: %d\n", twice[0]);
  return headers.size() == kExpectedHeaders ? 0 : 1;
}

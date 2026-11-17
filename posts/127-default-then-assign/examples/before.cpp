// verify: gcc-only
// before.cpp - objects that are default-constructed and then filled in. The wrocpp-default-then-assign check flags
// four of the five functions and writes a fix-it for two of them.
//
// Build: g++ -std=c++26 before.cpp
// Run:   ./a.out   prints the contents each function built and exits 0.
// Limits: the file is the INPUT of the check, so it is meant to be flagged. Nothing here is wrong at run time except
//         that request_with_overwrite assigns timeout_seconds twice and the second value silently wins.
#include <cstddef>
#include <cstdio>
#include <string>
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
  void add(std::string name, std::string value) { entries_.emplace_back(std::move(name), std::move(value)); }
  std::size_t size() const { return entries_.size(); }

 private:
  std::vector<std::pair<std::string, std::string>> entries_;
};

Request request_for(std::string path) {
  Request request;
  request.path = std::move(path);
  request.timeout_seconds = kDefaultTimeoutSeconds;
  request.retry = true;
  return request;
}

Request request_with_overwrite(std::string path) {
  Request request;
  request.path = std::move(path);
  request.timeout_seconds = kDefaultTimeoutSeconds;
  request.timeout_seconds = kShortTimeoutSeconds;
  request.retry = false;
  return request;
}

std::vector<int> retry_delays_ms() {
  std::vector<int> delays;
  delays.push_back(kFirstDelayMs);
  delays.push_back(kSecondDelayMs);
  delays.push_back(kThirdDelayMs);
  return delays;
}

Headers default_headers() {
  Headers headers;
  headers.add("accept", "application/json");
  headers.add("user-agent", "tidy-agent-demo");
  headers.add("connection", "close");
  return headers;
}

// Not flagged: the values come from a loop over runtime data.
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

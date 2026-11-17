// verify: gcc-only
// double_designator.cpp - the compile error that the assignment form never gives. Naming one member twice in a
// designated initializer is rejected, so this file is expected NOT to compile.
//
// Build: g++ -std=c++26 double_designator.cpp   (fails; the README quotes the diagnostic)
// Run:   none. Kept apart from after.cpp so that every default build of the example directory still passes.
// Limits: the diagnostic text is the compiler's, quoted in the README with compiler id and options.
#include <string>

namespace {

constexpr int kDefaultTimeoutSeconds = 30;
constexpr int kShortTimeoutSeconds = 5;

struct Request {
  std::string path;
  int timeout_seconds;
  bool retry;
};

Request request_with_two_timeouts() {
  return {.path = "/orders",
          .timeout_seconds = kDefaultTimeoutSeconds,
          .timeout_seconds = kShortTimeoutSeconds,
          .retry = false};
}

}  // namespace

int main() { return request_with_two_timeouts().retry ? 1 : 0; }

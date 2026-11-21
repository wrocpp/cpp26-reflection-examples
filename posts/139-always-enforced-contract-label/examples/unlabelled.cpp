// verify: gcc-only
// The same program as enforced_label.cpp with the labels removed: what a released compiler does today.
// All six assertions are unlabelled, so the build alone decides what runs.
//
// Build (Compiler Explorer, GCC 16.2):
//   g162   -std=c++26 -fcontracts                                        (enforce, the default semantic)
//   g162   -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe
//   g162   -std=c++26 -fcontracts -fcontract-evaluation-semantic=ignore
// Run with no argument or with the argument pre, assert or post. Under ignore no predicate is evaluated and
// every run exits 0. Under observe every run exits 0. Under enforce every run terminates (exit 134).
//
// Prints one line "evaluated <name>" per predicate that ran.
#include <contracts>
#include <cstdio>
#include <string_view>

namespace {

constexpr int kParity = 2;            // the unlabelled precondition wants an input divisible by this
constexpr int kScale = 2;             // f returns its input times this
constexpr int kLimit = 100;           // the unlabelled postcondition wants a result below this
constexpr int kSmall = 10;            // the unlabelled contract_assert wants an input below this
constexpr int kForbiddenInput = 7;    // the labelled contract_assert forbids this input
constexpr int kForbiddenResult = 42;  // the labelled postcondition forbids this result
constexpr int kDefaultInput = kLimit / kScale + 1;              // odd, not small, and scales past kLimit
constexpr int kNegativeInput = -2;                              // even and small, but not positive
constexpr int kScalesToForbidden = kForbiddenResult / kScale;   // odd and not small

bool check(const char* name, const bool holds) {
  std::printf("evaluated %s\n", name);
  std::fflush(stdout);
  return holds;
}

int f(const int x)
    pre(check("pre even", x % kParity == 0))
    pre(check("pre positive", x > 0))
    post(r : check("post below limit", r < kLimit))
    post(r : check("post not forbidden", r != kForbiddenResult)) {
  contract_assert(check("assert small", x < kSmall));
  contract_assert(check("assert not forbidden", x != kForbiddenInput));
  return kScale * x;
}

struct Scenario {
  std::string_view name;  // a view of a string literal: static storage, it outlives the table
  int input;
};

constexpr Scenario kScenarios[] = {
    {.name = "pre", .input = kNegativeInput},
    {.name = "assert", .input = kForbiddenInput},
    {.name = "post", .input = kScalesToForbidden},
};

int pick(const std::string_view name) {
  for (const Scenario& s : kScenarios)
    if (s.name == name) return s.input;
  return kDefaultInput;
}

}  // namespace

int main(const int argc, char** argv) {
  const int input = argc > 1 ? pick(argv[1]) : kDefaultInput;
  std::printf("f(%d) = %d\n", input, f(input));
}

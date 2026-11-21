// verify: ce-only
// P4451R2 (adopted in Buzios, 2026-11): a contract assertion can carry the label
// std::contracts::controls::enforced, and then every evaluation of it uses a terminating semantic.
//
// f has six contract assertions. Three are unlabelled, three carry the label. The default call, f(51),
// breaks every unlabelled one and none of the labelled ones, so the program exits 0 in every build.
// Each argument breaks one labelled assertion instead (and some unlabelled ones): the program then
// terminates, whatever -fcontract-evaluation-semantic says.
//
// Build (Compiler Explorer, GCC and Clang with P4451 support):
//   gcc_notadragon_contracts_p3850     -std=c++26 -fcontracts-p4451 -fcontract-evaluation-semantic=observe
//   gcc_notadragon_contracts_p3850     -std=c++26 -fcontracts-p4451 -fcontract-evaluation-semantic=ignore
//   clang_notadragon_contracts_p3850   the same flags
// Run with no argument (exit 0), or with the argument pre, assert or post (exit 134, terminate).
//
// Prints one line "evaluated <name>" per predicate that ran. Under observe all six lines appear and the
// violation reports go to stderr; under ignore only the three labelled predicates run.
// Limits: of the compilers run on 2026-11-21, only the two P3850 branch builds accept the label (see the README).
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
    pre<std::contracts::controls::enforced>(check("pre positive", x > 0))
    post(r : check("post below limit", r < kLimit))
    post<std::contracts::controls::enforced>(r : check("post not forbidden", r != kForbiddenResult)) {
  contract_assert(check("assert small", x < kSmall));
  contract_assert<std::contracts::controls::enforced>(check("assert not forbidden", x != kForbiddenInput));
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

// verify: gcc-only
// Which names in a contract predicate are const, and what a predicate can still change.
// The default build runs every row that compiles once and prints how many times its predicate acted.
// -DROW=n builds one row that does not compile (101 and up), to read the compiler's message. Row 101 also
// runs when -fpermissive turns its error into a warning.
// Build: g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe what_acts.cpp   (Compiler Explorer: g162)
#include <cstdio>

#ifndef ROW
#define ROW 0
#endif

namespace {

class Registry {
 public:
  [[nodiscard]] int revision() const noexcept { return revision_; }
  int advance() noexcept { return ++revision_; }

 private:
  int revision_ = 0;
};

struct Probe {
  mutable int hits = 0;
  static inline int calls = 0;
  bool checkMutable() const noexcept { ++hits; return true; }
  bool checkStatic() const noexcept { ++calls; return true; }
};

int g_counter = 0;

bool bump() noexcept { ++g_counter; return true; }

#if ROW == 0

void viaPointer(Registry* registry) pre(registry->advance() > 0) {}
void viaMutableMember(const Probe& probe) pre((++probe.hits, true)) {}
void viaMutableCounter(const Probe& probe) pre(probe.checkMutable()) {}
void viaStaticCounter(const Probe& probe) pre(probe.checkStatic()) {}
void viaFunction() pre(bump()) {}
void viaConstCastGlobal() pre(++const_cast<int&>(g_counter) > 0) {}
void viaConstCastParameter(Registry& registry) pre(const_cast<Registry&>(registry).advance() > 0) {}
void viaLambdaGlobal() pre([] { ++g_counter; return true; }()) {}
void viaLambdaLocal() pre([] { int local = 0; ++local; return local == 1; }()) {}

int fromGlobal(void (*call)()) {
  g_counter = 0;
  call();
  return g_counter;
}

int rowPointer() { Registry registry; viaPointer(&registry); return registry.revision(); }
int rowMutableMember() { const Probe probe; viaMutableMember(probe); return probe.hits; }
int rowMutableCounter() { const Probe probe; viaMutableCounter(probe); return probe.hits; }
int rowStaticCounter() { Probe::calls = 0; const Probe probe; viaStaticCounter(probe); return Probe::calls; }
int rowFunction() { return fromGlobal(&viaFunction); }
int rowConstCastGlobal() { return fromGlobal(&viaConstCastGlobal); }
int rowConstCastParameter() { Registry registry; viaConstCastParameter(registry); return registry.revision(); }
int rowLambdaGlobal() { return fromGlobal(&viaLambdaGlobal); }
int rowLambdaLocal() { return fromGlobal(&viaLambdaLocal); }

struct Row {
  const char* name;
  int (*run)();
};

constexpr Row kRows[] = {
    {.name = "object behind a pointer parameter", .run = &rowPointer},
    {.name = "mutable member named directly", .run = &rowMutableMember},
    {.name = "const member, mutable counter", .run = &rowMutableCounter},
    {.name = "const member, static counter", .run = &rowStaticCounter},
    {.name = "call to a function with an effect", .run = &rowFunction},
    {.name = "const_cast on a global", .run = &rowConstCastGlobal},
    {.name = "const_cast on a reference parameter", .run = &rowConstCastParameter},
    {.name = "lambda that names a global", .run = &rowLambdaGlobal},
    {.name = "lambda with its own local", .run = &rowLambdaLocal},
};

#elif ROW == 101   // a non-const member through a reference parameter
void failing(Registry& registry) pre(registry.advance() > 0) {}
#elif ROW == 102   // a non-const member through *this
struct Runner {
  Registry registry;
  void go() pre(registry.advance() > 0) {}
};
#elif ROW == 103   // assignment through a reference parameter
void failing(int& reference) pre((reference = 5)) {}
#elif ROW == 104   // a namespace-scope variable
void failing() pre(++g_counter > 0) {}
#elif ROW == 105   // a by-value parameter
void failing(int value) pre(++value > 0) {}
#elif ROW == 106   // contract_assert on a local
void failing() {
  Registry local;
  contract_assert(local.advance() > 0);
}
#elif ROW == 107   // a lambda that captures a reference parameter (the paper allows the capture)
void failing(Registry& registry) pre([&registry] { return registry.revision() >= 0; }()) {}
#endif

}  // namespace

int main() {
#if ROW == 0
  for (const Row& row : kRows) std::printf("%-38s %d\n", row.name, row.run());
#elif ROW == 101
  // Only reachable with -fpermissive, which turns the error into a warning.
  Registry registry;
  failing(registry);
  std::printf("revision after one call: %d\n", registry.revision());
#endif
}

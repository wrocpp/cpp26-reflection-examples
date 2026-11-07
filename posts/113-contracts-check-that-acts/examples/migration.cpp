// verify: gcc-only
// A migration step with a precondition (no revision is skipped) and a postcondition (the registry reached
// the target). The same file runs under three evaluation semantics, chosen by the build:
//   enforce  reports the violation and terminates (the default)
//   observe  reports the violation and carries on
//   ignore   does not evaluate the predicate; the registry jumps in silence
// Build: g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=observe migration.cpp   (Compiler Explorer: g162)
// -DSHOW_ERROR adds a check that calls a non-const member function; it does not compile.
#include <cstdint>
#include <cstdio>

namespace {

constexpr std::uint32_t kInitialRevision = 6;
constexpr std::uint32_t kSkippingTarget = 9;  // skips revisions 7 and 8

class Registry {
 public:
  [[nodiscard]] std::uint32_t revision() const noexcept { return revision_; }
  void setRevision(const std::uint32_t value) noexcept { revision_ = value; }
  std::uint32_t advance() noexcept { return ++revision_; }

 private:
  std::uint32_t revision_ = kInitialRevision;
};

void migrateTo(Registry& registry, const std::uint32_t target)
    pre(target == registry.revision() + 1)
    post(registry.revision() == target)
{
  registry.setRevision(target);
}

#ifdef SHOW_ERROR
void touchy(Registry& registry) pre(registry.advance() > 0) {}
#endif

}  // namespace

int main() {
  Registry registry;
  std::puts("start");
  std::fflush(stdout);
  migrateTo(registry, kSkippingTarget);
  std::printf("revision is now %u\n", registry.revision());
}

// verify: gcc-only
// Does the contract evaluation semantic change libstdc++ hardening? An out-of-range index into a vector,
// built with -D_GLIBCXX_ASSERTIONS, under each semantic. The default build indexes in range and exits 0.
// -DSHOW_ERROR indexes past the end: libstdc++ reports "Assertion ... failed" and aborts, under every
// semantic and without -fcontracts, because its assertion is a function call, not a contract assertion.
// Build: g++ -std=c++26 -fcontracts -O2 -D_GLIBCXX_ASSERTIONS -fcontract-evaluation-semantic=ignore hardening.cpp
//        (Compiler Explorer: g162)
#include <cstddef>
#include <cstdio>
#include <vector>

namespace {

constexpr std::size_t kSize = 3;
constexpr int kFill = 7;
constexpr std::size_t kOverrun = 7;  // how far past the end the failing build reads
#ifdef SHOW_ERROR
constexpr std::size_t kIndex = kSize + kOverrun;  // past the end
#else
constexpr std::size_t kIndex = kSize - 1;  // in range
#endif

}  // namespace

int main(int argc, char**) {
  const std::vector<int> values(kSize, kFill);
  const std::size_t index = kIndex + static_cast<std::size_t>(argc - 1);  // keeps the compiler from folding it
  std::puts("before");
  std::fflush(stdout);
  std::printf("value %d\n", values[index]);
}

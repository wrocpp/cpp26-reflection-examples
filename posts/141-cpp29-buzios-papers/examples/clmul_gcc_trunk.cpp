// verify: ce-only
// std::clmul and std::widening_clmul from libstdc++ on GCC trunk, compared with the hand-written loop of
// clmul_today.cpp. P3642R8 specifies clmul(x, y) and widening_clmul(x, y), whose result has low() and high() halves.
// Build: g++ -std=c++2d -O1   (Compiler Explorer: gsnapshot; -std=c++29 gives the same). At -std=c++26, on GCC 16.2, on
// Clang 23.1 and trunk with libc++, and on MSVC, the names are missing: "'clmul' is not a member of 'std'".
// Prints the feature-test macro that libstdc++ defines (the paper's own value is a placeholder, so the number below is
// the library's, not the standard's), then three checks. Exits 0 when all pass. The program has no macros.
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <numeric>
#include <version>

constexpr std::uint32_t kAllOnes = ~std::uint32_t{0};
constexpr int kValueBits = 16;
constexpr int kTopBit = 31;
constexpr std::uint32_t kValueCount = std::uint32_t{1} << kValueBits;
constexpr std::uint32_t kTopBitSet = std::uint32_t{1} << kTopBit;
constexpr std::uint32_t kThree = 0b11;
constexpr std::uint32_t kFive = 0b101;
constexpr std::uint32_t kExpectedLow = 1;
constexpr std::uint32_t kExpectedHigh = std::uint32_t{1} << (kTopBit - 1);
constexpr std::uint32_t kStep = 4099;  // any odd offset: the second factor differs from the first

template <std::unsigned_integral T>
constexpr T by_loop(T x, T y) noexcept {
  T result = 0;
  for (int bit = 0; bit < std::numeric_limits<T>::digits; ++bit) {
    if ((y >> bit) & 1) result ^= static_cast<T>(x << bit);
  }
  return result;
}

static_assert(std::clmul(kThree, kThree) == kFive);

bool agrees_with_the_loop() {
  for (std::uint32_t x = 0; x < kValueCount; ++x) {
    if (std::clmul(x, x + kStep) != by_loop(x, x + kStep)) return false;
  }
  return true;
}

bool parity_matches_popcount() {
  for (std::uint32_t x = 0; x < kValueCount; ++x) {
    if ((std::clmul(x, kAllOnes) >> kTopBit) != (std::popcount(x) & 1)) return false;
  }
  return true;
}

int main() {
  auto wide = std::widening_clmul(kTopBitSet | 1, kTopBitSet | 1);
  bool widening = wide.low() == kExpectedLow && wide.high() == kExpectedHigh;
  bool loop = agrees_with_the_loop();
  bool parity = parity_matches_popcount();
  std::printf("__cpp_lib_clmul = %ld (the library's value)\n", static_cast<long>(__cpp_lib_clmul));
  std::printf("widening_clmul(0x80000001, 0x80000001): low %08x, high %08x\n", static_cast<unsigned>(wide.low()),
              static_cast<unsigned>(wide.high()));
  std::printf("clmul equals the loop for 65536 inputs: %s; parity equals popcount parity: %s\n", loop ? "yes" : "no",
              parity ? "yes" : "no");
  return widening && loop && parity ? 0 : 1;
}

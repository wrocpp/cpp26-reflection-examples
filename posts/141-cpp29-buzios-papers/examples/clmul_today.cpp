// verify: gcc-only
// Carry-less multiplication, the operation P3642R8 adds as std::clmul: multiply like integers, but combine the shifted
// partial products with xor instead of addition. This is the naive loop of section 4 of the paper, rewritten for any
// unsigned type, with two uses the paper shows: clmul(x, all ones) >> 31 is the parity of x, and clmul(x, x)
// spreads the bits of x to the even positions (a bit interleave).
// Build: g++ -std=c++26 -O1   (Compiler Explorer: g162; also builds with clang2310)
// Prints three checks. Exits 0 when all pass: the small product, the parity of every 16-bit value against popcount,
// and the bit spread of every 16-bit value against a loop. The program has no macros.
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <limits>

constexpr std::uint32_t kAllOnes = ~std::uint32_t{0};
constexpr int kValueBits = 16;
constexpr int kSpreadFactor = 2;
constexpr int kTopBit = 31;
constexpr std::uint32_t kValueCount = std::uint32_t{1} << kValueBits;
constexpr std::uint32_t kThree = 0b11;
constexpr std::uint32_t kFive = 0b101;

template <std::unsigned_integral T>
constexpr T clmul(T x, T y) noexcept {
  T result = 0;
  for (int bit = 0; bit < std::numeric_limits<T>::digits; ++bit) {
    if ((y >> bit) & 1) result ^= static_cast<T>(x << bit);
  }
  return result;
}

static_assert(clmul(kThree, kThree) == kFive);

bool parity_matches_popcount() {
  for (std::uint32_t x = 0; x < kValueCount; ++x) {
    if ((clmul(x, kAllOnes) >> kTopBit) != (std::popcount(x) & 1)) return false;
  }
  return true;
}

std::uint32_t spread_by_loop(std::uint32_t x) {
  std::uint32_t spread = 0;
  for (int bit = 0; bit < kValueBits; ++bit) spread |= ((x >> bit) & 1) << (kSpreadFactor * bit);
  return spread;
}

bool square_spreads_bits() {
  for (std::uint32_t x = 0; x < kValueCount; ++x) {
    if (clmul(x, x) != spread_by_loop(x)) return false;
  }
  return true;
}

int main() {
  bool small = clmul(kThree, kThree) == kFive;
  bool parity = parity_matches_popcount();
  bool spread = square_spreads_bits();
  std::printf("clmul(3, 3) = %u\n", static_cast<unsigned>(clmul(kThree, kThree)));
  std::printf("parity from clmul equals popcount parity for all 16-bit values: %s\n", parity ? "yes" : "no");
  std::printf("clmul(x, x) spreads the bits of x for all 16-bit values: %s\n", spread ? "yes" : "no");
  return small && parity && spread ? 0 : 1;
}

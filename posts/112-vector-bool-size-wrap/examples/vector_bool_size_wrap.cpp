// A std::vector<bool> built from a size a few values below SIZE_MAX.
//
// libstdc++ rounds a bit count up to whole words with (n + 63) / 64. For the 63 sizes
// from SIZE_MAX - 62 to SIZE_MAX that addition wraps to a small number, so the vector
// gets zero words of storage but reports size() == n. A libstdc++ with the
// CVE-2026-114691 fix throws std::length_error instead.
//
// Default build: prints what each size does and exits 0. Nothing reads an element.
//   -DTOUCH_ELEMENT   also reads element 0 of a vector the library accepted
//                     (build with -fsanitize=address to see the report)

#include <cstddef>
#include <cstdio>
#include <limits>
#include <new>
#include <optional>
#include <stdexcept>
#include <vector>

namespace {

constexpr std::size_t kSizeMax = std::numeric_limits<std::size_t>::max();

// libstdc++ keeps the bits in unsigned long words.
constexpr std::size_t kWordBits = std::numeric_limits<unsigned long>::digits;

// The word count is (n + kRoundUp) / kWordBits, so n + kRoundUp wraps for every n
// that is less than kRoundUp below SIZE_MAX.
constexpr std::size_t kRoundUp = kWordBits - 1;
constexpr std::size_t kLastWrappingOffset = kRoundUp - 1;

constexpr std::size_t kProbeOffsets[] = {0, kLastWrappingOffset, kLastWrappingOffset + 1};

constexpr std::size_t kAcceptedCount = 1000;

#ifdef TOUCH_ELEMENT
void touch_first_element(const std::vector<bool>& bits) {
  std::printf("  reading element 0\n");
  std::printf("  element 0 = %d\n", static_cast<int>(bits[0]));
}
#endif

void report_construction(std::size_t count) {
  try {
    const std::vector<bool> bits(count);  // parentheses: braces would build one element
    std::printf("constructed, size() = %zu, capacity() = %zu\n", bits.size(), bits.capacity());
#ifdef TOUCH_ELEMENT
    touch_first_element(bits);
#endif
  } catch (const std::length_error& error) {
    std::printf("std::length_error: %s\n", error.what());
  } catch (const std::bad_alloc&) {
    std::printf("std::bad_alloc\n");
  }
}

// The practice: reject a count the library cannot hold before it reaches the constructor.
std::optional<std::vector<bool>> make_bits(std::size_t untrusted_count) {
  if (untrusted_count > std::vector<bool>{}.max_size()) {
    return std::nullopt;
  }
  return std::optional<std::vector<bool>>{std::in_place, untrusted_count};
}

void report_guarded(std::size_t untrusted_count) {
  const std::optional<std::vector<bool>> bits = make_bits(untrusted_count);
  if (bits) {
    std::printf("guarded %zu: accepted, size() = %zu\n", untrusted_count, bits->size());
  } else {
    std::printf("guarded %zu: rejected, above max_size() = %zu\n", untrusted_count,
                std::vector<bool>{}.max_size());
  }
}

}  // namespace

#if defined(__SANITIZE_ADDRESS__) && defined(TOUCH_ELEMENT)
// Compiler Explorer treats a non-zero exit as a failed run, so print the report and exit 0.
extern "C" const char* __asan_default_options() { return "exitcode=0"; }
#endif

int main() {
  std::setvbuf(stdout, nullptr, _IONBF, 0);  // keep earlier lines when a sanitizer aborts
  std::printf("%s\n", __VERSION__);
  for (const std::size_t offset : kProbeOffsets) {
    std::printf("n = SIZE_MAX - %zu: ", offset);
    report_construction(kSizeMax - offset);
  }
  report_guarded(kSizeMax);
  report_guarded(kAcceptedCount);
}

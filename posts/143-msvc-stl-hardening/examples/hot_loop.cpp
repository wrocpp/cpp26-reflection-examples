// verify: msvc-only
// A gather loop over std::span::operator[], for reading the code MSVC generates with and without STL hardening.
// Build, hardened: cl /std:c++latest /EHsc /O2 /D_MSVC_STL_HARDENING=1 /c   (Compiler Explorer: vcpp_v19_51_VS18_6_x64)
// Build, plain:    cl /std:c++latest /EHsc /O2 /c                           (same compiler)
// The switch lives in the build line only; the source has no macros. There is no main: read the assembly.
// On MSVC 19.51 (14.51.36246): hardened, the loop is scalar with one cmp/jae per element, and the failure block
// (mov ecx, 5 / int 41) sits after the ret. Plain, the loop is unrolled four elements at a time with SSE registers.
#include <cstddef>
#include <span>

int sum_at(std::span<const int> values, std::span<const std::size_t> indices) {
  int total = 0;
  for (const std::size_t index : indices) {
    total += values[index];
  }
  return total;
}

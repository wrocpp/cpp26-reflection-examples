// verify: msvc-only
// An out-of-bounds operator[] on std::vector and on std::span, with and without MSVC STL hardening.
// Build, hardened: cl /std:c++latest /EHsc /O2 /D_MSVC_STL_HARDENING=1   (Compiler Explorer: vcpp_v19_51_VS18_6_x64)
// Build, plain:    cl /std:c++latest /EHsc /O2                           (same compiler)
// The switch lives in the build line only; the source has no macros and reads none.
// Run with no argument: reads the last element of each, prints "vector[3] = 4" and "span[3] = 4", exits 0.
// Run with "vector" or "span": reads one past the end of that one.
//   hardened: the run stops at the read, exit code 0xC0000409 (3221226505), nothing printed for the read.
//   plain:    the read is undefined behaviour; on Compiler Explorer it printed a value and exited 0.
#include <cstddef>
#include <cstdio>
#include <span>
#include <string_view>
#include <vector>

namespace {

constexpr std::string_view kVectorArgument{"vector"};
constexpr std::string_view kSpanArgument{"span"};

struct Plan {
  std::size_t vector_index;
  std::size_t span_index;
};

// The default plan reads the last element of each; an argument moves one read one past the end.
Plan plan_for(std::string_view argument, std::size_t size) {
  const std::size_t last = size - 1;
  return Plan{
      .vector_index = argument == kVectorArgument ? size : last,
      .span_index = argument == kSpanArgument ? size : last,
  };
}

}  // namespace

int main(int argc, char** argv) {
  const std::vector<int> values{1, 2, 3, 4};
  const std::span<const int> view{values};
  const std::string_view argument = argc > 1 ? std::string_view{argv[1]} : std::string_view{};
  const Plan plan = plan_for(argument, values.size());

  std::printf("reading vector[%zu]\n", plan.vector_index);
  std::fflush(stdout);
  std::printf("vector[%zu] = %d\n", plan.vector_index, values[plan.vector_index]);

  std::printf("reading span[%zu]\n", plan.span_index);
  std::fflush(stdout);
  std::printf("span[%zu] = %d\n", plan.span_index, view[plan.span_index]);
}

#include "layout.hpp"
#include <cstdint>
struct Wide {
  [[=layout::hot]] std::uint64_t id;
  std::uint64_t pad_a[8];
  std::uint64_t pad_b[8];
  [[=layout::hot]] std::uint64_t count;
};
static_assert(layout::group<Wide, layout::hot_t>.span <= layout::constructive_size,
              layout::group_report<Wide, layout::hot_t, layout::constructive_size>);
int main() {}

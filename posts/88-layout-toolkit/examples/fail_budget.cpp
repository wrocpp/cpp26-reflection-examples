#include "layout.hpp"
#include <cstdint>
struct OrderBad { std::uint64_t price; char side; std::uint64_t quantity; bool active; };
static_assert(layout::padding_bytes<OrderBad> <= 8, layout::padding_report<OrderBad, 8>);
int main() {}

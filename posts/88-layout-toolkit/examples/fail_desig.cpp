#include "layout.hpp"
#include <cstdint>
struct OrderBad { std::uint64_t price; char side; std::uint64_t quantity; bool active; };
layout::reordered<OrderBad> r{.price = 1, .side = 's', .quantity = 2, .active = true};  // OrderBad's order
int main() {}

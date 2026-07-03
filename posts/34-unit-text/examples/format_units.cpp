// format_units.cpp  (~30 lines)
// Quantities carry their unit into std::format: no more "%f m/s" comments that
// drift out of sync with the code. mp-units plugs into std::format with format
// specs for the number and for the unit symbol, and the SAME quantity prints
// correctly after any conversion.
//
// Build (GCC 16.1 + mp-units@trunk):  g++ -std=c++23 -O2 format_units.cpp

#include <mp-units/systems/si.h>
#include <format>
#include <iostream>

using namespace mp_units;

int main() {
    using namespace mp_units::si::unit_symbols;

    quantity speed = 120.0 * km / h;

    // The unit travels with the value through std::format:
    std::cout << std::format("raw:        {}\n", speed);
    // Quantity format specs address the Number and Unit parts separately:
    std::cout << std::format("2 decimals: {::N[.2f]} and {::N[.2f]}\n", speed, speed.in(m / s));

    // Formatting is conversion-proof: change the unit, the text follows.
    quantity marathon = 42.195 * km;
    std::cout << std::format("marathon:   {} = {}\n", marathon, marathon.in(m));
}

// smoot.cpp  (~28 lines)
// In 1958, MIT pledges measured the Harvard Bridge with the body of Oliver
// Smoot: 364.4 smoots "plus one ear" (1 smoot = 5 ft 7 in = 1.7018 m).
// Extending mp-units with a brand-new unit, including its symbol and exact
// conversion factor, is ONE line. Everything else -- conversions, printing,
// dimensional analysis -- works immediately.
//
// Build (GCC 16.1 + mp-units@trunk):  g++ -std=c++23 -O2 smoot.cpp

#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// The one line: a named unit with an exact magnitude relative to the metre.
inline constexpr struct smoot final : named_unit<"smoot", mag_ratio<17'018, 10'000> * si::metre> {} smoot;

int main() {
    using namespace mp_units::si::unit_symbols;

    quantity bridge = 364.4 * smoot;

    std::cout << bridge << " = " << bridge.in(m) << "\n";

    // A new unit is a first-class citizen: derived quantities just work.
    quantity crossing_speed = bridge / (11.0 * si::minute);
    std::cout << "leisurely crossing: " << crossing_speed.in(km / h) << "\n";
}

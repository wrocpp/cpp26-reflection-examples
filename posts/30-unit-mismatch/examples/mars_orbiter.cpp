// mars_orbiter.cpp  (~25 lines)
// In 1999 the Mars Climate Orbiter disintegrated because ground software reported
// thruster impulse in pound-force seconds while the trajectory code expected
// newton seconds: a silent factor-4.45 error, undetected for months. With
// mp-units the mismatch is a TYPE property: mixing the two either converts
// exactly or refuses to compile. Assigning the raw number to a newton-second
// quantity does not compile at all.
//
// Build (GCC 16.1 + mp-units@trunk):  g++ -std=c++23 -O2 mars_orbiter.cpp

#include <mp-units/systems/si.h>
#include <mp-units/systems/yard_pound.h>
#include <iostream>

using namespace mp_units;

int main() {
    using namespace mp_units::si::unit_symbols;

    // What Lockheed's ground software produced: impulse in pound-force seconds.
    quantity small_forces = 100.0 * yard_pound::pound_force * si::second;

    // What NASA's trajectory code expected: newton seconds. With mp-units the
    // conversion is explicit, exact, and impossible to forget:
    std::cout << small_forces << " = " << small_forces.in(N * s) << "\n";

    // The 1999 bug, expressed in the type system, does not compile:
    // quantity<si::newton * si::second> impulse = 100.0;   // error: no conversion
}

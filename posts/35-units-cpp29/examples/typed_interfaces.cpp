// typed_interfaces.cpp  (~30 lines)
// What P3045 wants to put in the standard library: interfaces that say WHAT they
// take, not just which arithmetic type. A function taking quantity<si::metre>
// cannot receive feet, seconds, or a bare double by accident; the signature is
// the documentation and the compiler enforces it. mp-units is the reference
// implementation for the proposal targeting C++29.
//
// Build (GCC 16.1 + mp-units@trunk):  g++ -std=c++23 -O2 typed_interfaces.cpp

#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

// The signature carries the physics. No naming conventions, no comments.
constexpr quantity<si::metre / si::second> avg_speed(quantity<si::metre> distance,
                                                     quantity<si::second> time) {
    return distance / time;
}

int main() {
    using namespace mp_units::si::unit_symbols;

    // Callers may use ANY compatible unit; conversion to the interface is exact:
    std::cout << avg_speed(42'195.0 * m, 7'299.0 * s) << "\n";          // marathon WR pace
    std::cout << avg_speed((42.195 * km).in(m), (2.0 * h + 33.0 * si::minute).in(s)) << "\n";

    // And the mistake P3045 is designed to kill does not compile:
    // avg_speed(7'299.0 * s, 42'195.0 * m);   // error: swapped arguments caught by type
}

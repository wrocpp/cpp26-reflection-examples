// points_vs_deltas.cpp  (~30 lines)
// "The temperature is 21 degC" and "the temperature ROSE BY 21 degC" are different
// things, and adding two of the first kind is meaningless. mp-units models this
// with the affine space: quantity_point (absolute position against an origin)
// vs quantity (a delta). Point + delta and point - point work; point + point
// does not compile.
//
// Build (GCC 16.1 + mp-units@trunk):  g++ -std=c++23 -O2 points_vs_deltas.cpp

#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

int main() {
    using namespace mp_units::si::unit_symbols;

    quantity_point morning = point<deg_C>(17.0);   // an absolute reading
    quantity       warming = delta<deg_C>(4.5);    // a change

    quantity_point afternoon = morning + warming;  // point + delta -> point

    std::cout << "warmed by  " << (afternoon - morning) << "\n";  // point - point -> delta
    std::cout << "afternoon: " << afternoon.quantity_from(si::ice_point) << " above freezing\n";

    // The operations that are physical nonsense do not compile:
    // auto oops = morning + afternoon;        // error: cannot add two points
    // quantity q = morning;                   // error: a point is not a delta
}

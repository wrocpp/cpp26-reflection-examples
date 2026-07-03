// one_per_second.cpp  (~30 lines)
// Frequency (Hz), radioactivity (Bq), and angular velocity (rad/s) all have the
// same dimension: 1/s. A dimensions-only library treats them as interchangeable;
// adding a display refresh rate to a radiation reading type-checks fine. mp-units
// pioneered QUANTITY KIND safety: Hz and Bq are distinct kinds, so mixing them
// does not compile even though the dimensions match.
//
// Build (GCC 16.1 + mp-units@trunk):  g++ -std=c++23 -O2 one_per_second.cpp

#include <mp-units/systems/si.h>
#include <iostream>

using namespace mp_units;

int main() {
    using namespace mp_units::si::unit_symbols;

    quantity refresh_rate = 144.0 * Hz;   // kind: frequency
    quantity source_decay = 144.0 * Bq;   // kind: activity, same dimension 1/s

    std::cout << refresh_rate << " and " << source_decay
              << " share dimension 1/s, but:\n";

    // Same dimension, different kinds -> these do NOT compile:
    // auto nonsense  = refresh_rate + source_decay;   // error: no common quantity kind
    // quantity<Bq> q = refresh_rate;                  // error: Hz is not an activity

    // Within a kind, everything still works as physics says it should:
    quantity frame_time = (1.0 / refresh_rate).in(ms);
    std::cout << "one frame every " << frame_time << "\n";
}

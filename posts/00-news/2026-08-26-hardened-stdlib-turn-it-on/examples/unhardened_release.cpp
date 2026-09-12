// unhardened_release.cpp  (~20 lines)
// The same out-of-bounds access as bounds_check.cpp, built with the same
// optimisation level and without the hardening macro. Nothing checks the index,
// so operator[] reads whatever is past the end and the program carries on with
// a wrong value.
//
// The pair matters because GCC turns the libstdc++ assertions on by default at
// -O0. A debug build therefore catches this access and a release build of the
// identical source does not, which is the case for setting the macro in the
// build that ships rather than the one you debug in.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 unhardened_release.cpp
//
// The permalink for this file was minted BY HAND against g161, matching the
// compiler its sibling bounds_check.cpp runs on. shorten-examples.py cannot
// produce it: it hardcodes -std=c++26 -freflection and defaults to
// clang_bb_p2996, where the libstdc++ hardening never fires.
// verify: gcc-only
// verify: gcc-options: -std=c++26 -O2

#include <print>
#include <vector>

int main() {
    std::vector<int> v{10, 20, 30};
    std::println("v[1] = {} (in bounds)", v[1]);

    int bad = v[5];  // out of bounds: no check here, just a read
    std::println("v[5] = {} (out of bounds, and we are still running)", bad);

    return 0;
}

// The runner TU (a third TU). It declares ONE test of its own in the same namespace.
#include <cstdio>
#include "fixtures.hpp"
#include "rqt_tests.hpp"

namespace tests {
[[=rqt::test]] void in_runner() { rqt::expect(true); }
}  // namespace tests

static const rqt::registrar<^^tests, ^^fixtures> tests_of_this_tu{[] {}};

int main() {
    // What ONE runner sees by walking the namespace itself: only this TU's declarations.
    auto direct = rqt::discover<^^tests, ^^fixtures>();
    std::printf("direct walk of ^^tests in the runner TU: %zu case(s)\n", direct.size());
    std::printf("registry (fed by static initialisation): %zu case(s)\n", rqt::registry().size());
    return rqt::run(rqt::registry()) == 0 ? 0 : 1;
}

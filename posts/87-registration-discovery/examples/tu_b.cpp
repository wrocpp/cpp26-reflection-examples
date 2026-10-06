// TU B of the namespace `tests`.
#include "fixtures.hpp"
#include "rqt_tests.hpp"

namespace tests {
[[=rqt::test]] void subtracts() { rqt::expect(3 - 1 == 2); }
[[=rqt::test]] void by_name(int seed, int offset) { rqt::expect(seed == 42 && offset == 1000); }
[[=rqt::test, =rqt::xfail]] void known_bug() { rqt::expect(false, "known bug"); }
}  // namespace tests

static const rqt::registrar<^^tests, ^^fixtures> tests_of_this_tu{[] {}};  // the one line per TU

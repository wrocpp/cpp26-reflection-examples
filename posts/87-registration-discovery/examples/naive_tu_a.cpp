// TU A of the namespace `tests`.
#include "fixtures.hpp"
#include "naive_registrar.hpp"

namespace tests {
using fixtures::Fixture;
[[=rqt::test]] void adds() { rqt::expect(1 + 1 == 2); }
[[=rqt::test]] void parses(Fixture& f, [[=rqt::param(3), =rqt::param(4)]] int n) { rqt::expect(f.base + n >= 43); }
}  // namespace tests

static const rqt::registrar_naive<^^tests, ^^fixtures> tests_of_this_tu;  // the one line per TU

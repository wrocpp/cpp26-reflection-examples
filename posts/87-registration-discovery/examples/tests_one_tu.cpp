// (B) one TU: namespace tests, fixtures by type and by name, annotation parameterisation, pass/fail reporting.
#include "fixtures.hpp"
#include "rqt_tests.hpp"

namespace tests {
using fixtures::Fixture;
[[=rqt::test]] void adds() { rqt::expect(1 + 1 == 2); }
[[=rqt::test]] void parses(Fixture& f /* by TYPE */, [[=rqt::param(3), =rqt::param(4)]] int n) { rqt::expect(f.base + n >= 43, "base + n"); }
[[=rqt::test]] void by_name(int seed /* by NAME */, int offset) { rqt::expect(seed == 42 && offset == 1000, "seed/offset"); }
[[=rqt::test, =rqt::xfail]] void known_bug() { rqt::expect(false, "known bug"); }
void helper_not_a_test() {}
#ifdef DEMO_FAIL
[[=rqt::test]] void fails_on_purpose(Fixture& f) { rqt::expect(f.base == 0, "f.base is 40, not 0"); }
#endif
}  // namespace tests

int main() { return rqt::run(rqt::discover<^^tests, ^^fixtures>()) == 0 ? 0 : 1; }

// Forgetting the per-TU tag must not compile (registrar has no default constructor).
#include "fixtures.hpp"
#include "rqt_tests.hpp"
namespace tests { [[=rqt::test]] void t() {} }
static const rqt::registrar<^^tests, ^^fixtures> tests_of_this_tu;
int main() {}

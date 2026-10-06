#include "fixtures.hpp"
#include "rqt_tests.hpp"
namespace tests { [[=rqt::test]] void ambiguous(int count) { (void)count; } }
int main() { return rqt::run(rqt::discover<^^tests, ^^fixtures>()); }

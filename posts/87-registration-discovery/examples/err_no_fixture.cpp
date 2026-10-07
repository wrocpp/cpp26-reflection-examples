#include "fixtures.hpp"
#include "rqt_tests.hpp"
struct Unknown {};
namespace tests { [[=rqt::test]] void needs_unknown(Unknown&) {} }
int main() { return rqt::run(rqt::discover<^^tests, ^^fixtures>()); }

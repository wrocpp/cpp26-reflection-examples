// Bonus: "collect tests in this translation unit" = walk the global namespace recursively.
#include <cstdio>
#include <meta>
#include <vector>
#include "rqt_tests.hpp"

namespace one { [[=rqt::test]] void a() {} namespace deep { [[=rqt::test]] void b() {} } }
namespace two { [[=rqt::test]] void c() {} void not_a_test() {} }
[[=rqt::test]] void at_global() {}

consteval void collect_tests(std::meta::info ns, std::vector<std::meta::info>& out) {
    for (std::meta::info m : std::meta::members_of(ns, std::meta::access_context::unchecked())) {
        if (std::meta::is_function(m) && std::meta::has_identifier(m) && rqt::detail::has_annotation_of(m, ^^rqt::test_t)) out.push_back(m);
        else if (std::meta::is_namespace(m) && std::meta::has_identifier(m)) {
            std::string_view n = std::meta::identifier_of(m);
            if (n != "std" && n != "rqt" && n != "__gnu_cxx" && n != "__cxxabiv1") collect_tests(m, out);
        }
    }
}
consteval std::vector<std::meta::info> all_tests() { std::vector<std::meta::info> v; collect_tests(^^::, v); return v; }

int main() {
    std::size_t n = 0;
    template for (constexpr auto f : std::define_static_array(all_tests())) {
        std::printf("found %s\n", std::string(std::meta::identifier_of(f)).c_str());
        [:f:]();
        ++n;
    }
    std::printf("TU-wide discovery: %zu tests\n", n);
}

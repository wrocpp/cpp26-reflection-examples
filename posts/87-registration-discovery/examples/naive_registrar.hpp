// The TRAP: a registrar template with NO per-TU tag. `registrar_naive<^^tests, ^^fixtures>` is the same
// entity in every TU, but its constructor body depends on what `tests` contains in THAT TU.
#pragma once
#include "rqt_tests.hpp"
namespace rqt {
template <std::meta::info TestNs, std::meta::info Fix>
struct registrar_naive {
    registrar_naive() {
        for (auto& c : detail::collect<TestNs, Fix, void>()) registry().push_back(std::move(c));
    }
};
}  // namespace rqt

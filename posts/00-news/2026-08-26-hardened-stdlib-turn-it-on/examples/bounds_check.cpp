// bounds_check.cpp  (~26 lines)
// The hardened standard library turns the undefined behavior of an
// out-of-bounds operator[] into a defined, immediate stop. Building libstdc++
// with -D_GLIBCXX_ASSERTIONS (libc++ has -D_LIBCPP_HARDENING_MODE) adds a bounds
// check to vector::operator[], span, string_view and friends. Google reported
// this class of hardening cut segfault rates by about a third in production.
// The check calls std::abort on failure; this demo installs a SIGABRT handler
// only so it can exit 0 for Compiler Explorer -- in your process it just stops.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 -D_GLIBCXX_ASSERTIONS bounds_check.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++26 -O2 -D_GLIBCXX_ASSERTIONS

#include <csignal>
#include <cstdlib>
#include <print>
#include <vector>

[[noreturn]] void on_abort(int) {
    std::println("hardened stdlib stopped the out-of-bounds access");
    std::fflush(stdout);  // flush before _Exit: the abort skips normal teardown
    std::_Exit(0);
}

int main() {
    std::signal(SIGABRT, on_abort);
    std::vector<int> v{10, 20, 30};
    std::println("v[1] = {} (in bounds)", v[1]);
    std::fflush(stdout);
    int bad = v[5];  // out of bounds: the hardened operator[] traps instead of reading garbage
    std::println("unreachable: {}", bad);
    return 0;
}

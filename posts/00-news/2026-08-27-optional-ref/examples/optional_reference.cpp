// optional_reference.cpp  (~18 lines)
// std::optional<T&> was ill-formed from C++17 until C++26 (P2988), because the
// committee could not settle whether assignment should rebind the reference or
// assign through it. C++26 answers: assignment REBINDS, matching
// reference_wrapper and making the type usable as a value. The specialization
// stores a pointer, so it costs exactly what the T* you were writing costs.
//
// Availability note: this compiles and runs on GCC 16.1, but libstdc++ does not
// yet define __cpp_lib_optional_ref, so do not feature-test for it yet.
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 optional_reference.cpp

#include <optional>
#include <print>
#include <string>

int main() {
    std::string s = "hello";
    std::optional<std::string&> r = s;   // binds to s, no copy
    std::println("has={} value={}", r.has_value(), *r);

    *r += " world";                      // writes through to s
    std::println("s now = {}", s);

    std::optional<std::string&> empty;
    std::println("empty has={}", empty.has_value());
}

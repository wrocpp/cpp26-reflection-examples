// concat.cpp  (~25 lines)
// P2591R5 "Concatenation of strings and string views". Before C++26, `string + string_view`
// did not compile -- there was no operator+ for the mix, so you had to materialize a
// temporary std::string first. C++26 adds free (non-friend) operator+ templates that accept
// anything convertible to string_view, in BOTH orders: string + string_view and
// string_view + string.
//
// Compile (GCC 16.1):              g++-16 -std=c++26 concat.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ concat.cpp -o demo

#include <print>
#include <string>
#include <string_view>

int main() {
    std::string      hello{"hello "};
    std::string_view world{"world"};

    // Pre-C++26 this line was ill-formed: no operator+(string, string_view).
    std::string greeting = hello + world;                          // C++26
    // Both orders work, so you can bracket a std::string with views directly.
    std::string framed = std::string_view{"<< "} + greeting + std::string_view{" >>"};

    std::println("{}", greeting);
    std::println("{}", framed);
}

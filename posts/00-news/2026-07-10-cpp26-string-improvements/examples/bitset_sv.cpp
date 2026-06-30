// bitset_sv.cpp  (~22 lines)
// P2697R1 "Interfacing bitset with string_view". C++26 adds a std::bitset constructor taking
// a std::string_view, so you can build a bitset from a view without a temporary std::string
// and without the data()/null-termination footguns of the old char* path.
//
// Compile (GCC 16.1):              g++-16 -std=c++26 bitset_sv.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ bitset_sv.cpp -o demo

#include <bitset>
#include <print>
#include <string_view>

int main() {
    std::string_view bits{"101010"};

    // Pre-C++26: std::bitset<6> b{std::string{bits}};  // temporary string just to parse.
    std::bitset<6> b{bits};                             // C++26: straight from the view.

    std::println("value = {}", b.to_ulong());
    std::println("count = {}", b.count());
}

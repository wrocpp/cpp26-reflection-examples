// stringstream_sv.cpp  (~22 lines)
// P2495R3 "Interfacing stringstreams with string_view". C++26 gives the stringstream family
// (basic_stringbuf / istringstream / ostringstream / stringstream) constructors that take a
// std::string_view. Before C++26 you had to build a temporary std::string from the view just
// to feed a stream.
//
// Compile (GCC 16.1):              g++-16 -std=c++26 stringstream_sv.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ stringstream_sv.cpp -o demo

#include <print>
#include <sstream>
#include <string_view>

int main() {
    std::string_view csv{"12 34 56"};

    // Pre-C++26: std::istringstream in{std::string{csv}};  // forced a copy of the view.
    std::istringstream in{csv};                             // C++26: construct straight from the view.

    int a = 0, b = 0, c = 0;
    in >> a >> b >> c;
    std::println("sum = {}", a + b + c);
}

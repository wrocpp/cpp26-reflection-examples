// dangling_pair.cpp
// libstdc++ on GCC trunk makes std::pair and std::tuple construction ill-formed
// when a reference member would bind to a temporary (PR108822). GCC 16.2 rejects
// it only in C++20 and later; before the change, C++17 and lower needed
// -D_GLIBCXX_DEBUG to get the same error.
//
// Without -DSHOW_DANGLING the program builds a pair and a tuple that refer to a
// named string, prints the compiler version, and exits 0 on every compiler.
// With -DSHOW_DANGLING it also builds the two objects below, which bind a
// reference to a temporary std::string.
//
// Expected with -std=c++17 -DSHOW_DANGLING:
//   GCC 16.2:  compiles, no warning
//   GCC trunk: error: static assertion failed: std::pair constructor creates a
//              dangling reference (and the same for std::tuple)
//
// Compile: g++ -std=c++17 [-DSHOW_DANGLING] dangling_pair.cpp
// verify: ce-only

#include <cstdio>
#include <string>
#include <tuple>
#include <utility>

int main() {
    const std::string name{"x"};
    std::pair<const std::string&, int> ok_pair(name, 1);
    std::tuple<const std::string&, int> ok_tuple(name, 2);
#ifdef SHOW_DANGLING
    std::pair<const std::string&, int> bad_pair("x", 1);
    std::tuple<const std::string&, int> bad_tuple("x", 2);
#endif
    std::printf("%s\n", __VERSION__);
    std::printf("pair %s %d, tuple %s %d\n", ok_pair.first.c_str(), ok_pair.second,
                std::get<0>(ok_tuple).c_str(), std::get<1>(ok_tuple));
    return 0;
}

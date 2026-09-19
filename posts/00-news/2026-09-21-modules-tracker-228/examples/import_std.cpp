// import_std.cpp
//
// The whole standard library through one import, in a single file.
//
// This compiles on Compiler Explorer with GCC 16 only when the compiler is
// told to build the std module itself: -fmodules alone fails to find it, and
// Clang 20.1 reports "module 'std' not found" for the same file, because its
// std module has to be built by the build system first.
//
// Minted by hand against g162: shorten-examples.py hardcodes
// -std=c++26 -freflection and defaults to clang_bb_p2996.
//
// Compile (GCC 16.2): g++ -std=c++26 -fmodules --compile-std-module import_std.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++26 -fmodules --compile-std-module

import std;

int main() {
    std::vector<int> v{3, 1, 2};
    std::ranges::sort(v);
    std::println("sorted: {}", v);

    std::map<std::string, int> m{{"modules", 228}, {"tracked", 2613}};
    std::println("{}", m);
}

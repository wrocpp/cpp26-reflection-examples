// compile_time_format.cpp  (~16 lines)
// {fmt} can do the whole format at COMPILE time. FMT_COMPILE parses the format
// string during compilation and emits straight-line formatting code, so the
// call below is a constant expression: no parsing at runtime, and the format
// string cannot be wrong at runtime because it was checked already.
//
// verify: ce-libs: fmt
// Compile (GCC 16.1): g++ -std=c++20 -O2 compile_time_format.cpp -lfmt
// verify: gcc-only
// verify: gcc-options: -std=c++20 -O2

#include <fmt/compile.h>
#include <fmt/format.h>
#include <cstdio>

constexpr auto make_tag() {
    return fmt::format(FMT_COMPILE("{}-{:04d}"), "build", 42);
}

int main() {
    auto s = make_tag();
    std::printf("compile-time formatted: %s\n", s.c_str());
    std::printf("fmt version: %d\n", FMT_VERSION);
    return 0;
}

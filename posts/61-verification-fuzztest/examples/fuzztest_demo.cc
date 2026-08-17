// fuzztest_demo.cc  (~45 lines)
// Google FuzzTest turns one assertion into two tools: a bounded property test
// that runs with your normal suite, and a coverage-guided fuzzer you invoke on
// demand with --fuzz. The property below says Decode(Encode(s)) == s for ANY
// string, and the encoder deliberately gets that wrong for runs of ten or more
// identical characters, because the run length is written as a single digit.
//
// FuzzTest does NOT run on Compiler Explorer: it needs a multi-file build and a
// coverage-instrumented binary. Build it locally (Linux, clang) with the
// CMakeLists.txt sitting next to this file:
//
//   cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
//         -DFUZZTEST_FUZZING_MODE=on
//   cmake --build build -j
//   ./build/fuzztest_demo                                    # unit mode
//   ./build/fuzztest_demo --fuzz=RleTest.RoundTripsAnyString # fuzzing mode

#include <string>
#include "gtest/gtest.h"
#include "fuzztest/fuzztest.h"

// A run-length encoder with a deliberate bug: the run length is emitted as one
// character, so any run of ten or more does not survive the round trip.
std::string Encode(const std::string& in) {
    std::string out;
    for (size_t i = 0; i < in.size();) {
        size_t j = i;
        while (j < in.size() && in[j] == in[i]) ++j;
        size_t run = j - i;
        out += in[i];
        out += static_cast<char>('0' + run);  // BUG: only one digit fits
        i = j;
    }
    return out;
}

std::string Decode(const std::string& in) {
    std::string out;
    for (size_t i = 0; i + 1 < in.size(); i += 2) {
        int run = in[i + 1] - '0';
        for (int k = 0; k < run; ++k) out += in[i];
    }
    return out;
}

// An ordinary unit test. It passes, and it proves nothing about the general case.
TEST(RleTest, RoundTripsASimpleString) {
    EXPECT_EQ(Decode(Encode("aabbb")), "aabbb");
}

// The same assertion as a property, over every string rather than one.
void RoundTripsAnyString(const std::string& s) {
    EXPECT_EQ(Decode(Encode(s)), s);
}
FUZZ_TEST(RleTest, RoundTripsAnyString);

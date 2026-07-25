// unit_test.cpp  (~20 lines)
// GoogleTest is a Compiler Explorer library, so a real test binary builds and
// RUNS in the browser: add `googletest` from the libraries panel, turn on the
// execution pane, and the familiar [ RUN ] / [ OK ] report prints back. The
// exit code is the suite's result, which is why a green run exits 0.
// GoogleTest 1.17 requires C++17 or later.
//
// Compile (GCC 16.1): g++ -std=c++17 -O2 -pthread unit_test.cpp -lgtest -lgtest_main
// verify: ce-libs: googletest
// verify: gcc-only
// verify: gcc-options: -std=c++17 -O2 -pthread

#include <gtest/gtest.h>

int add(int a, int b) { return a + b; }

TEST(AddTest, HandlesPositive) { EXPECT_EQ(add(2, 3), 5); }
TEST(AddTest, HandlesNegative) { EXPECT_EQ(add(-2, 3), 1); }

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

# Minimal FuzzTest project. Fetches FuzzTest (and GoogleTest, Abseil, RE2)
# and wires the fuzzing flags. Requires clang; verified on Ubuntu 24.04.
cmake_minimum_required(VERSION 3.19)
project(fuzzdemo CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  fuzztest
  GIT_REPOSITORY https://github.com/google/fuzztest.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fuzztest)

enable_testing()
include(GoogleTest)

fuzztest_setup_fuzzing_flags()

add_executable(fuzztest_demo fuzztest_demo.cc)
link_fuzztest(fuzztest_demo)
gtest_discover_tests(fuzztest_demo)

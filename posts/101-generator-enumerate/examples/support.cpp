// Which standard library is this, and does it define the feature-test macros for std::generator and
// views::enumerate? Needs only <version>, so it also compiles where <generator> does not exist.
// The #pragma message lines show the answer at compile time for compilers that cannot run the program.
#include <version>

#define STRINGIFY_IMPL(text) #text
#define STRINGIFY(text) STRINGIFY_IMPL(text)

#if defined(_MSC_FULL_VER)
#define COMPILER "MSVC " STRINGIFY(_MSC_FULL_VER)
#else
#define COMPILER __VERSION__
#endif

#if defined(_LIBCPP_VERSION)
#define LIBRARY "libc++ " STRINGIFY(_LIBCPP_VERSION)
#elif defined(_GLIBCXX_RELEASE)
#define LIBRARY "libstdc++ " STRINGIFY(_GLIBCXX_RELEASE)
#elif defined(_MSVC_STL_VERSION)
#define LIBRARY "MSVC STL " STRINGIFY(_MSVC_STL_VERSION)
#else
#define LIBRARY "unknown"
#endif

#ifdef __cpp_lib_generator
#define GENERATOR_MACRO STRINGIFY(__cpp_lib_generator)
#else
#define GENERATOR_MACRO "undefined"
#endif

#ifdef __cpp_lib_ranges_enumerate
#define ENUMERATE_MACRO STRINGIFY(__cpp_lib_ranges_enumerate)
#else
#define ENUMERATE_MACRO "undefined"
#endif

#if __has_include(<generator>)
#define GENERATOR_HEADER "found"
#else
#define GENERATOR_HEADER "not found"
#endif

#pragma message("compiler: " COMPILER)
#pragma message("library: " LIBRARY)
#pragma message("__cpp_lib_generator = " GENERATOR_MACRO)
#pragma message("__cpp_lib_ranges_enumerate = " ENUMERATE_MACRO)
#pragma message("<generator> header: " GENERATOR_HEADER)

#include <cstdio>

int main() {
  std::puts("compiler: " COMPILER);
  std::puts("library: " LIBRARY);
  std::puts("__cpp_lib_generator = " GENERATOR_MACRO);
  std::puts("__cpp_lib_ranges_enumerate = " ENUMERATE_MACRO);
  std::puts("<generator> header: " GENERATOR_HEADER);
}

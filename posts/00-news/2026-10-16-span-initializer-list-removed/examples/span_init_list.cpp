// span_init_list.cpp
// Which std::span constructors a braced call can reach. P2447 added
// span(initializer_list<value_type>) for C++26; P4144R1 (LWG 4520) removes it
// again, and the C++ working draft no longer has it.
//
// The demo prints:
//   - whether <version> defines __cpp_lib_span_initializer_list
//   - whether span<const T, 2>{{a, b, c}} compiles. The array constructor
//     rejects three elements for an extent of 2, so only an initializer_list
//     constructor can accept it
//   - the size you get from s{ptr, n}, s(ptr, n) and a braced list, for
//     T = bool and T = int. Every pointer points into a real array.
//
// Expected: libstdc++ 15 (GCC 15.1 to 15.3) and libc++ up to 22.1 have the constructor
// (macro 202311L): GCC 15 warns about the narrowing and gives size=2 for
// span<const bool>, clang rejects the line. libstdc++ 16 and libc++ 23.1 do not have it.
//
// Compile: g++ -std=c++26 -O2 span_init_list.cpp
//          clang++ -std=c++26 -stdlib=libc++ -O2 span_init_list.cpp
// verify: ce-only

#include <cstddef>
#include <cstdio>
#include <span>
#include <version>

template <class T>
concept braced_list_makes_span = requires { std::span<const T, 2>{{T{}, T{}, T{}}}; };

template <class T>
void report(const char* name, const T* data, std::size_t n) {
    std::span<const T> braced_ptr_n{data, n};   // the call the paper is about
    std::span<const T> paren_ptr_n(data, n);    // always the (pointer, count) constructor
    std::printf("%s: s{ptr, n} size=%zu  s(ptr, n) size=%zu  braced-list-ctor=%d\n",
                name, braced_ptr_n.size(), paren_ptr_n.size(),
                static_cast<int>(braced_list_makes_span<T>));
}

int main() {
#ifdef __cpp_lib_span_initializer_list
    std::printf("__cpp_lib_span_initializer_list = %ld\n", static_cast<long>(__cpp_lib_span_initializer_list));
#else
    std::printf("__cpp_lib_span_initializer_list not defined\n");
#endif
    static constexpr bool flags[] = {true, false, true};
    static constexpr int  nums[]  = {10, 20, 30};
    report<bool>("span<const bool>", flags, 3);
    report<int>("span<const int> ", nums, 3);
    return 0;
}

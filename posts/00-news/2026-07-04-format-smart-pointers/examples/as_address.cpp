// as_address.cpp  (~30 lines)
// "Format a smart pointer the way operator<< does": print the managed object's
// address. std::format has no formatter for unique_ptr/shared_ptr (and none for
// arbitrary T*), so you cast the result of .get() to void* yourself -- void* is
// the one pointer type std::format will format.
//
// C++26 (P2510 "Formatting pointers") then lets you dress that void* up directly
// -- uppercase with {:P} and zero-padding with a width -- without the old trick
// of reinterpret_cast-ing to an integer first.
//
// Compile (GCC 16.1):              g++-16 -std=c++26 as_address.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ as_address.cpp -o demo

#include <memory>
#include <print>

int main() {
    auto u = std::make_unique<int>(42);

    // operator<< prints the managed object's address. For std::format you cast
    // .get() to void* yourself; an arbitrary int* is not formattable.
    const void* addr = static_cast<const void*>(u.get());
    std::println("address is non-null: {}", addr != nullptr);

    // C++26 P2510 formats a void* directly (no integer cast). Shown on a fixed
    // value so the output is reproducible -- real addresses vary per run.
    const void* p = reinterpret_cast<const void*>(0xCAFEF00DULL);
    std::println("plain:    {}",     p);   // 0xcafef00d
    std::println("upper:    {:P}",   p);   // 0XCAFEF00D
    std::println("zero-pad: {:018}", p);   // 0x00000000cafef00d
}

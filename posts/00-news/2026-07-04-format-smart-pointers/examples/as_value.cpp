// as_value.cpp  (~25 lines)
// "Format what the smart pointer POINTS AT": usually you do not want the
// address at all -- you want the value. std::format the dereferenced pointee
// (which must itself be formattable), guarding against null.
//
// This is why a built-in "print the address" formatter would be low value:
// the pointee is almost always what you actually mean.
//
// Compile (GCC 16.1):              g++-16 -std=c++26 as_value.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ as_value.cpp -o demo

#include <format>
#include <memory>
#include <print>
#include <string>

int main() {
    auto p = std::make_unique<int>(42);
    std::unique_ptr<int> n;   // empty

    // Null-safe: dereference only when the pointer is engaged.
    auto show = [](const auto& q) {
        return q ? std::format("{}", *q) : std::string("(null)");
    };

    std::println("p -> {}", show(p));   // p -> 42
    std::println("n -> {}", show(n));   // n -> (null)
}

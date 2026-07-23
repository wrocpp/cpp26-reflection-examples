// handrolled.cpp  (~30 lines)
// This is the pattern P4313 ("Bitmask operations for enums", Guterman + Williams,
// 2026-07 mailing) wants to delete. A scoped enum used as a flag set needs a
// full set of operator overloads written by hand, once per enum, just to get
// |, &, ~ back. The paper proposes an opt-in attribute -- enum class
// [[std::bitmask_type]] Permission { ... } -- that generates all of this and
// keeps the type distinct (no implicit conversion between different bitmasks).
// This file compiles today on any C++23 compiler; the attribute does not exist
// yet, so it shows the boilerplate the proposal removes.
//
// Compile (GCC 16.1): g++ -std=c++23 -O2 handrolled.cpp

#include <print>
#include <utility>

enum class Permission : unsigned { None = 0, Read = 1, Write = 2, Execute = 4 };

// The boilerplate: eight-ish operators, per bitmask enum, forever.
constexpr Permission operator|(Permission a, Permission b) {
    return Permission(std::to_underlying(a) | std::to_underlying(b));
}
constexpr Permission operator&(Permission a, Permission b) {
    return Permission(std::to_underlying(a) & std::to_underlying(b));
}
constexpr Permission operator~(Permission a) {
    return Permission(~std::to_underlying(a) & 0b111u);
}
constexpr bool holds(Permission set, Permission flag) {
    return std::to_underlying(set & flag) != 0;
}

int main() {
    Permission p = Permission::Read | Permission::Write;
    std::println("read   : {}", holds(p, Permission::Read));
    std::println("write  : {}", holds(p, Permission::Write));
    std::println("execute: {}", holds(p, Permission::Execute));
    std::println("value  : {}", std::to_underlying(p));
}

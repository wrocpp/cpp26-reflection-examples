// expansion_named_range.cpp  -- GCC 16.1 rejects this; clang-p2996 accepts it.
//
// A C++26 expansion statement can iterate a compile-time range. Give it the
// range as a NAMED constexpr local and GCC 16.1 rejects the program:
//
//     error: 'members' is not a constant
//
// Write the identical expression inline in the same statement and GCC accepts
// it. Same value, same initializer, same constexpr-ness; only the name differs.
// clang-p2996 accepts both.
//
// Build with -DNAMED_RANGE to see the rejection; without it you get the
// workaround. (Note for anyone copying this: `#ifdef NAMED_RANGE` does not
// work, because undefined identifiers are 0 in a preprocessor condition, so
// both branches compare equal. Ask whether a macro is defined instead.)
//
// Compile (gcc):   g++ -std=c++26 -freflection -DNAMED_RANGE
// Compile (clang): clang++ -std=c++26 -freflection-latest -stdlib=libc++
// verify: gcc-and-clang
// verify: gcc-options: -std=c++26 -freflection -O1

#include <meta>
#include <print>

struct Point { int x; int y; };

template <typename T>
void dump(T const& obj) {
    constexpr auto ctx = std::meta::access_context::unchecked();

#ifdef NAMED_RANGE
    // GCC 16.1: error: 'members' is not a constant
    constexpr auto members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, ctx));
    template for (constexpr auto m : members) {
        std::println("  {} = {}", std::meta::identifier_of(m), obj.[: m :]);
    }
#else
    // Accepted by both: the same expression, unnamed.
    template for (constexpr auto m : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        std::println("  {} = {}", std::meta::identifier_of(m), obj.[: m :]);
    }
#endif
}

int main() {
    Point p{1, 2};
    std::println("Point:");
    dump(p);
}

// reflect_probe.cpp  (~50 lines)
// A one-file "does my compiler actually have C++26 reflection (and contracts)?"
// probe. It prints the relevant feature-test macros, then runs a minimal
// reflection walk -- so the SAME source visibly behaves across every toolchain
// that claims support, and fails to compile on every one that does not.
//
// Compile (GCC 16.1 or later):     g++-16 -std=c++26 -freflection reflect_probe.cpp -o probe
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -freflection-latest -stdlib=libc++ reflect_probe.cpp -o probe
//
// As of June 2026 this compiles on exactly two toolchains: mainline GCC 16.1
// (-freflection) and Bloomberg's experimental clang-p2996 fork
// (-freflection-latest). Mainline Clang/libc++ and MSVC reject it outright --
// they ship no <experimental/meta> and no ^^ operator yet. That gap is the news.

#include <experimental/meta>
#include <print>
#include <string_view>

struct User {
    std::string_view name;
    int age;
    bool admin;
};

template <typename T>
consteval int field_count() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    int n = 0;
    for (auto m : std::meta::nonstatic_data_members_of(^^T, ctx)) {
        (void)m;
        ++n;
    }
    return n;
}

int main() {
    std::println("C++26 reflection / contracts probe");
    std::println("----------------------------------");

#ifdef __cpp_impl_reflection
    std::println("__cpp_impl_reflection = {}", __cpp_impl_reflection);
#else
    std::println("__cpp_impl_reflection = (undefined)");
#endif

#ifdef __cpp_contracts
    std::println("__cpp_contracts       = {}", __cpp_contracts);
#else
    std::println("__cpp_contracts       = (undefined on this toolchain)");
#endif

    // If this line compiled at all, reflection works here: walk a struct.
    std::println("User has {} reflected data members.", field_count<User>());
}

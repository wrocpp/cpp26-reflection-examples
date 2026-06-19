// init_profile_lint.cpp  (~55 lines)
// A reflection-driven "initialization risk" report -- the structural shadow of
// P4222 "An initialization profile" (R0, 2026-05-09), one of the memory-safety
// Profiles papers carried into the C++29 cycle opened at Brno (June 2026).
//
// P4222 is a LANGUAGE profile: once enabled, reading an uninitialized object
// becomes ill-formed -- the compiler enforces it. That is not something a user
// library can do today. But reflection already lets us REPORT the risk surface:
// walk a type's data members and flag the scalars/pointers that aggregate
// default-initialization would leave indeterminate (exactly the values P4222's
// profile is designed to stop you reading). Class-type members initialize
// themselves, so they are not flagged.
//
// Think of this as the "today" companion to P4222: reflection points at the
// fields to give a default member initializer; the C++29 profile makes the
// actual uninitialized READ a hard error.
//
// Compile (GCC 16.1 or later):     g++-16 -std=c++26 -freflection init_profile_lint.cpp -o lint
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -freflection-latest -stdlib=libc++ init_profile_lint.cpp -o lint

#include <experimental/meta>
#include <array>
#include <cstdint>
#include <print>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct WireHeader {
    std::uint32_t magic;       // scalar: indeterminate under default-init
    int           length;      // scalar: indeterminate under default-init
    char*         payload;     // pointer: indeterminate (and a dangling risk)
    std::string   trailer;     // class type: initializes itself, safe
    std::array<std::byte, 4> tag;  // class type: initializes itself, safe
};

template <typename T>
consteval auto init_risk_report() {
    std::vector<std::pair<char const*, char const*>> risky;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        using FieldT = [:std::meta::type_of(m):];
        if constexpr (std::is_scalar_v<FieldT>) {  // scalars include pointers
            risky.push_back({
                std::define_static_string(std::meta::identifier_of(m)),
                std::define_static_string(
                    std::meta::display_string_of(std::meta::type_of(m))),
            });
        }
    }
    return risky;
}

int main() {
    constexpr auto report = std::define_static_array(init_risk_report<WireHeader>());
    std::println("WireHeader: members left indeterminate by default-init");
    std::println("(give each a default member initializer, or rely on the");
    std::println(" P4222 profile in C++29 to make the uninitialized read fail):");
    for (auto const& [name, type] : report) {
        std::println("  {:<10} : {}", name, type);
    }
}

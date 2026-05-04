// A reflection-driven "basic safety profile" verifier. Inspects a
// class's data members at compile time and refuses to compile if any
// member violates per-type rules:
//   - No raw owning pointers (T*, T const*).
//   - No C-style arrays (a bare T[N] member is unbounded at the
//     interface level; std::array<T, N> is fine).
//
// Why this matters in 2026: the [[profiles::enforce]] attribute (P3081
// Sutter, P3589 Dos Reis framework, P3984 Stroustrup type-safety
// profile) was DEFERRED from C++26 to C++29 at the Croydon meeting
// (March 2026). For the per-class structural rules above, reflection
// already gives us the primitives to enforce equivalent constraints
// from a user library TODAY -- without waiting for the C++29 attribute,
// without leaving stock clang or GCC.
//
// Aligned with C++ Core Guidelines I.11 (never transfer ownership by
// raw pointer), F.7 / F.20 / F.43 (ownership rules), and the spirit of
// the MISRA C++:2023 ban on raw pointer arithmetic (the unified
// MISRA + AUTOSAR successor since the 2019 consortium merger).
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/profile && /tmp/profile" \
//     posts/toolset/memory-safety-cpp26-and-beyond/examples/reflect-profile.cpp

#include <experimental/meta>
#include <array>
#include <cstddef>
#include <memory>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

// Reflection-driven profile check. Walks T's nonstatic data members
// at compile time; static_assert fires per violation. Returns a bool
// so callers can use it as a value if they want to gate templates.
template <typename T>
consteval bool meets_basic_safety_profile() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        using FieldT = [:std::meta::type_of(m):];
        static_assert(!std::is_pointer_v<FieldT>,
                      "safety profile violation: raw pointer member");
        static_assert(!std::is_array_v<FieldT>,
                      "safety profile violation: C-style array member");
    }
    return true;
}

// Compliant: every member is owning + type-safe + lifetime-tracked.
struct GoodSession {
    std::string                username;
    std::vector<std::byte>     buffer;
    std::unique_ptr<int>       counter;
    std::array<std::byte, 16>  tag;
};

static_assert(meets_basic_safety_profile<GoodSession>(),
              "GoodSession should pass the basic safety profile");

// To see the profile fire in real time, uncomment one of these:
//
// struct BadSessionRawPtr  { int*    count;   };  // raw pointer
// struct BadSessionCArr    { char    name[64]; };  // C-style array
//
// static_assert(meets_basic_safety_profile<BadSessionRawPtr>());  // FAILS
// static_assert(meets_basic_safety_profile<BadSessionCArr>());    // FAILS

int main() {
    std::println("GoodSession passes the basic safety profile check.");
    std::println("Try uncommenting BadSession* in the source to see the");
    std::println("static_assert fire with the profile-violation reason.");
}

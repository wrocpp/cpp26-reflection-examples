// Today (no P3081 profiles yet): a reflection-driven "basic safety
// profile" verifier. Inspects a class's data members at compile time
// and refuses to compile if any member violates the profile rules:
//   - No raw owning pointers (T*, T const*).
//   - No C-style arrays (a bare T[N] member is unbounded at the
//     interface level; std::array<T, N> is fine).
//
// This is the pattern P3081 (Sutter) and P3274 (Stroustrup) will
// formalise as compiler-enforced subsets once they ship in clang-p2996
// and GCC. Until then, reflection already gives us the introspection
// primitives to enforce equivalent rules in user libraries today --
// not at the language level, but at the static_assert level.
//
// Aligned with C++ Core Guidelines I.11 (never transfer ownership by
// raw pointer), F.7 / F.20 / F.43 (ownership rules), and the spirit of
// MISRA C++ 2023 / AUTOSAR C++14 ban on raw pointer arithmetic.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/profile && /tmp/profile" \
//     posts/toolset/safety-profiles-2026/examples/reflect-profile.cpp

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

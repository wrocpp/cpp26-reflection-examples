// "Reflection today" demo for qualified-compilers: a compile-time
// MISRA C++:2023 Rule 11.0.1 ("Member data in non-POD class types
// should be private") checker driven by P2996 reflection. Walks T's
// nonstatic data members and refuses to compile when any has public
// or protected access. The classic encapsulation rule that static
// analyzers catch -- here baked into the type system.
//
// Why this matters for qualified compilers: in ISO 26262 / IEC 61508
// / DO-178C projects you ship a Software Tool Confidence Level (TCL)
// argument for every tool in the pipeline. A consteval check baked
// into the source is part of the language toolchain's TCL-1 argument;
// a separate Coverity / clang-tidy run is a TCL-3 tool requiring its
// own qualification kit. Moving rules into the type system shrinks
// the qualification surface.
//
// Compile + run via the wro.cpp container:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/h && /tmp/h" \
//     posts/toolset/qualified-compilers/examples/reflect-misra-rule-of-five.cpp

#include <experimental/meta>
#include <print>
#include <string>

template <typename T>
consteval bool meets_misra_11_0_1() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        static_assert(std::meta::is_private(m),
            "MISRA C++:2023 Rule 11.0.1: non-POD class data members "
            "must be private. Public/protected data leaks invariants "
            "and breaks the encapsulation argument in your safety case.");
    }
    return true;
}

// OK: all data members private; public interface goes through
// member functions whose pre/postconditions can be argued.
class SensorReading {
    double  value_;
    int     timestamp_;
    bool    valid_;
public:
    SensorReading(double v, int t) : value_(v), timestamp_(t), valid_(true) {}
    auto value()    const { return value_; }
    auto timestamp() const { return timestamp_; }
    auto valid()    const { return valid_; }
};

// To see the check fire, uncomment:
//
// struct LeakedInvariant {
//     double value;       // public -- VIOLATION
//     int    timestamp;
// };
// static_assert(meets_misra_11_0_1<LeakedInvariant>());

int main() {
    static_assert(meets_misra_11_0_1<SensorReading>());
    std::println("MISRA C++:2023 Rule 11.0.1 check passed for SensorReading.");
    std::println("Uncomment LeakedInvariant struct to see the static_assert fire.");
}

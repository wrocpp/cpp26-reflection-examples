// "Reflection today" demo for cpp-coding-standards: compose multiple
// coding-standard rules (MISRA C++:2023, SEI CERT C++, JSF AV C++)
// into a single consteval bundle that walks T's members and applies
// every rule in turn. Refuses to compile if any rule fires.
//
// Why: most projects in regulated industries pick A coding standard
// and run a separate analyzer for it (clang-tidy with the misra-*
// or cert-* check sets). Different teams pick different standards;
// some need to satisfy two simultaneously (e.g. ISO 26262 + DO-178C
// project shares a header). Encoding the rules as consteval
// predicates over reflection means the bundle is composable -- a
// header consumed by both projects can declare BOTH standards via
// `static_assert(misra<T>() && cert<T>() && jsf<T>())` and refuse
// to compile under either's violations. No analyzer-config matrix.
//
// The rules below are illustrative (one per standard). A real bundle
// would have dozens; the composition pattern is the contribution.
//
// Compile + run via the wro.cpp container:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/h && /tmp/h" \
//     posts/toolset/cpp-coding-standards/examples/reflect-coding-standard-bundle.cpp

#include <experimental/meta>
#include <print>
#include <type_traits>

namespace standards {

// MISRA C++:2023 Rule 11.0.1 -- non-POD class data members must be
// private. Encapsulation rule; covered in qualified-compilers post too.
template <typename T>
consteval bool meets_misra_11_0_1() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        static_assert(std::meta::is_private(m),
            "MISRA C++:2023 Rule 11.0.1: non-POD class data members "
            "must be private.");
    }
    return true;
}

// SEI CERT C++ DCL56-CPP -- avoid cycles during initialization of
// static objects. Approximated here as: no static_assert-violating
// raw pointer members (raw pointers are the most common vector for
// initialization-order dependencies). A more faithful check would
// require call-graph analysis; the pattern composes either way.
template <typename T>
consteval bool meets_cert_dcl56_cpp() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        using FieldT = [:std::meta::type_of(m):];
        static_assert(!std::is_pointer_v<FieldT>,
            "CERT C++ DCL56-CPP (approximation): no raw pointer members. "
            "Use std::unique_ptr / std::shared_ptr / std::span / a "
            "lifetime-checked wrapper -- raw pointers are the highest-"
            "frequency vector for static-init-order issues.");
    }
    return true;
}

// JSF AV C++ Rule 75 -- no struct with bit-fields shall be used as
// a function parameter or return type (for portability and ABI
// stability across DO-178C-qualified compilers). Approximated as:
// no bit-field members in any T that the bundle is applied to.
template <typename T>
consteval bool meets_jsf_av_75() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        static_assert(!std::meta::is_bit_field(m),
            "JSF AV C++ Rule 75: no bit-field members. Bit-field "
            "layout is implementation-defined and breaks ABI portability "
            "across qualified compilers.");
    }
    return true;
}

// Composition: bundle that applies all three standards. Multi-standard
// projects (ISO 26262 + DO-178C shared headers) declare:
//   static_assert(standards::all<MyType>());
// and get every rule enforced at compile time.
template <typename T>
consteval bool all() {
    return meets_misra_11_0_1<T>()
        && meets_cert_dcl56_cpp<T>()
        && meets_jsf_av_75<T>();
}

}  // namespace standards

// OK: passes all three standards. Private members, no raw pointers,
// no bit-fields. Suitable for a header shared between an ISO 26262
// (MISRA), DO-178C (JSF AV), and avionics-defensive (CERT) project.
class SensorReading {
    double  value_;
    long    timestamp_;
    bool    valid_;
public:
    SensorReading(double v, long t) : value_(v), timestamp_(t), valid_(true) {}
    auto value()     const { return value_; }
    auto timestamp() const { return timestamp_; }
    auto valid()     const { return valid_; }
};

// To see each rule fire in turn, uncomment one of these:
//
// struct PublicMember     { double v; };                 // MISRA 11.0.1
// struct RawPointerMember { int* p; };                   // CERT DCL56-CPP
// struct BitfieldMember   { unsigned x : 4; unsigned y : 4; }; // JSF 75
// static_assert(standards::all<PublicMember>());
// static_assert(standards::all<RawPointerMember>());
// static_assert(standards::all<BitfieldMember>());

int main() {
    static_assert(standards::all<SensorReading>());
    std::println("SensorReading passes the composed coding-standard bundle:");
    std::println("  MISRA C++:2023 Rule 11.0.1   (encapsulation)");
    std::println("  CERT C++ DCL56-CPP           (no raw pointers)");
    std::println("  JSF AV C++ Rule 75           (no bit-fields)");
    std::println("");
    std::println("Uncomment one of the bad-example structs at the bottom of");
    std::println("the source to see each rule fire individually.");
}

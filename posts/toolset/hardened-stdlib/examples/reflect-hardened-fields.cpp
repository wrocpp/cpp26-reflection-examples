// "Reflection today" demo for hardened-stdlib: a compile-time linter
// that walks a struct's data members and refuses to compile if any
// field uses a raw-pointer / C-array shape that the hardened standard
// library can't catch. The hardened stdlib protects you when you USE
// std::vector / std::string / std::span (P3471 bounds checks fire at
// the access). It can't protect you when you DECLARE a raw int* or
// char[64] field -- the hardened guarantees stop at the type system
// boundary. Reflection closes that gap at the schema layer.
//
// Pattern: a single consteval predicate walks T's members and
// static_asserts on each, refusing to compile structures whose fields
// can't benefit from hardened-stdlib's runtime checks. Add a hardened-
// stdlib-friendly type (std::vector, std::span, std::string), the check
// passes. Add a raw int* or char[N], the build refuses with the field
// name in the diagnostic.
//
// Cross-link: this is the same `meets_<profile>` pattern as the safety-
// profiles user-library demo (see memory-safety-cpp26-and-beyond), but
// scoped to "fields the hardened stdlib will actually protect".
//
// Compile + run via the wro.cpp container:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST \
//                      $0 -o /tmp/h && /tmp/h" \
//     posts/toolset/hardened-stdlib/examples/reflect-hardened-fields.cpp

#include <experimental/meta>
#include <array>
#include <cstdint>
#include <print>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

// Per-class structural check. Refuses to compile when a field is a
// shape the hardened stdlib can't protect.
template <typename T>
consteval bool meets_hardened_field_shapes() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        using FieldT = [:std::meta::type_of(m):];
        static_assert(!std::is_pointer_v<FieldT>,
            "hardened-stdlib check: raw-pointer member -- use std::span "
            "or std::unique_ptr so the library's checks apply at access");
        static_assert(!std::is_array_v<FieldT>,
            "hardened-stdlib check: C-array member -- use std::array<T,N> "
            "or std::span; raw [N] has no bounds-check hook");
    }
    return true;
}

// Demo: a wire-protocol struct that the hardened stdlib will protect
// fully. Every member is a type the library knows how to bounds-check
// (vector::operator[], string::operator[], span::operator[], optional's
// .value()).
struct Frame {
    std::uint16_t           id;
    std::vector<std::byte>  payload;
    std::string             tag;
    std::span<const int>    samples;          // viewer-only, no ownership
    std::array<std::byte,4> trailer;
};

// To see the check fire in real time, uncomment one of these:
//
// struct FrameBadPtr   { int*    refcount; };  // raw pointer
// struct FrameBadArr   { char    name[64]; };  // C-style array
//
// static_assert(meets_hardened_field_shapes<FrameBadPtr>());  // FAILS
// static_assert(meets_hardened_field_shapes<FrameBadArr>());  // FAILS

int main() {
    static_assert(meets_hardened_field_shapes<Frame>(),
                  "Frame should be hardened-stdlib friendly");

    std::println("Frame passes the hardened-field-shapes check.");

    // Demonstrate the hardened stdlib actually catching an OOB access.
    // With -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST, the
    // following line would abort at runtime with a contract violation.
    // Commented out so the demo's exit is clean; uncomment to verify
    // the hardened mode is wired:
    //
    //   std::vector<int> v = {1, 2, 3};
    //   std::println("{}", v[42]);   // <-- libc++ hardened: terminates here

    std::println("Uncomment the v[42] line + rebuild with hardened mode");
    std::println("to see libc++ terminate with a bounds violation.");
}

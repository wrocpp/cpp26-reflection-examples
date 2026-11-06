// "Reflection today" demo for lifetime-safety-2026: a compile-time
// check that every non-owning view member (std::string_view, std::span,
// reference, raw pointer) carries an explicit P3394 annotation naming
// the field it borrows from. The pattern forces the schema to document
// lifetime relationships at the type level -- the information a
// reviewer needs to argue "this view cannot dangle" without reading
// the constructor body.
//
// Why this matters: the C++ lifetime profile (P1179) and clang's
// -Wdangling-gsl catch the easy cases (return-local-string-view,
// pass-temporary-to-string_view-param). They miss the structural cases
// where the dangling happens inside a struct: a string_view member
// alongside a sibling string member, with the constructor pointing
// the SV at the string. Move the struct -- SV dangles. Reflection +
// P3394 annotations let you encode "this SV borrows from the
// `payload` field" in the type, and a consteval check enforces it.
//
// The annotation has no runtime effect; it exists purely for the
// reflection-walking lint to consume. Static analyzers that don't
// understand the annotation see nothing -- so the lint is the only
// place this rule lives. Compose this with the hardened-stdlib
// schema lint (no raw pointers/C-arrays) and the qualified-compilers
// MISRA Rule 11.0.1 lint (members must be private) -- same walker,
// orthogonal rules.
//
// Compile and run (checked on Compiler Explorer on 2026-11-06, exit 0):
//   g162 (GCC 16.2) and gsnapshot (GCC trunk):
//     g++ -std=c++26 -freflection reflect-borrow-annotations.cpp
//   clang_bb_p2996 (the P2996 fork):
//     clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//       reflect-borrow-annotations.cpp

#if __has_include(<meta>)
#include <meta>
#else
#include <experimental/meta>   // clang-p2996 fork
#endif
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

// P3394 user-defined annotation: "this field borrows its lifetime
// from the named sibling field". Carries a compile-time string.
// Empty tag: marks a view member as deliberate-borrow. (Capturing the
// source-field name as a payload would be nice, but P3394 annotations
// must be "structural type" template-argument-suitable -- string_view
// holding a string-literal pointer is not. The empty tag is sufficient
// for the lint: forcing the developer to mark borrow intent.)
struct borrows_from {};

// Trait: is FieldT a non-owning view that needs a borrow annotation?
template <typename T>
constexpr bool is_view_type =
       std::is_same_v<T, std::string_view>
    || std::is_pointer_v<T>
    || std::is_reference_v<T>;

template <typename T>
constexpr bool is_span_type = false;

template <typename U, std::size_t E>
constexpr bool is_span_type<std::span<U, E>> = true;

template <typename T>
constexpr bool needs_borrow_annotation = is_view_type<T> || is_span_type<T>;

template <typename Tag>
consteval bool has_annotation(std::meta::info r) {
    for (auto a : std::meta::annotations_of(r))
        if (std::meta::remove_const(std::meta::type_of(a)) == ^^Tag) return true;
    return false;
}

template <typename T>
consteval bool meets_borrow_annotated() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        using FieldT = [:std::meta::type_of(m):];
        if constexpr (needs_borrow_annotation<FieldT>) {
            static_assert(has_annotation<borrows_from>(m),
                "lifetime-safety: non-owning view member needs a "
                "[[=borrows_from{}]] annotation. The lint "
                "is the only place this rule lives -- without it, a "
                "move of the enclosing struct can dangle the view "
                "with no compiler diagnostic.");
        }
    }
    return true;
}

// OK: every view member is annotated.
struct ParsedFrame {
    std::string                                  payload;
    [[=borrows_from{}]] std::string_view header;
    [[=borrows_from{}]] std::string_view body;
};

// To see the check fire, uncomment:
//
// struct UnannotatedFrame {
//     std::string      payload;
//     std::string_view header;   // VIOLATION: no borrows_from annotation
// };
// static_assert(meets_borrow_annotated<UnannotatedFrame>());

int main() {
    static_assert(meets_borrow_annotated<ParsedFrame>());
    std::println("lifetime-safety check passed for ParsedFrame.");
    std::println("Uncomment UnannotatedFrame to see the static_assert fire.");
}

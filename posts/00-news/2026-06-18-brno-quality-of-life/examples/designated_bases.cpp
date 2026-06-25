// designated_bases.cpp  (~40 lines)
// "C++29 quality-of-life": designated-initializers that reach through a base
// class -- P2287 (Revzin), adopted for C++29 at the Brno meeting (June 2026).
//
// C++20 added designated initializers but forbade them for any aggregate that
// has a base class. P2287 lifts that: you name the inherited member directly,
// with no special base designator. None of the June-2026 toolchains (GCC 16.1,
// clang-p2996) implement P2287 yet, so the C++29 form is kept in a comment.
//
// What compiles TODAY is the C++17/20 nested-brace form: you can initialize the
// base subobject, you just cannot use a .designator for an inherited member.
//
// Compile (GCC 16.1 or later):     g++-16 -std=c++26 designated_bases.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ designated_bases.cpp -o demo

#include <print>

struct Rect   { int w; int h; };
struct Button : Rect { const char* label; };

int main() {
    // TODAY (C++17/20): brace the base subobject; no designators through a base.
    Button ok{{120, 32}, "OK"};
    std::println("{}x{} [{}]", ok.w, ok.h, ok.label);

    // C++29 -- P2287, not yet implemented anywhere in June 2026:
    //
    //   Button a{.w = 120, .h = 32, .label = "OK"};   // name inherited members
    //   Button b{{.w = 120, .h = 32}, .label = "C"};  // or brace the base
    //   Button c{Rect{120, 32}, .label = "T"};        // or init the base by type
    //   // Button{.label = "X", .w = 1, .h = 2};      // still ill-formed: order
    //
    // The out-of-order rule from C++20 stays: designators appear in declaration
    // order, base members first.
}

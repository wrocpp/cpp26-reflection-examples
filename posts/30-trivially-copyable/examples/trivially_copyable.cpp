// trivially_copyable.cpp
//
// A type that is trivially copyable and cannot be copied.
//
// From P3279R0, "CWG2463: What 'trivially fooable' should mean", Arthur
// O'Dwyer, 2024-05-15. The paper reports that EDG and MSVC say Nutshell is
// trivially copyable while Clang and GCC say it is not, and that "EDG+MSVC
// are certainly correct according to the Standard".
//
// Two years on the divergence is still there:
//
//   is_trivially_copyable_v<Nutshell>   MSVC 19.51: true    GCC 16.1 / clang 22.1: false
//
// The mechanism is easy to miss. Declaring the move operations suppresses the
// copy operations, so Nutshell has no eligible copy constructor or copy
// assignment at all. The standard's definition asks whether every ELIGIBLE
// copy/move operation is trivial. With the copies gone, only the trivial
// moves remain, so the condition holds vacuously.
//
// The consequence is the assertion block below: on a conforming compiler the
// type is "trivially copyable" and is neither copy-constructible nor
// copy-assignable.
//
// NOTE ON THE POPULAR RETELLING: this does NOT happen for a plain aggregate
// of non-trivially-copyable members. PlainPair below is false everywhere,
// including MSVC. The defaulted move declarations are what does the work.
//
// Compile (msvc, all asserts hold): cl /std:c++latest /DCHECK
// Compile (gcc,  first assert fails): g++ -std=c++23 -DCHECK
// verify: msvc-only
// verify: gcc-options: -std=c++23

#include <type_traits>
#include <cstdio>

struct Hamlet {
    Hamlet(const Hamlet&) { }                    // user-provided: not trivial
    Hamlet(Hamlet&&) = default;
    Hamlet& operator=(Hamlet&&) = default;
};

struct Nutshell {
    Hamlet h_;
    Nutshell(Nutshell&&) = default;              // these two suppress
    Nutshell& operator=(Nutshell&&) = default;   // the copy operations
};

// What the retelling describes: a plain aggregate. Not trivially copyable
// on any compiler, MSVC included.
struct PlainPair {
    Hamlet a;
    Hamlet b;
};

#ifdef CHECK
static_assert(std::is_trivially_copyable_v<Nutshell>,
              "Nutshell is not trivially copyable on this compiler");
static_assert(!std::is_copy_constructible_v<Nutshell>,
              "Nutshell is copy constructible after all");
static_assert(!std::is_copy_assignable_v<Nutshell>,
              "Nutshell is copy assignable after all");
#endif

int main() {
    std::printf("Hamlet    trivially copyable = %d\n", (int)std::is_trivially_copyable_v<Hamlet>);
    std::printf("Nutshell  trivially copyable = %d\n", (int)std::is_trivially_copyable_v<Nutshell>);
    std::printf("Nutshell  copy constructible = %d\n", (int)std::is_copy_constructible_v<Nutshell>);
    std::printf("PlainPair trivially copyable = %d\n", (int)std::is_trivially_copyable_v<PlainPair>);
    return 0;
}

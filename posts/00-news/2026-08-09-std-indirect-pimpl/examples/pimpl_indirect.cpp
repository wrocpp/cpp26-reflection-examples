// pimpl_indirect.cpp  (~24 lines)
// PImpl with std::unique_ptr costs you the special members: the copy
// constructor and copy assignment have to be written by hand (unique_ptr is
// move-only), the destructor has to be defined where Impl is complete, and
// const does not propagate through the pointer, so a const method can still
// mutate the implementation.
//
// C++26's std::indirect (P3019) is an indirect VALUE: it copies deeply,
// propagates const, and is never null except after a move. All five special
// members can then be defaulted. GCC 16.1's libstdc++ ships it
// (__cpp_lib_indirect = 202502).
//
// Compile (GCC 16.1): g++ -std=c++26 -O2 pimpl_indirect.cpp

#include <memory>
#include <print>

struct Impl { int value = 42; };

struct Widget {
    std::indirect<Impl> pimpl;
    Widget() : pimpl(std::in_place) {}
    int  get() const { return pimpl->value; }   // const propagates through
    void set(int v)  { pimpl->value = v; }
};

int main() {
    Widget a;
    a.set(7);
    Widget b = a;      // deep copy, with no user-written copy constructor
    b.set(99);
    std::println("a={} b={}", a.get(), b.get());
    std::println("copyable and movable with nothing hand-written: {}",
                 std::is_copy_constructible_v<Widget> &&
                 std::is_move_constructible_v<Widget>);
}

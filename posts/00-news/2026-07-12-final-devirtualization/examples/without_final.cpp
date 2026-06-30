// without_final.cpp  (~18 lines)
// A virtual call through a reference to the most-derived type. Because `Derived` is NOT
// final, the compiler cannot prove that `d.compute()` will not dispatch to some
// further-derived override, so it must keep the virtual call: load the vtable pointer,
// load the slot, call through it.
//
// Compile to see the asm (x86-64 GCC):  g++ -O2 -std=c++20 -S without_final.cpp

struct Base {
    virtual int compute() const { return 1; }
    virtual ~Base() = default;
};

struct Derived : Base {                  // not final
    int compute() const override { return 42; }
};

// Keep `call` out of line so we can read its codegen on its own.
int call(const Derived& d) {
    return d.compute();                  // stays a virtual dispatch
}

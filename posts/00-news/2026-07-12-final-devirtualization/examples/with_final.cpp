// with_final.cpp  (~18 lines)
// The SAME code, with one word changed: `Derived` is now `final`. That tells the compiler
// the dynamic type of a `Derived&` is exactly `Derived` -- there can be no further-derived
// override. So `d.compute()` is devirtualized and inlined to its body, and `call` folds to
// a single constant load. Compare the asm side by side with without_final.cpp.
//
// Compile to see the asm (x86-64 GCC):  g++ -O2 -std=c++20 -S with_final.cpp

struct Base {
    virtual int compute() const { return 1; }
    virtual ~Base() = default;
};

struct Derived final : Base {            // the only change: `final`
    int compute() const override { return 42; }
};

// Keep `call` out of line so we can read its codegen on its own.
int call(const Derived& d) {
    return d.compute();                  // devirtualized + inlined to `return 42;`
}

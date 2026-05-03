// Why std::vector cannot be a `constexpr` or `constinit` namespace-scope
// variable (its heap allocation would have to escape transient constant
// evaluation), and three ways to consume the result of a constant-time
// std::vector once you have one.
//
// std::define_static_array (P3491) promotes the values into static
// storage. The constexpr-vs-consteval choice on `fibs` decides whether
// the runtime call site (pattern 3) is well-formed.
//
// === Toggle: uncomment to make fibs consteval ===
// Default (commented out) leaves `fibs` constexpr, so all three
// patterns run. Uncomment to switch the specifier to consteval --
// patterns (1) and (2) keep working, pattern (3) is excluded by the
// matching #ifndef. With clang-p2996 today there is no in-source way
// to detect consteval-vs-constexpr automatically (see the post's
// callout on P3795), so a single preprocessor toggle is the cleanest
// reproducer for the difference.

// #define FIBS_AS_CONSTEVAL

#ifdef FIBS_AS_CONSTEVAL
#  define FIBS_SPECIFIER consteval
#else
#  define FIBS_SPECIFIER constexpr
#endif

#include <experimental/meta>
#include <print>
#include <vector>

// constexpr std::vector<int> bad1 = {1, 2, 3};   // error: storage escapes
// constinit std::vector<int> bad2 = {1, 2, 3};   // error: same problem

FIBS_SPECIFIER auto fibs(int n) {
    std::vector<int> out;
    int a = 0, b = 1;
    for (int i = 0; i < n; ++i) {
        out.push_back(a);
        int next = a + b;
        a = b;
        b = next;
    }
    return out;
}

int main() {
    // (1) template for: needs a constant-expression range at template-
    // instantiation time. define_static_array(fibs(8)) is an immediate-
    // function-context call (define_static_array is consteval), so
    // fibs(8) runs at compile time regardless of its specifier.
    template for (constexpr int x : std::define_static_array(fibs(8))) {
        std::println("(1) template for      fib = {}", x);
    }

    // (2) constexpr local span: the std::span value is literal and its
    // pointer addresses static storage, so it is free to "escape" into
    // runtime. Iterate as many times as you want; zero runtime
    // allocation. This is often the pattern you actually want in
    // production code.
    constexpr auto f2 = std::define_static_array(fibs(8));
    for (auto x : f2) {
        std::println("(2) constexpr span    fib = {}", x);
    }

#ifndef FIBS_AS_CONSTEVAL
    // (3) runtime call: a normal runtime invocation that returns a
    // real, heap-allocated std::vector<int>. Well-formed only because
    // `fibs` is `constexpr` (callable from runtime contexts too). With
    // FIBS_AS_CONSTEVAL defined this call site would be ill-formed --
    // "call to consteval function 'fibs' is not a constant expression"
    // -- and the program would not compile at all. The #ifndef
    // isolates the conditional surface so toggling the specifier is a
    // one-line uncomment at the top of the file.
    auto f3 = fibs(8);
    for (auto x : f3) {
        std::println("(3) runtime vector    fib = {}", x);
    }
#endif // !FIBS_AS_CONSTEVAL
}

// Why std::vector cannot be a `constexpr` or `constinit` namespace-scope
// variable (its heap allocation would have to escape transient constant
// evaluation), and how std::define_static_array (P3491) promotes the
// consteval value into a static-storage std::span you can iterate at
// template-instantiation time.

#include <experimental/meta>
#include <print>
#include <vector>

// constexpr std::vector<int> bad1 = {1, 2, 3};   // error: storage escapes
// constinit std::vector<int> bad2 = {1, 2, 3};   // error: same problem

// A consteval function may build and return a std::vector<int>: the
// allocation lives during constant evaluation and is destroyed when the
// evaluation finishes. The values can survive -- the container can't.
consteval auto fibs(int n) {
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
    // std::define_static_array takes a consteval value (here, fibs(8))
    // and emits a static-storage const array, returning a std::span over
    // it. The span survives constant evaluation; the original vector
    // does not. The span is a constant expression at template-for
    // instantiation time, which is exactly what `template for` needs.
    template for (constexpr int x : std::define_static_array(fibs(8))) {
        std::println("fib = {}", x);
    }
}

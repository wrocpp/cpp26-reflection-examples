// verify: gcc-only
// Build: g++ -std=c++20 -O1 which_braces_are_flagged.cpp -o which_braces_are_flagged
// Prints: how many items each braced variable holds. The file exists to be given to clang-tidy:
// five of the lines below hijack the copy and are flagged, and three are not flagged (the
// two-element list is correct, and Value{3} and vector<int>{3} are hijacks the check misses).
// Limits: Value only counts its items, it stores no data. Exits 0.
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace {

struct Value {
    Value() = default;
    Value(int) {}
    Value(std::initializer_list<Value> items) : count{items.size()} {}

    std::size_t count{};
};

struct Holder {
    Value member{Value{}};  // one element of the class itself, in a member initializer
};

// The shape of the storage in a result<T> polyfill: a member initialized with braces from a
// value of the template argument. It holds T for int and wraps it for Value.
template <class T>
struct Box {
    explicit Box(T value) : stored{std::move(value)} {}

    T stored;
};

constexpr int kNumber = 3;

}  // namespace

int main() {
    const Value source;
    const auto made = Value{source};                      // one element of the class
    const Value assigned = {source};                      // copy-list-initialization
    const auto heap = std::make_unique<Value>(Value{source});  // the inner braces
    const Value number{kNumber};                          // not flagged: the element is an int
    const Value pair{source, source};                     // not flagged: two elements are a real list
    const Holder holder;
    const Box<Value> boxed{Value{}};
    std::cout << "Value{v}: " << made.count << ", = {v}: " << assigned.count
              << ", in make_unique: " << heap->count << ", Value{3}: " << number.count
              << ", Value{v, v}: " << pair.count << ", member: " << holder.member.count
              << ", Box<Value>: " << boxed.stored.count << "\n";
    const std::vector<int> sized(kNumber);
    const std::vector<int> listed{kNumber};               // not flagged: the element is an int
    std::cout << "vector<int>(3): " << sized.size() << ", vector<int>{3}: " << listed.size() << "\n";
    return 0;
}

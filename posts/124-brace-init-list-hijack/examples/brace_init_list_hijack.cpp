// verify: gcc-only
// Build: g++ -std=c++20 -O1 brace_init_list_hijack.cpp -o brace_init_list_hijack
// Prints: what the same value becomes with braces and with parentheses, for a JSON-like Value
// and for std::vector<int>. The Value half is what the clang-tidy check flags (the two lines
// marked below); the vector half is a hijack the check does not flag.
// Limits: Value only counts its items, it stores no data. Exits 0.
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <vector>

namespace {

enum class Kind { object, array };

// The shape of a JSON value type: a constructor taking a list of values next to the implicit copy.
struct Value {
    Value() = default;
    Value(std::initializer_list<Value> items) : kind{Kind::array}, count{items.size()} {}

    Kind kind{Kind::object};
    std::size_t count{};
};

constexpr int kElementCount = 5;

Value wrap(const Value& v) {
    return {v};  // flagged: this is a one-element array, not a copy
}

Value copy(const Value& v) {
    return Value(v);
}

const char* describe(const Value& v) {
    return v.kind == Kind::array ? "array" : "object";
}

}  // namespace

int main() {
    const Value object;
    const Value braces{object};  // flagged
    const Value parens(object);
    const Value wrapped = wrap(object);
    const Value copied = copy(object);
    std::cout << "braces: " << describe(braces) << " of " << braces.count << "\n";
    std::cout << "parens: " << describe(parens) << "\n";
    std::cout << "return {v}: " << describe(wrapped) << ", return Value(v): " << describe(copied) << "\n";

    const std::vector<int> list{kElementCount};
    const std::vector<int> sized(kElementCount);
    std::cout << "vector<int>{5}: size " << list.size() << ", vector<int>(5): size " << sized.size() << "\n";
    return 0;
}

// verify: gcc-only
// The same Slice with an rvalue overload deleted: a view of a temporary string no longer compiles.
//
// Build: g++ -std=c++23 -Wall -Wextra guarded_slice.cpp -o guarded_slice      (GCC 16.2)
// Prints: the size and first character of a slice of a named string.
// Limits: this guards one class and one owner type (std::string). A function that returns a view
// of its argument, or a class not guarded this way, is not covered.

#include <cstddef>
#include <cstdio>
#include <string>
#include <string_view>
#include <type_traits>

constexpr std::string_view kText = "a text long enough to leave the small-string buffer";

class LooseSlice {
public:
    LooseSlice(const std::string& s) : data_(s.data()), size_(s.size()) {}
    char first() const { return *data_; }
    std::size_t size() const { return size_; }

private:
    const char* data_;
    std::size_t size_;
};

class GuardedSlice {
public:
    GuardedSlice(const std::string& s) : data_(s.data()), size_(s.size()) {}
    GuardedSlice(std::string&&) = delete;
    char first() const { return *data_; }
    std::size_t size() const { return size_; }

private:
    const char* data_;
    std::size_t size_;
};

// The compiler accepts the temporary for the loose class and refuses it for the guarded one.
static_assert(std::is_constructible_v<LooseSlice, std::string>);
static_assert(!std::is_constructible_v<GuardedSlice, std::string>);
static_assert(std::is_constructible_v<GuardedSlice, const std::string&>);
// A string literal builds a temporary string too, so it is refused as well.
static_assert(std::is_constructible_v<LooseSlice, const char (&)[4]>);
static_assert(!std::is_constructible_v<GuardedSlice, const char (&)[4]>);

int main() {
    const std::string named{kText};
    const GuardedSlice s{named};
    std::printf("size %zu, first '%c'\n", s.size(), s.first());
    return 0;
}

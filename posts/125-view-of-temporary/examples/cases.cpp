// verify: gcc-only
// A view built from a temporary owner: thirteen cases that dangle, and four that do not.
//
// Build: g++ -std=c++23 -Wall -Wextra cases.cpp -o cases      (GCC 16.2; clang 22 and later also build it)
// Run:   ./cases        runs the four safe cases only, prints one line, exits 0
//        ./cases N      runs dangling case N (1 to 13) once. That is undefined behaviour: use it
//                       under -fsanitize=address, which reports a heap-use-after-free.
// Limits: the text is longer than any small-string buffer, so the owner's bytes live on the heap
// and a sanitizer can see them freed. Nothing here is a benchmark.

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <span>
#include <string>
#include <string_view>
#include <vector>

constexpr std::string_view kText = "a text long enough to leave the small-string buffer";
constexpr std::size_t kSafeCaseCount = 4;
constexpr std::size_t kDanglingCaseCount = 13;
constexpr int kArgIndex = 1;
constexpr int kArgsWithCase = 2;

// A project view: it borrows bytes and owns none.
class Slice {
public:
    Slice(const std::string& s) : data_(s.data()), size_(s.size()) {}
    char first() const { return *data_; }
    std::size_t size() const { return size_; }

private:
    const char* data_;
    std::size_t size_;
};

// The same shape, but its name is not in the check's HandleClasses list.
class Other {
public:
    Other(const std::string& s) : data_(s.data()) {}
    char first() const { return *data_; }

private:
    const char* data_;
};

std::string make() { return std::string{kText}; }
std::vector<char> make_chars() { return std::vector<char>(kText.begin(), kText.end()); }
std::string_view first_word(const std::string& s) { return std::string_view{s}.substr(0, 1); }
std::string_view pass(std::string_view v) { return v; }
std::string_view returns_view() { return make(); }
char consume(const Slice& s) { return s.first(); }

// Dangling cases 1 to 13.
char case1(const std::string&) { Slice s{std::string{kText}}; return s.first(); }
char case2(const std::string&) { Slice s(make()); return s.first(); }
char case3(const std::string&) { std::string_view v = make(); return v.front(); }
char case4(const std::string&) { std::string_view v = make() + "!"; return v.front(); }
char case5(const std::string&) { std::span<const char> v = make_chars(); return v.front(); }
char case6(const std::string&) { std::string_view v{make()}; return v.front(); }
char case7(const std::string&) { Other o{make()}; return o.first(); }
char case8(const std::string&) { auto v = first_word(make()); return v.front(); }
char case9(const std::string& named) {
    std::string_view v = !named.empty() ? std::string_view{make()} : std::string_view{named};
    return v.front();
}
char case10(const std::string&) { std::string_view v = pass(make()); return v.front(); }
char case11(const std::string&) { std::string_view v = make().substr(0, kText.size()); return v.front(); }
char case12(const std::string&) { const char* p = make().c_str(); return *p; }
char case13(const std::string&) { return returns_view().front(); }

// Safe cases 1 to 4.
char safe1(const std::string& named) { Slice s{named}; return s.first(); }
char safe2(const std::string&) { std::string_view v = "literal"; return v.front(); }
char safe3(const std::string&) { return static_cast<char>(make().size()); }
char safe4(const std::string&) { return consume(Slice{make()}); }

using Case = char (*)(const std::string&);

constexpr Case kDangling[kDanglingCaseCount] = {case1, case2, case3, case4,  case5,  case6, case7,
                                                case8, case9, case10, case11, case12, case13};
constexpr Case kSafe[kSafeCaseCount] = {safe1, safe2, safe3, safe4};

int main(int argc, char** argv) {
    const std::string named{kText};
    if (argc == kArgsWithCase) {
        const auto n = static_cast<std::size_t>(std::atoi(argv[kArgIndex]));
        if (n < 1 || n > kDanglingCaseCount) return 1;
        std::printf("case %zu read '%c'\n", n, kDangling[n - 1](named));
        return 0;
    }
    std::size_t ran = 0;
    for (const Case c : kSafe) {
        if (c(named) != '\0') ++ran;
    }
    std::printf("%zu safe cases ran\n", ran);
    return 0;
}

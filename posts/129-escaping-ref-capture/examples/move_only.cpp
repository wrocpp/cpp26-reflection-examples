// verify: gcc-only
// The same mistake with std::move_only_function, one of the three callable types the check converts.
//
// Build: g++ -std=c++23 -Wall -Wextra move_only.cpp -o move_only      (GCC 16.2)
// Run:   ./move_only        builds the callable, calls a by-value copy of the state, prints one line, exits 0
//        ./move_only bug    calls the escaped by-reference lambda after its frame ended (undefined
//                           behaviour: use it under -fsanitize=address)
// Limits: needs a standard library with std::move_only_function (libstdc++ 12 or later).

#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <string_view>

constexpr std::string_view kText = "a text long enough to leave the small-string buffer";
constexpr int kArgIndex = 1;
constexpr int kArgsWithMode = 2;

using MoveOnlyHandler = std::move_only_function<int()>;

int first_char(const std::string& s) { return static_cast<int>(s.front()); }

MoveOnlyHandler make_dangling() {
    std::string label{kText};
    return [&label] { return first_char(label); };
}

MoveOnlyHandler make_safe() {
    std::string label{kText};
    return [label] { return first_char(label); };
}

int main(int argc, char** argv) {
    const bool bug = argc == kArgsWithMode && std::strcmp(argv[kArgIndex], "bug") == 0;
    MoveOnlyHandler handler = bug ? make_dangling() : make_safe();
    std::printf("read '%c'\n", handler());
    return 0;
}

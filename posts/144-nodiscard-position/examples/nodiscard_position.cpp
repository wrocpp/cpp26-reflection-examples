// verify: gcc-only
// [[nodiscard]] written first, before every keyword of the declaration.
//
// Build (GCC 16.2):   g++ -std=c++26 -Wall -Wextra -Werror=unused-result nodiscard_position.cpp
// Build (MSVC):       cl /std:c++latest /W4 /we4834 /we5240 nodiscard_position.cpp
// Prints one line per declaration form and exits 0. No call discards its result, so the build stays
// clean with a discarded [[nodiscard]] result turned into an error on both compilers.
// Limit: a correct placement cannot be asserted in code; standard C++ has no trait for an attribute.
// The misplaced forms are in ../misplaced/ and are expected not to compile on GCC or Clang.
#include <print>

namespace {

constexpr int free_value = 7;
constexpr int member_value = 11;
constexpr int virtual_value = 13;
constexpr int friend_value = 17;
constexpr int guard_id = 19;

[[nodiscard]] static int static_answer() { return free_value; }
[[nodiscard]] inline int inline_answer() { return free_value; }
[[nodiscard]] constexpr int constexpr_answer() { return free_value; }

struct Util {
    [[nodiscard]] static int answer() { return member_value; }
};

struct Base {
    [[nodiscard]] virtual int answer() const { return virtual_value; }
    virtual ~Base() = default;
};

struct Token {
    [[nodiscard]] friend int answer(Token) { return friend_value; }
};

struct Guard {
    [[nodiscard]] explicit Guard(int id) : id_{id} {}
    [[nodiscard]] int id() const { return id_; }

private:
    int id_;
};

}  // namespace

int main() {
    static_assert(constexpr_answer() == free_value);
    const Base base{};
    const Guard guard{guard_id};
    std::println("[[nodiscard]] static:            {}", static_answer());
    std::println("[[nodiscard]] inline:            {}", inline_answer());
    std::println("[[nodiscard]] constexpr:         {}", constexpr_answer());
    std::println("[[nodiscard]] static member:     {}", Util::answer());
    std::println("[[nodiscard]] virtual:           {}", base.answer());
    std::println("[[nodiscard]] friend:            {}", answer(Token{}));
    std::println("[[nodiscard]] explicit ctor:     {}", guard.id());
    return 0;
}

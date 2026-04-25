#include <experimental/meta>
#include <print>

int main() {
    constexpr std::meta::info r = ^^int;
    static_assert(r == ^^int);
    std::println("clang-p2996 reflection: OK");
}

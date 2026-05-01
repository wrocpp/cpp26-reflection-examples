// hello_reflect.cpp  (~25 lines)
// Compile (GCC 16.1 or later):  g++-16 -std=c++26 -freflection hello_reflect.cpp -o hello
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -freflection-latest -stdlib=libc++ hello_reflect.cpp -o hello
#include <experimental/meta>
#include <print>
#include <string>

struct User {
    std::string name;
    int age;
    bool admin;
};

template <typename T>
consteval auto field_names() {
    std::string out;
    constexpr auto ctx = std::meta::access_context::unchecked();
    for (auto m : std::meta::nonstatic_data_members_of(^^T, ctx)) {
        out += std::meta::identifier_of(m);
        out += '\n';
    }
    return out;
}

int main() {
    static constexpr auto names = field_names<User>();
    std::println("{}", names);
}

// Post 18 — Reflection across languages, C++26 side.
// Representative C++ implementations of the five canonical tasks from the
// capstone comparison: serialize, enum↔string, CLI parse, ORM row, mock.
// This file exercises the SERIALIZE task; the rest are cross-linked to
// previous posts' examples.
// Compile: ./cpp posts/18-cross-language-comparison/examples/cpp/main.cpp \
//                -o posts/18-cross-language-comparison/examples/cpp/main
// Run:     ./run posts/18-cross-language-comparison/examples/cpp/main

#include <experimental/meta>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>

namespace rjson {

template <typename T> std::string to_json(T const& v);

void append_quoted(std::string& out, std::string_view s) {
    out += '"'; out += s; out += '"';
}

template <typename T>
void append_value(std::string& out, T const& v) {
    if constexpr (std::is_same_v<T, bool>) out += v ? "true" : "false";
    else if constexpr (std::is_arithmetic_v<T>) out += std::to_string(v);
    else if constexpr (std::is_convertible_v<T, std::string_view>) append_quoted(out, v);
    else out += to_json(v);
}

template <typename T>
std::string to_json(T const& obj) {
    std::string out = "{";
    bool first = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (!first) out += ',';
        first = false;
        append_quoted(out, std::meta::identifier_of(m));
        out += ':';
        append_value(out, obj.[:m:]);
    }
    out += '}';
    return out;
}

}  // namespace rjson

struct User { std::string name; int age; bool admin; };

int main() {
    // Task 1: serialize
    std::println("{}", rjson::to_json(User{"Filip", 40, true}));
}

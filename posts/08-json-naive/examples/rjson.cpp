// Post 8 — A 40-line JSON serializer with reflection.
// Compile: ./cpp posts/08-json-naive/examples/rjson.cpp \
//                -o posts/08-json-naive/examples/rjson
// Run:     ./run posts/08-json-naive/examples/rjson

#include <experimental/meta>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace rjson {

template <typename T> std::string to_json(T const& v);

void append_escaped(std::string& out, std::string_view s) {
    out += '"';
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;
        }
    }
    out += '"';
}

template <typename T>
void append_value(std::string& out, T const& v) {
    if constexpr (std::is_same_v<T, bool>) {
        out += v ? "true" : "false";
    } else if constexpr (std::is_arithmetic_v<T>) {
        out += std::to_string(v);
    } else if constexpr (std::is_convertible_v<T, std::string_view>) {
        append_escaped(out, v);
    } else if constexpr (std::ranges::range<T>) {
        out += '[';
        bool first = true;
        for (auto const& e : v) {
            if (!first) out += ',';
            first = false;
            append_value(out, e);
        }
        out += ']';
    } else {
        out += to_json(v);
    }
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
        append_escaped(out, std::meta::identifier_of(m));
        out += ':';
        append_value(out, obj.[:m:]);
    }
    out += '}';
    return out;
}

}  // namespace rjson

struct Address { std::string city; int postal_code; };
struct User {
    std::string name;
    int age;
    bool admin;
    Address home;
    std::vector<std::string> aliases;
};

int main() {
    User u{"Filip", 40, true, {"Warsaw", 12345}, {"fsjdk", "phi"}};
    std::println("{}", rjson::to_json(u));
}

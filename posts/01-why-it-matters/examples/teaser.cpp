// C++26 reflection teaser: a JSON serializer that works on any aggregate.
// Compile: ./cpp posts/01-why-it-matters/examples/teaser.cpp -o /tmp/teaser
// Run:     ./run /tmp/teaser

#include <experimental/meta>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>

namespace rjson {

template <typename T>
std::string to_json(T const& obj);

void append_quoted(std::string& out, std::string_view s) {
    out += '"';
    out += s;  // real impl would escape control characters
    out += '"';
}

template <typename T>
void append_value(std::string& out, T const& v) {
    if constexpr (std::is_same_v<T, bool>) {
        out += v ? "true" : "false";
    } else if constexpr (std::is_arithmetic_v<T>) {
        out += std::to_string(v);
    } else if constexpr (std::is_same_v<T, std::string>
                     || std::is_same_v<T, std::string_view>
                     || std::is_same_v<T, char const*>) {
        append_quoted(out, v);
    } else {
        out += to_json(v);
    }
}

template <typename T>
std::string to_json(T const& obj) {
    std::string out = "{";
    bool first = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto member
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (!first) out += ',';
        first = false;
        append_quoted(out, std::meta::identifier_of(member));
        out += ':';
        append_value(out, obj.[:member:]);
    }
    out += '}';
    return out;
}

}  // namespace rjson

// --- your code ---------------------------------------------------------

struct Address {
    std::string city;
    int postal_code;
};

struct User {
    std::string name;
    int age;
    bool admin;
    Address home;
};

int main() {
    User u{"Filip", 40, true, {"Warsaw", 12345}};
    std::println("{}", rjson::to_json(u));
}

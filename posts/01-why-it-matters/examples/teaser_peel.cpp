// Same teaser, "peel the first iteration" style — no per-iteration bool flag.
// Compile: ./cpp posts/01-why-it-matters/examples/teaser_peel.cpp \
//                -o posts/01-why-it-matters/examples/teaser_peel
// Run:     ./run posts/01-why-it-matters/examples/teaser_peel

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
    out += s;
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

// One emit step; shared between the peeled first element and the tail.
template <std::meta::info Member, typename T>
void append_member(std::string& out, T const& obj) {
    append_quoted(out, std::meta::identifier_of(Member));
    out += ':';
    append_value(out, obj.[:Member:]);
}

template <typename T>
std::string to_json(T const& obj) {
    std::string out = "{";

    if constexpr (constexpr auto members = std::define_static_array(
                      std::meta::nonstatic_data_members_of(
                          ^^T, std::meta::access_context::unchecked()));
                  !members.empty()) {
        // First member — no leading comma.
        append_member<members[0]>(out, obj);

        // Tail — each iteration emits ',' before its member.
        template for (constexpr auto member : members.subspan(1)) {
            out += ',';
            append_member<member>(out, obj);
        }
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

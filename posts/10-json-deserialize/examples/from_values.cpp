// Post 10 — Deserialization and std::expected.
// Scaled-down demo: populate a struct from a pre-parsed map of string→variant,
// with std::expected error paths. The real post pairs this with a lexer/parser.
// Compile: ./cpp posts/10-json-deserialize/examples/from_values.cpp \
//                -o posts/10-json-deserialize/examples/from_values
// Run:     ./run posts/10-json-deserialize/examples/from_values

#include <experimental/meta>
#include <expected>
#include <map>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace rjson {

using value = std::variant<std::monostate, bool, double, std::string>;
using object = std::map<std::string, value, std::less<>>;

struct parse_error { std::string path; std::string reason; };

template <typename T>
std::expected<T, parse_error> from_value_primitive(value const& v, std::string_view path) {
    if constexpr (std::is_same_v<T, bool>) {
        if (auto* p = std::get_if<bool>(&v)) return *p;
        return std::unexpected(parse_error{std::string{path}, "expected bool"});
    } else if constexpr (std::is_arithmetic_v<T>) {
        if (auto* p = std::get_if<double>(&v)) return static_cast<T>(*p);
        return std::unexpected(parse_error{std::string{path}, "expected number"});
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (auto* p = std::get_if<std::string>(&v)) return *p;
        return std::unexpected(parse_error{std::string{path}, "expected string"});
    } else {
        return std::unexpected(parse_error{std::string{path}, "unsupported type"});
    }
}

template <typename T>
std::expected<T, parse_error> from_object(object const& obj, std::string path = "") {
    T result{};
    std::optional<parse_error> err;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        constexpr auto key = std::meta::identifier_of(m);
        auto it = obj.find(std::string{key});
        if (it == obj.end()) continue;   // missing → leave default
        using M = [:std::meta::type_of(m):];
        auto parsed = from_value_primitive<M>(it->second, path + "." + std::string{key});
        if (!parsed) { err = parsed.error(); break; }
        result.[:m:] = *parsed;
    }
    if (err) return std::unexpected(*err);
    return result;
}

}  // namespace rjson

struct User { std::string name; double age; bool admin; };

int main() {
    rjson::object obj{
        {"name",  rjson::value{std::string{"Filip"}}},
        {"age",   rjson::value{40.0}},
        {"admin", rjson::value{true}},
    };
    auto r = rjson::from_object<User>(obj);
    if (r) {
        std::println("name={} age={} admin={}", r->name, r->age, r->admin);
    } else {
        std::println("error at {}: {}", r.error().path, r.error().reason);
    }

    // Failure case: wrong type for age
    rjson::object bad{
        {"name", rjson::value{std::string{"Filip"}}},
        {"age",  rjson::value{std::string{"forty"}}},
    };
    auto br = rjson::from_object<User>(bad);
    if (!br) std::println("failure path: at {} — {}", br.error().path, br.error().reason);
}

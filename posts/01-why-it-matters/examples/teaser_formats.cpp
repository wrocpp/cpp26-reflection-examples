// One reflection walk, three wire formats: JSON, YAML, XML.
// Annotations (rename, skip, rename_all) apply uniformly across all formats.
//
// Compile: ./cpp posts/01-why-it-matters/examples/teaser_formats.cpp \
//                -o posts/01-why-it-matters/examples/teaser_formats
// Run:     ./run posts/01-why-it-matters/examples/teaser_formats

#include <experimental/meta>
#include <cstdint>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace rserial {

// --- annotations ---------------------------------------------------

enum class naming { as_is, camel_case };

struct json_name_tag {
    char const* value;
    bool operator==(json_name_tag const&) const = default;
};
consteval auto json_name(std::string_view s) -> json_name_tag {
    return {std::define_static_string(s)};
}

struct skip { bool operator==(skip const&) const = default; };

struct rename_all_tag {
    naming style;
    bool operator==(rename_all_tag const&) const = default;
};
consteval auto rename_all(naming n) -> rename_all_tag { return {n}; }

// --- compile-time key resolution -----------------------------------

consteval char const* to_camel_case(std::string_view s) {
    std::string out;
    bool upper_next = false;
    for (char c : s) {
        if (c == '_') { upper_next = true; continue; }
        if (upper_next && c >= 'a' && c <= 'z')
            out += static_cast<char>(c - ('a' - 'A'));
        else
            out += c;
        upper_next = false;
    }
    return std::define_static_string(out);
}

template <std::meta::info Member>
consteval std::string_view key_of() {
    if constexpr (constexpr auto a
                  = std::meta::annotation_of_type<json_name_tag>(Member)) {
        return a->value;
    } else {
        constexpr auto parent = std::meta::parent_of(Member);
        if constexpr (constexpr auto pa
                      = std::meta::annotation_of_type<rename_all_tag>(parent)) {
            if constexpr (pa->style == naming::camel_case) {
                return to_camel_case(std::meta::identifier_of(Member));
            }
        }
        return std::meta::identifier_of(Member);
    }
}

// --- format policies -----------------------------------------------

struct json_format {
    void begin_object(std::string& o)    { o += '{'; }
    void end_object(std::string& o)      { o += '}'; }
    void field_separator(std::string& o) { o += ','; }
    void end_field(std::string&)         {}
    void emit_key(std::string& o, std::string_view k) {
        o += '"'; o += k; o += "\":";
    }
    void emit_string(std::string& o, std::string_view v) { o += '"'; o += v; o += '"'; }
    void emit_bool  (std::string& o, bool v)             { o += v ? "true" : "false"; }
    template <typename N>
    void emit_number(std::string& o, N v)                { o += std::to_string(v); }
};

struct yaml_format {
    int depth = 0;
    void begin_object(std::string& o) {
        if (depth > 0) o += '\n';
        ++depth;
    }
    void end_object(std::string&)        { --depth; }
    void field_separator(std::string& o) { o += '\n'; }
    void end_field(std::string&)         {}
    void emit_key(std::string& o, std::string_view k) {
        for (int i = 0; i < depth - 1; ++i) o += "  ";
        o += k; o += ": ";
    }
    void emit_string(std::string& o, std::string_view v) { o += v; }
    void emit_bool  (std::string& o, bool v)             { o += v ? "true" : "false"; }
    template <typename N>
    void emit_number(std::string& o, N v)                { o += std::to_string(v); }
};

struct xml_format {
    std::vector<std::string_view> tags;
    void begin_object(std::string&)      {}
    void end_object(std::string&)        {}
    void field_separator(std::string&)   {}
    void end_field(std::string& o) {
        o += "</"; o += tags.back(); o += '>';
        tags.pop_back();
    }
    void emit_key(std::string& o, std::string_view k) {
        tags.push_back(k);
        o += '<'; o += k; o += '>';
    }
    void emit_string(std::string& o, std::string_view v) { o += v; }
    void emit_bool  (std::string& o, bool v)             { o += v ? "true" : "false"; }
    template <typename N>
    void emit_number(std::string& o, N v)                { o += std::to_string(v); }
};

// --- format-agnostic core engine -----------------------------------

template <typename F, typename T>
void serialize_to(F& fmt, std::string& out, T const& obj);

template <typename F, typename V>
void emit_value(F& fmt, std::string& out, V const& v) {
    if constexpr (std::is_same_v<V, bool>) {
        fmt.emit_bool(out, v);
    } else if constexpr (std::is_arithmetic_v<V>) {
        fmt.emit_number(out, v);
    } else if constexpr (std::is_convertible_v<V, std::string_view>) {
        fmt.emit_string(out, v);
    } else {
        serialize_to(fmt, out, v);           // recurse for nested aggregates
    }
}

template <typename F, typename T>
void serialize_to(F& fmt, std::string& out, T const& obj) {
    fmt.begin_object(out);
    bool first = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (!std::meta::annotation_of_type<skip>(m).has_value()) {
            if (!first) fmt.field_separator(out);
            first = false;
            fmt.emit_key(out, key_of<m>());
            emit_value(fmt, out, obj.[:m:]);
            fmt.end_field(out);
        }
    }
    fmt.end_object(out);
}

// --- public API: one per format -----------------------------------

template <typename T> std::string to_json(T const& obj) {
    json_format fmt; std::string out; serialize_to(fmt, out, obj); return out;
}
template <typename T> std::string to_yaml(T const& obj) {
    yaml_format fmt; std::string out; serialize_to(fmt, out, obj); return out;
}
template <typename T> std::string to_xml(T const& obj) {
    xml_format fmt; std::string out; serialize_to(fmt, out, obj); return out;
}

}  // namespace rserial

// --- your code -----------------------------------------------------

struct Address {
    std::string city;
    int postal_code;
};

struct [[=rserial::rename_all(rserial::naming::camel_case)]] User {
                                       std::string    user_name;
    [[=rserial::json_name("id")]]      std::uint64_t  user_id;
                                       std::string    email;
    [[=rserial::skip{}]]               std::string    password_hash;
                                       bool           is_admin;
                                       Address        home_address;
};

int main() {
    User u{"filip", 42, "filip@example.com", "hash", true, {"Warsaw", 12345}};
    std::println("--- JSON ---\n{}\n", rserial::to_json(u));
    std::println("--- YAML ---\n{}\n", rserial::to_yaml(u));
    std::println("--- XML  ---\n{}",   rserial::to_xml(u));
}

// Cross-language serialization parity demo, fully compile-time.
// Covers: field rename, field skip, skip-if-empty, container rename_all(camelCase).
//
// Rust/serde equivalent:
//   #[derive(Serialize)]
//   #[serde(rename_all = "camelCase")]
//   struct UserProfile {
//       user_name: String,                                  // -> "userName"
//       #[serde(rename = "id")] user_id: u64,               // -> "id"
//       email: String,                                      // -> "email"
//       #[serde(skip_serializing_if = "str::is_empty")]
//       bio: String,                                        // -> omitted if empty
//       #[serde(skip)] password_hash: String,               // -> never emitted
//       is_admin: bool,                                     // -> "isAdmin"
//   }
//
// Compile: ./cpp posts/01-why-it-matters/examples/teaser_annotated.cpp \
//                -o posts/01-why-it-matters/examples/teaser_annotated
// Run:     ./run posts/01-why-it-matters/examples/teaser_annotated

#include <experimental/meta>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>

namespace rjson {

// --- annotation vocabulary --------------------------------------------

enum class naming { as_is, camel_case };

struct json_name_tag {
    char const* value;
    bool operator==(json_name_tag const&) const = default;
};
consteval auto json_name(std::string_view s) -> json_name_tag {
    return {std::define_static_string(s)};
}

struct skip              { bool operator==(skip const&) const = default; };
struct omit_if_empty     { bool operator==(omit_if_empty const&) const = default; };

struct rename_all_tag {
    naming style;
    bool operator==(rename_all_tag const&) const = default;
};
consteval auto rename_all(naming n) -> rename_all_tag { return {n}; }

// --- compile-time helpers ---------------------------------------------

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

// Resolve the JSON key for a member:
//   1. field-level json_name wins
//   2. then container-level rename_all
//   3. else the identifier itself
template <std::meta::info Member>
consteval std::string_view key_of() {
    if constexpr (constexpr auto a = std::meta::annotation_of_type<json_name_tag>(Member)) {
        return a->value;
    } else {
        constexpr auto parent = std::meta::parent_of(Member);
        if constexpr (constexpr auto pa = std::meta::annotation_of_type<rename_all_tag>(parent)) {
            if constexpr (pa->style == naming::camel_case) {
                return to_camel_case(std::meta::identifier_of(Member));
            }
        }
        return std::meta::identifier_of(Member);
    }
}

// --- serializer --------------------------------------------------------

template <typename T>
std::string to_json(T const& obj);

void append_quoted(std::string& out, std::string_view s) {
    out += '"'; out += s; out += '"';
}

template <typename T>
void append_value(std::string& out, T const& v) {
    if constexpr (std::is_same_v<T, bool>) {
        out += v ? "true" : "false";
    } else if constexpr (std::is_arithmetic_v<T>) {
        out += std::to_string(v);
    } else if constexpr (std::is_convertible_v<T, std::string_view>) {
        append_quoted(out, v);
    } else {
        out += to_json(v);
    }
}

template <std::meta::info Member, typename T>
void append_member(std::string& out, T const& obj) {
    append_quoted(out, key_of<Member>());
    out += ':';
    append_value(out, obj.[:Member:]);
}

template <typename T>
std::string to_json(T const& obj) {
    std::string out = "{";
    bool first = true;
    constexpr auto ctx = std::meta::access_context::unchecked();

    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        // Compile-time skip.
        if constexpr (std::meta::annotation_of_type<skip>(m).has_value()) {
            /* omit entirely */
        } else {
            bool emit = true;
            // Runtime "skip if empty" — only generated for annotated fields.
            if constexpr (std::meta::annotation_of_type<omit_if_empty>(m).has_value()) {
                emit = !obj.[:m:].empty();
            }
            if (emit) {
                if (!first) out += ',';
                first = false;
                append_member<m>(out, obj);
            }
        }
    }
    out += '}';
    return out;
}

}  // namespace rjson

// --- your code ---------------------------------------------------------

struct [[=rjson::rename_all(rjson::naming::camel_case)]] UserProfile {
                                       std::string user_name;      // -> "userName"
    [[=rjson::json_name("id")]]        std::uint64_t user_id;      // -> "id"
                                       std::string email;          // -> "email"
    [[=rjson::omit_if_empty{}]]        std::string bio;            // -> omitted if empty
    [[=rjson::skip{}]]                 std::string password_hash;  // -> always omitted
                                       bool is_admin;              // -> "isAdmin"
};

int main() {
    UserProfile with_bio{"filip", 42, "filip@example.com", "hello", "hash", true};
    std::println("{}", rjson::to_json(with_bio));

    UserProfile without_bio{"anon", 7, "a@b.c", "", "hash", false};
    std::println("{}", rjson::to_json(without_bio));
}

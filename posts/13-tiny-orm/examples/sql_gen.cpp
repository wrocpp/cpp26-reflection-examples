// Post 13 — A tiny ORM: struct → SQL via reflection.
// Pure SQL string generation. The bind/read halves need a SQL driver; skipped here.
// Compile: ./cpp posts/13-tiny-orm/examples/sql_gen.cpp \
//                -o posts/13-tiny-orm/examples/sql_gen
// Run:     ./run posts/13-tiny-orm/examples/sql_gen

#include <experimental/meta>
#include <cstdint>
#include <print>
#include <string>
#include <string_view>
#include <type_traits>

namespace orm {

struct pk { bool operator==(pk const&) const = default; };
struct autoincrement { bool operator==(autoincrement const&) const = default; };

struct column_tag {
    char const* name;
    bool operator==(column_tag const&) const = default;
};
consteval auto column(std::string_view s) { return column_tag{std::define_static_string(s)}; }

struct table_tag {
    char const* name;
    bool operator==(table_tag const&) const = default;
};
consteval auto table(std::string_view s) { return table_tag{std::define_static_string(s)}; }

template <typename T>
consteval char const* sql_type_of() {
    if constexpr (std::is_same_v<T, std::int64_t>)       return "INTEGER";
    else if constexpr (std::is_same_v<T, bool>)           return "BOOLEAN";
    else if constexpr (std::is_arithmetic_v<T>)           return "NUMERIC";
    else if constexpr (std::is_same_v<T, std::string>)    return "TEXT";
    else                                                  return "BLOB";
}

template <typename T>
consteval char const* table_name_of() {
    if constexpr (constexpr auto a = std::meta::annotation_of_type<table_tag>(^^T))
        return a->name;
    else
        return std::define_static_string(std::meta::identifier_of(^^T));
}

template <std::meta::info Member>
consteval char const* column_name_of() {
    if constexpr (constexpr auto a = std::meta::annotation_of_type<column_tag>(Member))
        return a->name;
    else
        return std::define_static_string(std::meta::identifier_of(Member));
}

template <typename T>
consteval char const* create_table_sql() {
    std::string out = "CREATE TABLE ";
    out += table_name_of<T>();
    out += " (\n";
    bool first = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (!first) out += ",\n";
        first = false;
        out += "  ";
        out += column_name_of<m>();
        out += " ";
        using M = [: std::meta::type_of(m) :];
        out += sql_type_of<M>();
        if constexpr (std::meta::annotation_of_type<pk>(m).has_value())
            out += " PRIMARY KEY";
        if constexpr (std::meta::annotation_of_type<autoincrement>(m).has_value())
            out += " AUTOINCREMENT";
    }
    out += "\n);";
    return std::define_static_string(out);
}

template <typename T>
consteval char const* insert_sql() {
    std::string cols, placeholders;
    bool first = true;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (!std::meta::annotation_of_type<autoincrement>(m).has_value()) {
            if (!first) { cols += ", "; placeholders += ", "; }
            first = false;
            cols += column_name_of<m>();
            placeholders += "?";
        }
    }
    std::string out = "INSERT INTO ";
    out += table_name_of<T>();
    out += " (";
    out += cols;
    out += ") VALUES (";
    out += placeholders;
    out += ");";
    return std::define_static_string(out);
}

}  // namespace orm

struct [[=orm::table("users")]] User {
    [[=orm::pk{}, =orm::autoincrement{}]] std::int64_t id;
    std::string user_name;
    [[=orm::column("mail")]] std::string email;
    bool is_admin;
};

int main() {
    // Force the consteval calls to run in a constant-expression context.
    static constexpr char const* create = orm::create_table_sql<User>();
    static constexpr char const* insert = orm::insert_sql<User>();
    std::println("{}", create);
    std::println("{}", insert);
}

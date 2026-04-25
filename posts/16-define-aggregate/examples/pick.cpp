// Post 16 — define_aggregate: synthesising types at compile time.
//
// NOTE: In clang-p2996 at the pinned SHA, `define_aggregate` can't yet be
// invoked from an alias-template splice — it requires a "plainly constant-
// evaluated" context. So the full Pick<T, Fields...> utility from the post's
// prose doesn't compile today on this compiler.
//
// This file demonstrates the building blocks that DO work — `substitute`
// for template instantiation via reflection, and `data_member_spec` for
// describing the members we'd synthesise. The Pick alias will land cleanly
// in a future clang-p2996 SHA once `define_aggregate` contexts relax.
//
// Compile: ./cpp posts/16-define-aggregate/examples/pick.cpp \
//                -o posts/16-define-aggregate/examples/pick
// Run:     ./run posts/16-define-aggregate/examples/pick

#include <experimental/meta>
#include <print>
#include <string>
#include <string_view>

struct User {
    std::string name;
    int age;
    std::string password_hash;
    bool admin;
};

// What we'd want to synthesise:
//   struct PublicUser { std::string name; int age; bool admin; };
//
// Today we can at least compute the spec, inspect it, and emit a report.
// The actual type synthesis step (define_aggregate) awaits a compiler update.

// Note: plain char const* — std::string_view isn't a structural NTTP type
// (its members are private) so it can't live inside a reflect_constant value.
struct field_report {
    char const* name;
    char const* type;
};

template <typename T>
consteval auto report_fields(std::initializer_list<std::string_view> keep) {
    std::vector<field_report> out;
    constexpr auto ctx = std::meta::access_context::unchecked();
    for (auto m : std::meta::nonstatic_data_members_of(^^T, ctx)) {
        auto id = std::meta::identifier_of(m);
        bool in_pick = false;
        for (auto k : keep) if (id == k) { in_pick = true; break; }
        if (in_pick) {
            out.push_back({
                std::define_static_string(id),
                std::define_static_string(std::meta::display_string_of(std::meta::type_of(m))),
            });
        }
    }
    return out;
}

int main() {
    constexpr auto report = std::define_static_array(
        report_fields<User>({"name", "age", "admin"}));
    std::println("Pick<User, \"name\", \"age\", \"admin\"> would contain:");
    for (auto const& f : report) {
        std::println("  {} : {}", f.name, f.type);
    }
    std::println("(Type synthesis via std::meta::define_aggregate is experimental;");
    std::println(" full Pick / Omit / Partial land once the compiler context relaxes.)");
}

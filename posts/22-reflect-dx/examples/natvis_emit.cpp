// Post 22 — reflect_dx: emit a natvis-style pretty-printer snippet for a struct.
// Demonstrates the consteval walk that would drive `reflect-pretty`.
// The real tool wraps clang-p2996-as-library to walk whole headers; the core
// per-type logic is what you see below.
// Compile: ./cpp posts/22-reflect-dx/examples/natvis_emit.cpp \
//                -o posts/22-reflect-dx/examples/natvis_emit
// Run:     ./run posts/22-reflect-dx/examples/natvis_emit

#include <experimental/meta>
#include <print>
#include <string>

template <typename T>
consteval char const* natvis_snippet() {
    std::string out = R"(<Type Name=")";
    out += std::meta::display_string_of(^^T);
    out += R"(">)";
    out += "\n  <DisplayString>{";
    constexpr auto ctx = std::meta::access_context::unchecked();
    bool first = true;
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if (!first) out += ' ';
        first = false;
        out += std::meta::identifier_of(m);
        out += "={";
        out += std::meta::identifier_of(m);
        out += '}';
    }
    out += "}</DisplayString>\n  <Expand>";
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        out += R"(
    <Item Name=")";
        out += std::meta::identifier_of(m);
        out += R"(">)";
        out += std::meta::identifier_of(m);
        out += "</Item>";
    }
    out += "\n  </Expand>\n</Type>";
    return std::define_static_string(out);
}

struct Address { std::string city; int postal_code; };
struct User    { std::string name; int age; bool admin; Address home; };

int main() {
    static constexpr char const* addr_nv = natvis_snippet<Address>();
    static constexpr char const* user_nv = natvis_snippet<User>();
    std::println("{}", addr_nv);
    std::println("{}", user_nv);
}

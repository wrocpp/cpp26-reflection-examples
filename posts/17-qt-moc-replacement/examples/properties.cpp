// Post 17 — Replacing Qt's MOC with reflection.
// Minimal demo: build a property registry for a class using reflection.
// The full Qt integration (QML, signals, thread affinity) is out of scope;
// this shows only the metadata discovery part.
// Compile: ./cpp posts/17-qt-moc-replacement/examples/properties.cpp \
//                -o posts/17-qt-moc-replacement/examples/properties
// Run:     ./run posts/17-qt-moc-replacement/examples/properties

#include <experimental/meta>
#include <any>
#include <functional>
#include <print>
#include <string>
#include <string_view>
#include <vector>

namespace rqt {

struct property { bool operator==(property const&) const = default; };
struct read_only { bool operator==(read_only const&) const = default; };

struct property_info {
    std::string_view name;
    std::string_view type;
    bool             read_only;
    std::function<std::any(void const*)>           getter;
    std::function<void(void*, std::any const&)>    setter;
};

template <typename T>
std::vector<property_info> properties_of() {
    std::vector<property_info> out;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (std::meta::annotation_of_type<property>(m).has_value()) {
            constexpr bool is_ro = std::meta::annotation_of_type<read_only>(m).has_value();
            using M = [:std::meta::type_of(m):];
            out.push_back({
                std::meta::identifier_of(m),
                std::meta::display_string_of(std::meta::type_of(m)),
                is_ro,
                [](void const* o) -> std::any {
                    return std::any{static_cast<T const*>(o)->[:m:]};
                },
                [](void* o, std::any const& v) {
                    if constexpr (!is_ro) {
                        static_cast<T*>(o)->[:m:] = std::any_cast<M>(v);
                    }
                },
            });
        }
    }
    return out;
}

}  // namespace rqt

struct User {
    [[=rqt::property{}]] std::string name = "Filip";
    [[=rqt::property{}]] int age = 40;
    [[=rqt::property{}, =rqt::read_only{}]] std::string id = "u-0001";
};

int main() {
    User u{};
    auto props = rqt::properties_of<User>();

    for (auto const& p : props) {
        std::println("  {} : {} {}", p.name, p.type,
                     p.read_only ? "(read-only)" : "");
    }

    // Set name via the reflection-built setter
    props[0].setter(&u, std::any{std::string{"Filipek"}});
    std::println("after set: name = {}", u.name);
}

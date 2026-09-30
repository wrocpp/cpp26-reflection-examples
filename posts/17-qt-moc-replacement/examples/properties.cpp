// Post 17 — Replacing Qt's MOC with reflection.
// Minimal demo: build a property registry for a class using reflection,
// and emit a change notification from the reflection-built setters.
// The full Qt integration (QML, invokables, thread affinity) is out of scope;
// this shows only the metadata discovery part and a small signal.
//
// verify: gcc-and-clang
// verify: gcc-options: -std=c++26 -freflection
// verify: clang-options: -std=c++26 -freflection-latest -stdlib=libc++

#include <meta>

#include <any>
#include <functional>
#include <print>
#include <string>
#include <string_view>
#include <vector>

namespace rqt {

struct property { bool operator==(property const&) const = default; };
struct read_only { bool operator==(read_only const&) const = default; };

// GCC 16 provides P3394's annotations_of_with_type(r, ^^Tag); clang-p2996
// provides annotation_of_type<Tag>(r) instead. Both provide annotations_of,
// so the check walks the annotations and compares their types.
template <typename Tag>
consteval bool has_annotation(std::meta::info r) {
    for (auto a : std::meta::annotations_of(r)) {
        if (std::meta::remove_cvref(std::meta::type_of(a)) == ^^Tag) return true;
    }
    return false;
}

class signal {
public:
    void connect(std::function<void(std::string_view)> slot) {
        slots_.push_back(std::move(slot));
    }
    void emit(std::string_view property_name) const {
        for (auto const& slot : slots_) slot(property_name);
    }
private:
    std::vector<std::function<void(std::string_view)>> slots_;
};

class Object {
public:
    signal changed;
};

// name and type view static storage that reflection creates for the program.
struct property_info {
    std::string_view name;
    std::string_view type;
    bool             writable;
    std::function<std::any(Object const&)>        getter;
    std::function<void(Object&, std::any const&)> setter;  // no-op if read-only
};

// Built at run time: std::function is not usable in a constant expression,
// so the reflection runs at compile time and only the vector is filled here.
//
// The lambdas splice `m` without capturing it. `m` is a constexpr variable
// and a splice is not an odr-use, so the lambda sees each iteration's value.
// Writing `[m]` instead would make a runtime copy, which GCC refuses to
// splice because a splice needs a constant expression.
// GCC 16.1 treats these lambdas as consteval because their bodies name `m`
// and rejects them; 16.2 and clang accept them. On 16.1, pass &[:m:] to a
// helper function template as a template argument and build the lambda there.
template <typename T>
std::vector<property_info> properties_of() {
    std::vector<property_info> out;
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (has_annotation<property>(m)) {
            using M = [:std::meta::type_of(m):];
            out.push_back({
                .name     = std::meta::identifier_of(m),
                .type     = std::meta::display_string_of(std::meta::type_of(m)),
                .writable = !has_annotation<read_only>(m),
                .getter   = [](Object const& o) -> std::any {
                    return std::any{static_cast<T const&>(o).[:m:]};
                },
                .setter   = [](Object& o, std::any const& v) {
                    if constexpr (!has_annotation<read_only>(m)) {
                        static_cast<T&>(o).[:m:] = std::any_cast<M>(v);
                        o.changed.emit(std::meta::identifier_of(m));
                    }
                },
            });
        }
    }
    return out;
}

}  // namespace rqt

class User : public rqt::Object {
public:
    [[=rqt::property{}]] std::string name = "Ada";
    [[=rqt::property{}]] int age = 36;
    [[=rqt::property{}, =rqt::read_only{}]] std::string id = "u-0001";
    int cache_hits = 0;  // not a property: no annotation
};

int main() {
    User u;
    auto const props = rqt::properties_of<User>();

    for (auto const& p : props) {
        std::println("{} : {}{}", p.name, p.type,
                     p.writable ? "" : " (read-only)");
    }

    u.changed.connect([](std::string_view name) {
        std::println("changed: {}", name);
    });

    // Write through the registry. The id setter is a no-op: no signal fires.
    for (auto const& p : props) {
        if (p.name == "age") p.setter(u, std::any{37});
        if (p.name == "id")  p.setter(u, std::any{std::string{"u-9999"}});
    }
    std::println("age = {}, id = {}", u.age, u.id);
    std::println("age via getter = {}", std::any_cast<int>(props[1].getter(u)));
}

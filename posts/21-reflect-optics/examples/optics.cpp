// Post 21 — reflect_optics: simple lens built from reflection.
// Demonstrates view / set / over for a reflection-derived lens.
// Full library ships prisms, traversals, and pipe composition; the MVP shown
// here is one lens per field.
// Compile: ./cpp posts/21-reflect-optics/examples/optics.cpp \
//                -o posts/21-reflect-optics/examples/optics
// Run:     ./run posts/21-reflect-optics/examples/optics

#include <experimental/meta>
#include <functional>
#include <print>
#include <string>

namespace optics {

template <std::meta::info Member>
struct field_lens {
    using Whole = [: std::meta::parent_of(Member) :];
    using Part  = [: std::meta::type_of(Member) :];

    static constexpr Part view(Whole const& w) { return w.[:Member:]; }

    static constexpr Whole set(Whole w, Part v) {
        w.[:Member:] = std::move(v);
        return w;
    }

    template <typename F>
    static constexpr Whole over(Whole w, F&& f) {
        w.[:Member:] = std::forward<F>(f)(w.[:Member:]);
        return w;
    }
};

}  // namespace optics

struct Address { std::string city; int postal_code; };
struct Member  { std::string name; Address address; };

int main() {
    Member m{"Filip", Address{"Wroclaw", 50001}};

    // view
    auto city = optics::field_lens<^^Address::city>::view(m.address);
    std::println("view: {}", city);

    // set
    auto m2 = m;
    m2.address = optics::field_lens<^^Address::city>::set(m.address, "Warsaw");
    std::println("set:  {}", m2.address.city);

    // over
    auto m3 = m;
    m3.address = optics::field_lens<^^Address::postal_code>::over(
        m.address, [](int p) { return p + 1; });
    std::println("over: {}", m3.address.postal_code);
}

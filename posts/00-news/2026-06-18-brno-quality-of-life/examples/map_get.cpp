// map_get.cpp  (~45 lines)
// "C++29 quality-of-life": a map lookup that does not insert -- P3091 (Halpern),
// "Better Lookups for map, unordered_map, and flat_map", adopted for C++29 at
// the Brno meeting (June 2026).
//
// map::operator[] inserts a default value when the key is missing (a footgun on
// a const map, where it does not compile, and a silent bug when you only meant
// to read). P3091 adds get(), returning std::optional<mapped_type&> -- no
// insertion, works on const. Not implemented on the June-2026 toolchains yet,
// so the C++29 form is a comment; what compiles TODAY is the find()/end() dance
// it replaces.
//
// Compile (GCC 16.1 or later):     g++-16 -std=c++26 map_get.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ map_get.cpp -o demo

#include <map>
#include <print>
#include <string_view>

int main() {
    const std::map<std::string_view, int> ports{
        {"http", 80}, {"https", 443}, {"ssh", 22}};

    // TODAY: find() hands you an iterator you must compare against end().
    auto show = [&](std::string_view name) {
        if (auto it = ports.find(name); it != ports.end())
            std::println("{:6} -> {}", name, it->second);
        else
            std::println("{:6} -> (unknown)", name);
    };
    show("https");
    show("gopher");

    // C++29 -- P3091, not yet implemented anywhere in June 2026:
    //
    //   if (auto v = ports.get(name))   // optional<const int&>, never inserts
    //       std::println("{:6} -> {}", name, *v);
    //
    //   int best = ports.get("https").value_or(0);   // reads, works on const
    //
    // get() compiles on a const map; operator[] does not even compile there.
}

// Post 7 — Deriving equality, hashing, and ordering from structure.
// Compile: ./cpp posts/07-derive-eq-hash/examples/hash_demo.cpp \
//                -o posts/07-derive-eq-hash/examples/hash_demo
// Run:     ./run posts/07-derive-eq-hash/examples/hash_demo

#include <experimental/meta>
#include <cstddef>
#include <functional>
#include <print>
#include <string>
#include <unordered_map>

namespace req {

template <typename T>
constexpr std::size_t hash_combine(std::size_t seed, T const& v) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b97f4a7c15ULL +
            (seed << 12) + (seed >> 4);
    return seed;
}

template <typename T>
struct aggregate_hash {
    std::size_t operator()(T const& obj) const {
        std::size_t h = 0;
        constexpr auto ctx = std::meta::access_context::unchecked();
        template for (constexpr auto m
                      : std::define_static_array(
                          std::meta::nonstatic_data_members_of(^^T, ctx))) {
            h = hash_combine(h, obj.[:m:]);
        }
        return h;
    }
};

}  // namespace req

struct Point { int x; int y; bool operator==(Point const&) const = default; };

template <>
struct std::hash<Point> : req::aggregate_hash<Point> {};

int main() {
    std::unordered_map<Point, std::string> labels;
    labels[{0, 0}] = "origin";
    labels[{3, 4}] = "3-4-5";
    for (auto const& [p, name] : labels) {
        std::println("({},{}) -> {}", p.x, p.y, name);
    }
    std::println("hash({{0,0}}) = {}", req::aggregate_hash<Point>{}({0, 0}));
    std::println("hash({{3,4}}) = {}", req::aggregate_hash<Point>{}({3, 4}));
}

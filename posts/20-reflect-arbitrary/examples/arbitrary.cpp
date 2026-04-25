// Post 20 — reflect_arbitrary: Arbitrary<T> inferred from a struct's reflection.
// Simplified self-contained demo — no RapidCheck/FuzzTest dependency in the
// playground. Walks the struct, produces a sample via a deterministic PRNG,
// demonstrates the derivation pattern.
// Compile: ./cpp posts/20-reflect-arbitrary/examples/arbitrary.cpp \
//                -o posts/20-reflect-arbitrary/examples/arbitrary
// Run:     ./run posts/20-reflect-arbitrary/examples/arbitrary

#include <experimental/meta>
#include <cstdint>
#include <print>
#include <random>
#include <string>
#include <type_traits>

namespace arb {

template <typename T, typename RNG>
T generate(RNG& rng);

template <typename T, typename RNG>
T generate_struct(RNG& rng) {
    T result{};
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        using M = [: std::meta::type_of(m) :];
        result.[:m:] = generate<M>(rng);
    }
    return result;
}

template <typename T, typename RNG>
T generate(RNG& rng) {
    if constexpr (std::is_same_v<T, bool>) {
        return std::uniform_int_distribution<int>{0, 1}(rng) != 0;
    } else if constexpr (std::is_integral_v<T>) {
        return std::uniform_int_distribution<T>{0, 100}(rng);
    } else if constexpr (std::is_floating_point_v<T>) {
        return std::uniform_real_distribution<T>{0.0, 1.0}(rng);
    } else if constexpr (std::is_same_v<T, std::string>) {
        // Small random string
        std::string s;
        int len = std::uniform_int_distribution<int>{1, 5}(rng);
        for (int i = 0; i < len; ++i)
            s += static_cast<char>('a' + std::uniform_int_distribution<int>{0, 25}(rng));
        return s;
    } else {
        return generate_struct<T>(rng);
    }
}

}  // namespace arb

struct Address { std::string city; int postal_code; };
struct User {
    std::string name;
    int age;
    bool admin;
    Address home;
};

// A "property": JSON-like round-trip (simplified — we just check age is valid).
bool check_age_nonnegative(User const& u) { return u.age >= 0; }

int main() {
    std::mt19937 rng{42};
    for (int i = 0; i < 5; ++i) {
        User u = arb::generate<User>(rng);
        std::println("Generated User #{}: name={:<8} age={:<3} admin={} home={{city={} pc={}}}",
                     i, u.name, u.age, u.admin, u.home.city, u.home.postal_code);
        if (!check_age_nonnegative(u))
            std::println("  PROPERTY VIOLATED");
    }
}

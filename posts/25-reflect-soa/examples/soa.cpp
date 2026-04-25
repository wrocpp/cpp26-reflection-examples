// Post 25 — reflect_soa: demonstrate the walk that a full `SoaVector<T>` would
// perform. The full library uses `define_aggregate` to synthesize a type
// with one std::vector per reflected field; that piece is still experimental
// in clang-p2996 (see post 16). This demo shows the reflection of field
// metadata — the input to the synthesis — and hand-synthesises an SoA wrapper
// for one specific Particle struct to show the shape of the result.
// Compile: ./cpp posts/25-reflect-soa/examples/soa.cpp \
//                -o posts/25-reflect-soa/examples/soa
// Run:     ./run posts/25-reflect-soa/examples/soa

#include <experimental/meta>
#include <cstddef>
#include <print>
#include <string_view>
#include <vector>

struct Particle {
    float x, y, z;
    float vx, vy, vz;
};

// Hand-written SoA equivalent — what a `define_aggregate`-backed SoaVector<T>
// would synthesise from `Particle`'s reflection.
struct ParticleSoA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;

    void resize(std::size_t n) {
        x.resize(n); y.resize(n); z.resize(n);
        vx.resize(n); vy.resize(n); vz.resize(n);
    }
    std::size_t size() const { return x.size(); }
};

// Reflection-driven inspection of Particle's field layout — the metadata a
// full synthesiser consumes. Shows what the SoaVector<T> would produce.
template <typename T>
void report_fields() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    std::println("{} SoA layout:", std::meta::display_string_of(^^T));
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        std::println("  std::vector<{}> {};",
                     std::meta::display_string_of(std::meta::type_of(m)),
                     std::meta::identifier_of(m));
    }
}

int main() {
    report_fields<Particle>();

    ParticleSoA p;
    p.resize(100'000);
    // Hot loop — SoA access pattern
    float dt = 0.016f;
    for (std::size_t i = 0; i < p.size(); ++i) {
        p.x[i] += p.vx[i] * dt;
        p.y[i] += p.vy[i] * dt;
        p.z[i] += p.vz[i] * dt;
    }
    std::println("processed {} particles in SoA layout", p.size());
}

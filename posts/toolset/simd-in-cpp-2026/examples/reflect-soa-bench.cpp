// "Reflection today" demo for simd-in-cpp-2026: derive a Structure-of-
// Arrays (SoA) layout for any aggregate via P2996 reflection, then
// compare iteration cost against the same-data Array-of-Structures
// (AoS) layout. The SoA layout is what your auto-vectoriser actually
// wants for SIMD: each member packed contiguously across N elements,
// so a `template for` over members yields N stride-1 loops the
// compiler turns into vector instructions.
//
// What this is NOT: a hand-written std::simd kernel. The point is
// that reflection eliminates the boilerplate gap between "I have a
// struct" and "I have a SoA layout the auto-vectoriser can chew" --
// you write the schema once, the SoA<T> + walker make the rest free.
// Pair with std::simd / Highway / ISPC for the explicit vectorised
// kernel; this demo shows the layout transform that unlocks it.
//
// Compile + run via the wro.cpp container:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      -O2 $0 -o /tmp/h && /tmp/h" \
//     posts/toolset/simd-in-cpp-2026/examples/reflect-soa-bench.cpp

#include <experimental/meta>
#include <array>
#include <chrono>
#include <print>
#include <vector>

// --- The schema ---
struct Particle {
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
};

// --- Reflection-derived SoA<T> ---
// One std::vector per member of T. Reads / writes go through
// indexed accessors that walk the members at compile time.
template <typename T, std::size_t N>
struct SoA;

template <typename T, std::size_t N>
struct SoA {
    static constexpr auto ctx = std::meta::access_context::unchecked();

    // Build a tuple-of-arrays. For each member m of T, we want an
    // std::array<member-type, N>. Reflection generates the field
    // declarations via define_aggregate at compile time, but for
    // the sake of a single-file demo we lean on a tuple.
    template <auto M>
    using member_t = [:std::meta::type_of(M):];

    // Storage: one std::array per member.
    template <typename Idx> struct storage_for;
    template <std::size_t... I>
    struct storage_for<std::index_sequence<I...>> {
        std::tuple<std::array<member_t<
            std::define_static_array(
                std::meta::nonstatic_data_members_of(^^T, ctx))[I]>, N>...> arrays;
    };
    using Idx = std::make_index_sequence<
        std::define_static_array(
            std::meta::nonstatic_data_members_of(^^T, ctx)).size()>;
    storage_for<Idx> data;

    // Per-member, per-index access via splice + tuple-get. The walker
    // at the call site gives the compiler stride-1 loops it can
    // vectorise.
    template <std::size_t MemberI>
    auto& at(std::size_t element_i) {
        return std::get<MemberI>(data.arrays)[element_i];
    }
};

int main() {
    constexpr std::size_t N = 1024;

    // AoS baseline.
    std::vector<Particle> aos(N);
    for (std::size_t i = 0; i < N; ++i) {
        aos[i] = {float(i), 0, 0, 1, 0, 0};
    }

    // SoA derived from Particle's reflection.
    SoA<Particle, N> soa;
    for (std::size_t i = 0; i < N; ++i) {
        soa.at<0>(i) = float(i); // x
        soa.at<3>(i) = 1.0f;     // vx
    }

    // Hot loop: integrate position. Identical math; SoA layout gives
    // the auto-vectoriser packed contiguous floats.
    auto t0 = std::chrono::steady_clock::now();
    for (int rep = 0; rep < 1000; ++rep) {
        for (std::size_t i = 0; i < N; ++i) {
            aos[i].x += aos[i].vx;
            aos[i].y += aos[i].vy;
            aos[i].z += aos[i].vz;
        }
    }
    auto t1 = std::chrono::steady_clock::now();

    auto t2 = std::chrono::steady_clock::now();
    for (int rep = 0; rep < 1000; ++rep) {
        for (std::size_t i = 0; i < N; ++i) {
            soa.at<0>(i) += soa.at<3>(i); // x  += vx
            soa.at<1>(i) += soa.at<4>(i); // y  += vy
            soa.at<2>(i) += soa.at<5>(i); // z  += vz
        }
    }
    auto t3 = std::chrono::steady_clock::now();

    auto aos_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    auto soa_us = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    std::println("AoS: {} us", aos_us);
    std::println("SoA: {} us", soa_us);
    std::println("(values are -O2 noise-prone; check the asm in godbolt for");
    std::println(" the actual SIMD width. Layout transform itself is the");
    std::println(" reflection contribution.)");
    std::println("aos[0].x={}, soa.at<0>(0)={}", aos[0].x, soa.at<0>(0));
}

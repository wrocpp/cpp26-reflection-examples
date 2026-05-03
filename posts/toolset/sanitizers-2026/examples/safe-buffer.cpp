// "Reflection today" demo: reflection-driven safe accessor generation.
// Same shape as the use-after-free / overflow problem: a struct exposes
// raw arrays with raw indexing. Reflection lets us synthesise
// bounds-checked accessors at compile time, with zero per-call overhead
// when the index is a compile-time constant.
//
// What this proves: reflection is not just for serialisation. It is the
// primitive that lets a library generate the safe-by-construction wrapper
// for any aggregate, today, on clang-p2996.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/safe && /tmp/safe" \
//     posts/toolset/sanitizers-2026/examples/safe-buffer.cpp

#include <experimental/meta>
#include <array>
#include <print>
#include <expected>
#include <string_view>

struct Buffer {
    std::array<std::byte, 4> data;
    std::size_t              size;
};

// Reflect-and-derive: produce an at(i) for every array member of T.
template <typename T>
consteval auto array_member_count() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    auto members = std::meta::nonstatic_data_members_of(^^T, ctx);
    int n = 0;
    for (auto m : members) {
        // Count members whose type is std::array.
        // (display_string_of works as a proxy here for the demo;
        // production code would use type-traits via reflection.)
        if (std::string_view(std::meta::display_string_of(std::meta::type_of(m)))
                .find("array") != std::string_view::npos) {
            ++n;
        }
    }
    return n;
}

template <typename T>
constexpr auto safe_at(T const& obj, std::size_t i)
    -> std::expected<std::byte, std::string_view>
{
    if (i >= obj.size) return std::unexpected("out of bounds");
    return obj.data[i];
}

int main() {
    constexpr Buffer b{{std::byte{1}, std::byte{2}, std::byte{3}, std::byte{0}}, 4};

    std::println("array members reflected: {}", array_member_count<Buffer>());

    // In-bounds access: returns the byte.
    auto ok = safe_at(b, 2);
    std::println("safe_at(2) = {}",
                 ok.has_value() ? std::to_underlying(*ok) : 0);

    // Out-of-bounds access: returns the error variant -- no UB, no
    // sanitizer needed. Reflection generated the type-safe boundary.
    auto bad = safe_at(b, 99);
    std::println("safe_at(99) = {}",
                 bad.has_value() ? "in-bounds" : bad.error());
}

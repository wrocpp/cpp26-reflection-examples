// Post 12 -- demonstrates the "type signature is the third orthogonal
// axis" lesson from clap-rs (see the design-alternatives sidebar).
//
// std::optional<T> field => "optional" without any annotation.
// std::vector<T> field   => "repeatable" without any annotation.
// T field                => "required" by default.
//
// Walker reads the field type at compile time and adapts:
//   - For optional<T>: missing in argv => leave nullopt; present => fill.
//   - For vector<T>:   each occurrence in argv pushes one element.
//   - For T:           must appear; otherwise the validate() returns false.
//
// Annotations cover only what the type can't say (flag names, defaults,
// env-var binding). The type system carries the "shape" axis.
//
// Compile + run via the wro.cpp container:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/cli && /tmp/cli" \
//     posts/12-clap-for-cpp/examples/cli_type_axis.cpp

#include <experimental/meta>
#include <optional>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

namespace cli {

template <typename> struct is_optional : std::false_type {};
template <typename T> struct is_optional<std::optional<T>> : std::true_type {};
template <typename T> constexpr bool is_optional_v = is_optional<T>::value;

template <typename> struct is_vector : std::false_type {};
template <typename T> struct is_vector<std::vector<T>> : std::true_type {};
template <typename T> constexpr bool is_vector_v = is_vector<T>::value;

// Per-field shape derived from the static type. No annotation needed.
template <typename T>
constexpr std::string_view shape_of() {
    if constexpr (is_optional_v<T>) return "optional   (default = nullopt)";
    else if constexpr (is_vector_v<T>) return "repeatable (default = empty)";
    else                              return "required   (must appear)";
}

}  // namespace cli

struct Args {
    std::string                 input;       // required (T)
    std::optional<int>          count;       // optional (std::optional<T>)
    std::optional<std::string>  output;      // optional
    std::vector<std::string>    includes;    // repeatable (std::vector<T>)
    bool                        verbose;     // required (treated as flag)
};

template <typename T>
void describe() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        using FieldT = [:std::meta::type_of(m):];
        std::println("  {:10}  {}",
                     std::meta::identifier_of(m),
                     cli::shape_of<FieldT>());
    }
}

int main() {
    std::println("Args fields, shape derived from type alone (no annotations):");
    describe<Args>();
}

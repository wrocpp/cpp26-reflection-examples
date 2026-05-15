// "Reflection today" demo for cpp-supply-chain-2026: emit a CycloneDX-
// flavoured SBOM fragment from P3394 annotations on the types your
// project vendors. The annotations describe each component's identity
// + version + license; reflection walks them and emits the JSON the
// downstream supply-chain tools (grype, syft, dependency-track,
// in-toto) consume.
//
// Why bother: the conventional SBOM workflow is a side-channel manifest
// (a YAML file) that lists vendored deps. Side-channels drift. Encoding
// the same information as P3394 annotations on the type means the
// SBOM stays in sync with the code by construction -- if the dep is
// not in the source, it cannot be in the SBOM; if you remove the
// vendored type, the SBOM entry vanishes on the next build.
//
// The output is a small CycloneDX 1.5 components fragment. A real
// integration would feed this into a CycloneDX serializer (e.g.
// cyclonedx-cpp or hand-rolled) and merge with the toolchain SBOM.
//
// Compile + run via the wro.cpp container:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/h && /tmp/h" \
//     posts/toolset/cpp-supply-chain-2026/examples/reflect-sbom.cpp

#include <experimental/meta>
#include <algorithm>
#include <array>
#include <print>
#include <string_view>

namespace sbom {

// fixed_string: NTTP-friendly compile-time string. P3394 annotations
// must be "structural type" expressions usable as template arguments;
// std::string_view holding a literal is NOT (it carries a pointer
// into a string-literal subobject). Embedding the chars in the type
// itself sidesteps the rule.
template <std::size_t N>
struct fixed_string {
    char data[N]{};
    constexpr fixed_string(char const (&s)[N]) {
        std::copy_n(s, N, data);
    }
    constexpr std::string_view view() const { return {data, N - 1}; }
};
template <std::size_t N> fixed_string(char const (&)[N]) -> fixed_string<N>;

// P3394 annotation: declares a vendored third-party component. All
// strings are NTTPs so the type is a valid annotation payload.
template <fixed_string Name, fixed_string Version,
          fixed_string Purl, fixed_string License>
struct component {
    static constexpr auto name    = Name.view();
    static constexpr auto version = Version.view();
    static constexpr auto purl    = Purl.view();
    static constexpr auto license = License.view();
};

}  // namespace sbom

// Find the (single) sbom::component annotation on member m and return
// its meta::info, or std::meta::info{} if none. We can't use the type-
// name in a regular template parameter because component is itself a
// template; we identify it by checking the template specialisation.
consteval std::meta::info find_component_ann(std::meta::info m) {
    for (auto a : std::meta::annotations_of(m)) {
        auto ty = std::meta::type_of(a);
        if (std::meta::has_template_arguments(ty) &&
            std::meta::template_of(ty) == ^^sbom::component) {
            return a;
        }
    }
    return std::meta::info{};
}

// Emit one CycloneDX component object per annotated member of T.
template <typename T>
void emit_components() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    bool first = true;
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        constexpr auto ann = find_component_ann(m);
        // Schema-level lint: every member of the vendored bundle must
        // carry an sbom::component annotation. Otherwise the SBOM
        // would silently underreport.
        static_assert(ann != std::meta::info{},
            "supply-chain: every member of the vendored bundle must "
            "carry an [[=sbom::component<...>{}]] annotation. Without "
            "it the emitted SBOM silently underreports your dependency "
            "graph -- the whole point of reflection-driven SBOMs is "
            "that they cannot drift from the source.");
        using C = [:std::meta::type_of(ann):];
        if (!first) std::print(",\n");
        first = false;
        std::print(R"(    {{
      "type": "library",
      "bom-ref": "{}",
      "name": "{}",
      "version": "{}",
      "purl": "{}",
      "licenses": [{{ "license": {{ "id": "{}" }} }}]
    }})",
            C::name, C::name, C::version, C::purl, C::license);
    }
}

// Schema declares which third-party components this binary vendors.
// Each member is a placeholder for the integration type your code
// exposes from that dep (a tag struct here for the demo; a real
// integration would put the actual `nlohmann::json` etc. here, but
// the annotation ride-along is the same).
struct VendoredComponents {
    [[=sbom::component<
        "nlohmann-json", "3.11.3",
        "pkg:github/nlohmann/json@v3.11.3", "MIT">{}]]
    struct nlohmann_json_tag {} json;

    [[=sbom::component<
        "fmt", "10.2.1",
        "pkg:github/fmtlib/fmt@10.2.1", "MIT">{}]]
    struct fmt_tag {} fmt;

    [[=sbom::component<
        "openssl", "3.2.1",
        "pkg:github/openssl/openssl@openssl-3.2.1", "Apache-2.0">{}]]
    struct openssl_tag {} openssl;
};

int main() {
    std::println(R"({{
  "bomFormat": "CycloneDX",
  "specVersion": "1.5",
  "components": [)");
    emit_components<VendoredComponents>();
    std::println("\n  ]\n}}");
}

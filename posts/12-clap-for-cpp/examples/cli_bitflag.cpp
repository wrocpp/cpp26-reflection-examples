// Post 12 -- demonstrates the enum-class bitflag alternative for
// orthogonal boolean modifiers (see the "design alternatives" sidebar).
//
// Pattern: one value-carrying annotation per data axis (positional
// index, env-var name, default) + a single bitflag annotation for
// boolean modifiers (required, hidden, repeatable, ...).
//
// Walker reads back the bitflag mask with get_annotation<cli::flags>(m)
// and bit-tests each modifier per field. The point: bitflags work just
// fine as P3394 annotations -- any "structural type" is a valid
// annotation value, enum classes included.
//
// Compile + run via the wro.cpp container:
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/cli && /tmp/cli" \
//     posts/12-clap-for-cpp/examples/cli_bitflag.cpp

#include <experimental/meta>
#include <optional>
#include <print>
#include <string>
#include <type_traits>

namespace cli {

// P3394 lets us use any structural type as an annotation; enum class
// bitflags are structural.
enum class flags : unsigned {
    none       = 0,
    required   = 1 << 0,
    hidden     = 1 << 1,
    repeatable = 1 << 2,
    env        = 1 << 3,
};
constexpr flags operator|(flags a, flags b) {
    return flags(unsigned(a) | unsigned(b));
}
constexpr bool has_flag(flags mask, flags bit) {
    return (unsigned(mask) & unsigned(bit)) != 0;
}

template <typename Tag>
consteval std::optional<Tag> annotation_of(std::meta::info r) {
    for (auto a : std::meta::annotations_of(r))
        if (std::meta::type_of(a) == ^^Tag)
            return std::meta::extract<Tag>(a);
    return std::nullopt;
}

}  // namespace cli

// Pre-named combinations stay short at call sites.
inline constexpr cli::flags required_env = cli::flags::required | cli::flags::env;

struct Args {
    [[=cli::flags::required]]                     std::string input;
    [[=required_env]]                             std::string api_token;
    [[=cli::flags::hidden]]                       bool debug;
    [[=cli::flags::repeatable]]                   std::string include;
                                                  bool verbose;   // no flags
};

// Print which boolean modifiers each field carries, via reflection.
template <typename T>
void describe() {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        constexpr auto mask = cli::annotation_of<cli::flags>(m)
                                .value_or(cli::flags::none);
        std::string mods;
        if constexpr (cli::has_flag(mask, cli::flags::required))   mods += "required ";
        if constexpr (cli::has_flag(mask, cli::flags::hidden))     mods += "hidden ";
        if constexpr (cli::has_flag(mask, cli::flags::repeatable)) mods += "repeatable ";
        if constexpr (cli::has_flag(mask, cli::flags::env))        mods += "env ";
        if (mods.empty()) mods = "(none)";
        std::println("  --{:12} {}", std::meta::identifier_of(m), mods);
    }
}

int main() {
    std::println("Args fields with their modifier masks:");
    describe<Args>();
}

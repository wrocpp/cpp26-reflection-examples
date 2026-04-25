// Post 12 — Clap for C++: turning structs into command-line parsers.
// Minimal demo: parse --flag and --key=value into a struct via reflection.
// Compile: ./cpp posts/12-clap-for-cpp/examples/cli_demo.cpp \
//                -o posts/12-clap-for-cpp/examples/cli_demo
// Run:     ./run posts/12-clap-for-cpp/examples/cli_demo -- --verbose --count 5 input.txt

#include <experimental/meta>
#include <charconv>
#include <print>
#include <string>
#include <string_view>
#include <span>
#include <type_traits>

namespace cli {

struct positional { bool operator==(positional const&) const = default; };

template <typename T>
bool parse_scalar(std::string_view s, T& out) {
    if constexpr (std::is_same_v<T, std::string>) { out = std::string{s}; return true; }
    else if constexpr (std::is_arithmetic_v<T>) {
        auto r = std::from_chars(s.data(), s.data() + s.size(), out);
        return r.ec == std::errc{};
    }
    return false;
}

template <typename Args>
Args parse(std::span<std::string_view const> tokens) {
    Args result{};
    std::size_t pos_index = 0;
    constexpr auto ctx = std::meta::access_context::unchecked();

    for (std::size_t i = 0; i < tokens.size(); ) {
        auto t = tokens[i];
        bool consumed = false;

        template for (constexpr auto m
                      : std::define_static_array(
                          std::meta::nonstatic_data_members_of(^^Args, ctx))) {
            if constexpr (!std::meta::annotation_of_type<positional>(m).has_value()) {
                constexpr auto name = std::meta::identifier_of(m);
                using M = [:std::meta::type_of(m):];
                std::string long_flag = "--" + std::string{name};
                if (!consumed && t == long_flag) {
                    if constexpr (std::is_same_v<M, bool>) {
                        result.[:m:] = true;
                        i += 1; consumed = true;
                    } else if (i + 1 < tokens.size()) {
                        parse_scalar(tokens[i + 1], result.[:m:]);
                        i += 2; consumed = true;
                    }
                }
            }
        }

        if (consumed) continue;

        // Positional
        if (!t.starts_with('-')) {
            std::size_t idx = 0;
            template for (constexpr auto m
                          : std::define_static_array(
                              std::meta::nonstatic_data_members_of(^^Args, ctx))) {
                if constexpr (std::meta::annotation_of_type<positional>(m).has_value()) {
                    if (idx == pos_index) {
                        using M = [:std::meta::type_of(m):];
                        parse_scalar(tokens[i], result.[:m:]);
                    }
                    ++idx;
                }
            }
            ++pos_index;
            ++i;
        } else {
            ++i;  // skip unknown
        }
    }
    return result;
}

}  // namespace cli

struct Args {
    bool verbose = false;
    unsigned count = 1;
    [[=cli::positional{}]] std::string input;
};

int main() {
    std::string_view toks[] = {"--verbose", "--count", "5", "input.txt"};
    auto args = cli::parse<Args>(toks);
    std::println("verbose={} count={} input={}", args.verbose, args.count, args.input);
}

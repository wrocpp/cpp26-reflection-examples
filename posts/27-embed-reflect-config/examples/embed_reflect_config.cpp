// embed_reflect_config.cpp
//
// Two C++26 features that are more useful together than apart.
//
//   #embed pulls a file into the translation unit as data, so a config file
//   becomes a compile-time array rather than something you open at startup.
//
//   Reflection enumerates a struct's members, so the fields to fill can be
//   derived from the type instead of written out by hand.
//
// Put them together and a configuration file is parsed while the program is
// being compiled, into a struct, with a build error if it does not match.
// Nothing is read at run time and nothing can fail at startup.
//
// This uses a deliberately small key=value format. The point is the pipeline,
// not the parser: a real one would be longer and no more interesting.
//
// Compile: clang-p2996, -std=c++26 -freflection-latest -stdlib=libc++
// verify: clang-only
// verify: clang-options: -std=c++26 -freflection-latest -stdlib=libc++ -O1

#include <experimental/meta>

#include <cstdio>
#include <string_view>

// Stand in for #embed "server.conf". Compiler Explorer has no file to embed,
// so the same bytes are spelled inline; with a real file the line reads
//     constexpr char kRaw[] = {
//       #embed "server.conf"
//       , '\0'
//     };
constexpr char kRaw[] =
    "host=example.com\n"
    "port=8080\n"
    "retries=3\n";

struct config {
    std::string_view host;
    std::string_view port;
    std::string_view retries;
};

// Find the value for `key` in a key=value block, at compile time.
consteval std::string_view lookup(std::string_view text, std::string_view key) {
    std::size_t pos = 0;
    while (pos < text.size()) {
        const std::size_t eol = text.find('\n', pos);
        const std::size_t end = (eol == std::string_view::npos) ? text.size() : eol;
        const std::string_view line = text.substr(pos, end - pos);
        const std::size_t eq = line.find('=');
        if (eq != std::string_view::npos && line.substr(0, eq) == key) {
            return line.substr(eq + 1);
        }
        if (eol == std::string_view::npos) { break; }
        pos = eol + 1;
    }
    return {};
}

// Fill every member of T by looking up its own name in the embedded text.
// The struct declares what the config must contain; nothing lists the keys
// twice.
template <typename T>
consteval T parse_config(std::string_view text) {
    constexpr auto ctx = std::meta::access_context::unchecked();
    constexpr auto members =
        std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx));

    T out{};
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        ((out.[:members[I]:] =
              lookup(text, std::meta::identifier_of(members[I]))), ...);
    }(std::make_index_sequence<members.size()>{});
    return out;
}

constexpr config cfg = parse_config<config>(kRaw);

// The file is checked while the program is compiled. Delete a line from the
// config and this assertion fails the build instead of the service failing
// to start.
static_assert(!cfg.host.empty(),    "config is missing host");
static_assert(!cfg.port.empty(),    "config is missing port");
static_assert(!cfg.retries.empty(), "config is missing retries");
static_assert(cfg.port == "8080");

int main() {
    std::printf("host    = %.*s\n", (int)cfg.host.size(), cfg.host.data());
    std::printf("port    = %.*s\n", (int)cfg.port.size(), cfg.port.data());
    std::printf("retries = %.*s\n", (int)cfg.retries.size(), cfg.retries.data());
    std::printf("\nparsed during compilation; the keys came from the struct\n");
    return 0;
}

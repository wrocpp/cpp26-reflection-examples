// reflect_named_captures.cpp
//
// A regex with N captures already describes a record with N fields. C++26
// reflection can enumerate a struct's fields. Joining the two gives a generic
// parse_into<T, pattern>() that fills any aggregate from a match, with no
// per-struct code:
//
//     struct date { std::string_view year, month, day; };
//     auto d = parse_into<date, R"((\d{4})/(\d{1,2})/(\d{1,2}))">("2026/8/22");
//
// Captures are assigned positionally: capture 1 to the first member, capture 2
// to the second, and so on. Addressing them by capture NAME instead would need
// the name as a template argument, and reflection hands identifiers back as
// values, so the positional form is the one that composes cleanly today.
//
// Compile: clang-p2996 fork, -std=c++26 -freflection-latest -stdlib=libc++
// verify: clang-only
// verify: clang-options: -std=c++26 -freflection-latest -stdlib=libc++ -O1

#include <ctre.hpp>
#include <experimental/meta>

#include <cstdio>
#include <optional>
#include <string_view>
#include <utility>

using namespace std::string_view_literals;

template <typename T, ctll::fixed_string Pattern>
constexpr std::optional<T> parse_into(std::string_view s) {
    auto m = ctre::match<Pattern>(s);
    if (!m) { return std::nullopt; }

    constexpr auto ctx = std::meta::access_context::unchecked();
    constexpr auto members =
        std::define_static_array(std::meta::nonstatic_data_members_of(^^T, ctx));

    T out{};
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        // capture I+1 goes to member I; capture 0 is the whole match
        ((out.[:members[I]:] = m.template get<I + 1>().to_view()), ...);
    }(std::make_index_sequence<members.size()>{});
    return out;
}

struct date {
    std::string_view year;
    std::string_view month;
    std::string_view day;
};

// A different shape, same helper, no new code.
struct host_port {
    std::string_view host;
    std::string_view port;
};

int main() {
    if (auto d = parse_into<date, R"((\d{4})/(\d{1,2})/(\d{1,2}))">("2026/8/22"sv)) {
        std::printf("date      : %.*s-%.*s-%.*s\n",
                    (int)d->year.size(), d->year.data(),
                    (int)d->month.size(), d->month.data(),
                    (int)d->day.size(), d->day.data());
    }

    if (auto h = parse_into<host_port, R"(([a-z.]+):(\d+))">("example.com:8080"sv)) {
        std::printf("host_port : host=%.*s port=%.*s\n",
                    (int)h->host.size(), h->host.data(),
                    (int)h->port.size(), h->port.data());
    }

    if (!parse_into<date, R"((\d{4})/(\d{1,2})/(\d{1,2}))">("nonsense"sv)) {
        std::printf("rejected  : nonsense did not match, nullopt returned\n");
    }

    return 0;
}

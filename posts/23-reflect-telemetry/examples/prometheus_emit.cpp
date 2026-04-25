// Post 23 — reflect_telemetry: Prometheus exposition from annotated fields.
// Simplified — counters and gauges only; histograms ship in the real library.
// Compile: ./cpp posts/23-reflect-telemetry/examples/prometheus_emit.cpp \
//                -o posts/23-reflect-telemetry/examples/prometheus_emit
// Run:     ./run posts/23-reflect-telemetry/examples/prometheus_emit

#include <experimental/meta>
#include <atomic>
#include <cstdint>
#include <print>
#include <string>
#include <string_view>

namespace metrics {

struct counter { bool operator==(counter const&) const = default; };
struct gauge   { bool operator==(gauge const&)   const = default; };

struct namespace_tag {
    char const* name;
    bool operator==(namespace_tag const&) const = default;
};
consteval auto namespace_(std::string_view s) -> namespace_tag {
    return {std::define_static_string(s)};
}

struct help_tag {
    char const* text;
    bool operator==(help_tag const&) const = default;
};
consteval auto help(std::string_view s) -> help_tag {
    return {std::define_static_string(s)};
}

template <typename T>
std::string expose_prometheus(T const& obj) {
    std::string out;
    constexpr auto type_ns = std::meta::annotation_of_type<namespace_tag>(^^T);
    constexpr std::string_view ns = type_ns ? type_ns->name : "";
    constexpr auto ctx = std::meta::access_context::unchecked();

    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        constexpr bool is_counter
            = std::meta::annotation_of_type<counter>(m).has_value();
        constexpr bool is_gauge
            = std::meta::annotation_of_type<gauge>(m).has_value();

        if constexpr (is_counter || is_gauge) {
            // Build the metric name: "<namespace>_<field>"
            std::string full_name;
            if (!ns.empty()) { full_name = ns; full_name += '_'; }
            full_name += std::meta::identifier_of(m);

            if constexpr (constexpr auto h
                          = std::meta::annotation_of_type<help_tag>(m)) {
                out += "# HELP "; out += full_name; out += ' ';
                out += h->text;   out += '\n';
            }
            out += "# TYPE "; out += full_name; out += ' ';
            out += is_counter ? "counter" : "gauge";
            out += '\n';
            out += full_name; out += ' ';
            out += std::to_string(obj.[:m:].load());
            out += '\n';
        }
    }
    return out;
}

}  // namespace metrics

struct [[=metrics::namespace_("http")]] HttpStats {
    [[=metrics::counter{}, =metrics::help("Total HTTP requests.")]]
    std::atomic<std::uint64_t> requests_total;

    [[=metrics::gauge{}, =metrics::help("Open connections.")]]
    std::atomic<std::int64_t> open_connections;

    // Not annotated → not emitted
    std::atomic<int> internal_counter;
};

int main() {
    HttpStats stats;
    stats.requests_total.fetch_add(42);
    stats.open_connections.fetch_add(7);
    stats.internal_counter.fetch_add(99);
    std::print("{}", metrics::expose_prometheus(stats));
}

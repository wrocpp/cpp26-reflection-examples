// "Reflection today" demo for profiling-cpp-2026: auto-instrument every
// method of a struct of std::function members.
//
// Real-world setting: tracing frameworks (Perfetto, Tracy, OpenTelemetry,
// chrome-trace) require manually wrapping every hot-path entry/exit
// with TRACE_EVENT or ZoneScoped macros. The wrapping is mechanical
// and you forget to add it where it actually matters -- the new method
// added last sprint that's now eating 40% of latency. Reflection lets
// the wrapper generate itself from the interface shape.
//
// Pattern: a struct of std::function members marked [[=trace]] gets
// auto-instrumented at construction. Each call records (function name,
// timestamp) into a global span log. Replace the println below with
// Perfetto's TRACE_EVENT_BEGIN / TRACE_EVENT_END or Tracy's
// ZoneScopedN(name) in production -- the reflection harness is the
// same, only the sink changes.
//
// Aligned with Core Guidelines T (Testing) -- treat tracing as
// observability infrastructure that must follow the schema. New method
// in the interface -> trace point added automatically.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/tr && /tmp/tr" \
//     posts/toolset/profiling-cpp-2026/examples/reflect-tracing.cpp

#include <experimental/meta>
#include <chrono>
#include <functional>
#include <print>
#include <string>
#include <string_view>
#include <vector>

// Simple in-memory span log; in production this is Perfetto's track
// buffer, Tracy's queue, or OpenTelemetry's exporter. The reflection
// harness below is the only piece that's reflection-specific.
struct Span {
    std::string name;
    std::chrono::nanoseconds duration;
};
inline std::vector<Span>& span_log() {
    static std::vector<Span> log;
    return log;
}

// User-defined annotation tag (P3394). Mark fields you want auto-traced.
struct trace {};

template <typename Tag>
consteval bool has_annotation(std::meta::info r) {
    for (auto a : std::meta::annotations_of(r))
        if (std::meta::type_of(a) == ^^Tag) return true;
    return false;
}

// Wrap an interface T: for each member with [[=trace]] annotation,
// replace the std::function with a timing wrapper. Reflection walks
// the members at compile time; the runtime wrapper just records the
// elapsed time around each call.
template <typename T>
auto instrument(T iface) -> T {
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr auto m
                  : std::define_static_array(
                      std::meta::nonstatic_data_members_of(^^T, ctx))) {
        if constexpr (has_annotation<trace>(m)) {
            std::string name(std::meta::identifier_of(m));
            auto inner = iface.[:m:];
            iface.[:m:] = [inner = std::move(inner), name = std::move(name)]
                          (auto&&... args) {
                auto t0 = std::chrono::steady_clock::now();
                if constexpr (std::is_void_v<decltype(inner(args...))>) {
                    inner(std::forward<decltype(args)>(args)...);
                    auto t1 = std::chrono::steady_clock::now();
                    span_log().push_back({name, t1 - t0});
                } else {
                    auto r = inner(std::forward<decltype(args)>(args)...);
                    auto t1 = std::chrono::steady_clock::now();
                    span_log().push_back({name, t1 - t0});
                    return r;
                }
            };
        }
    }
    return iface;
}

// --- Subject interface --------------------------------------------------

struct Renderer {
    [[=trace{}]] std::function<void(std::string_view)> draw;
    [[=trace{}]] std::function<void(int)>              flush;
                 std::function<void()>                 invisible;  // no trace
};

int main() {
    auto r = instrument(Renderer{
        .draw     = [](std::string_view) { /* expensive paint */ },
        .flush    = [](int) { /* expensive sync */ },
        .invisible = [] {},
    });

    r.draw("frame 1");
    r.flush(60);
    r.draw("frame 2");
    r.invisible();          // not traced -- no annotation

    std::println("== span log ({} entries) ==", span_log().size());
    for (auto const& s : span_log()) {
        std::println("  {:14}  {} ns",
                     s.name, s.duration.count());
    }
}

// Post 24 — reflect_tracing: four instrumentation forms, side by side.
//
// The four forms (chosen per-function in the demo):
//
//   Form 1  scope_guard<^^Fn>                 reflection-native, explicit name,
//                                             function-address payload.
//   Form 2  scope_guard_named<_fn> (+ 1 line) compile-time NTTP name via
//                                             std::source_location.current().
//   Form 3  AUTO_SCOPE() macro                same as Form 2, wrapped in one line.
//   Form 4  TRACE() macro                     Maciek Gajewski's 2021 technique:
//                                             labels-as-values (&&label) captures
//                                             a per-call-site address with no name.
//
// All four populate the same ring buffer. Address-based forms (1, 4) store
// a function/label address; name-based forms (2, 3) store a pointer to a
// compile-time string. A per-key registry resolves display names at dump time
// (standing in for DWARF — dwarf++ / addr2line in the real library).
//
// Compile: ./cpp posts/24-reflect-tracing/examples/trace_emit.cpp \
//                -o posts/24-reflect-tracing/examples/trace_emit
// Run:     ./run posts/24-reflect-tracing/examples/trace_emit

#include <experimental/meta>
#include <array>
#include <chrono>
#include <cstdint>
#include <print>
#include <source_location>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef REFLECT_TRACING_DISABLED
#define REFLECT_TRACING_DISABLED 0
#endif

namespace tracing {

// A key is either a function address or a pointer to a static name string.
// Both are 8 bytes; the registry resolves either flavour to a display name.
struct span_record {
    void const* key;
    std::uint64_t enter_ns;
    std::uint64_t exit_ns;
};

inline thread_local bool enabled = true;

inline constexpr std::size_t ring_size = 1024;
inline thread_local std::array<span_record, ring_size> ring{};
inline thread_local std::size_t ring_head = 0;

inline std::uint64_t now_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

struct reg_entry { void const* key; char const* name; };
inline std::vector<reg_entry>& registry() {
    static std::vector<reg_entry> r;
    return r;
}

inline void register_once(void const* key, char const* name) {
    for (auto const& e : registry()) if (e.key == key) return;
    registry().push_back({key, name});
}

inline void push_span(void const* key, std::uint64_t enter_ns) {
    ring[ring_head % ring_size] = {key, enter_ns, now_ns()};
    ++ring_head;
}

// ----- Form 1: reflection-native, explicit function reflection -----

#if REFLECT_TRACING_DISABLED
template <std::meta::info Fn>
struct scope_guard { scope_guard() noexcept = default; };
#else
template <std::meta::info Fn>
struct scope_guard {
    static inline bool const registered_ = []{
        register_once(
            reinterpret_cast<void const*>(&[:Fn:]),
            std::define_static_string(std::meta::identifier_of(Fn)));
        return true;
    }();
    void const* key_;
    std::uint64_t enter_ns_ = 0;
    scope_guard() noexcept : key_(reinterpret_cast<void const*>(&[:Fn:])) {
        (void)registered_;
        if (!enabled) return;
        enter_ns_ = now_ns();
    }
    ~scope_guard() noexcept {
        if (enter_ns_ == 0) return;
        push_span(key_, enter_ns_);
    }
};
#endif

// ----- Form 2: name as compile-time NTTP -----

#if REFLECT_TRACING_DISABLED
template <char const* Name>
struct scope_guard_named { scope_guard_named() noexcept = default; };
#else
template <char const* Name>
struct scope_guard_named {
    static inline bool const registered_ = []{
        register_once(Name, Name);
        return true;
    }();
    std::uint64_t enter_ns_ = 0;
    scope_guard_named() noexcept {
        (void)registered_;
        if (!enabled) return;
        enter_ns_ = now_ns();
    }
    ~scope_guard_named() noexcept {
        if (enter_ns_ == 0) return;
        push_span(Name, enter_ns_);
    }
};
#endif

// ----- Form 3: AUTO_SCOPE() macro — wraps Form 2 -----

#if REFLECT_TRACING_DISABLED
#define AUTO_SCOPE() do {} while (false)
#else
#define REFLECT_CONCAT2(a, b) a##b
#define REFLECT_CONCAT(a, b) REFLECT_CONCAT2(a, b)
// std::define_static_string promotes a string literal (not a valid NTTP
// on its own) into compile-time-stable storage that IS a valid NTTP.
#define AUTO_SCOPE()                                                          \
    static constexpr char const* REFLECT_CONCAT(_auto_scope_fn_, __LINE__) =  \
        std::define_static_string(                                            \
            std::source_location::current().function_name());                 \
    ::tracing::scope_guard_named<REFLECT_CONCAT(_auto_scope_fn_, __LINE__)>   \
        REFLECT_CONCAT(_auto_scope_g_, __LINE__)
#endif

// ----- Form 4: TRACE() macro — Maciek's labels-as-values -----

#if REFLECT_TRACING_DISABLED
#define TRACE() do {} while (false)
#else
struct trace_addr_scope {
    void const* key_;
    std::uint64_t enter_ns_ = 0;
    trace_addr_scope(void const* addr, char const* display_name) noexcept
      : key_(addr) {
        register_once(addr, display_name);
        if (!enabled) return;
        enter_ns_ = now_ns();
    }
    ~trace_addr_scope() noexcept {
        if (enter_ns_ == 0) return;
        push_span(key_, enter_ns_);
    }
};
#define TRACE()                                                               \
    REFLECT_CONCAT(_trace_addr_, __LINE__):                                   \
    ::tracing::trace_addr_scope REFLECT_CONCAT(_trace_g_, __LINE__){          \
        &&REFLECT_CONCAT(_trace_addr_, __LINE__),                             \
        std::source_location::current().function_name()                       \
    }
#endif

// ----- dump -----

inline std::string as_chrome_trace() {
    std::unordered_map<void const*, char const*> name_map;
    for (auto const& e : registry()) name_map[e.key] = e.name;

    std::string out = R"({"traceEvents":[)";
    bool first = true;
    std::size_t n = std::min(ring_head, ring_size);
    for (std::size_t i = 0; i < n; ++i) {
        auto const& r = ring[i];
        if (!first) out += ',';
        first = false;
        auto it = name_map.find(r.key);
        out += R"({"name":")";
        out += it != name_map.end() ? it->second : "?";
        out += R"(","ph":"X","ts":)";
        out += std::to_string(r.enter_ns / 1000);
        out += R"(,"dur":)";
        out += std::to_string((r.exit_ns - r.enter_ns) / 1000);
        out += R"(,"pid":1,"tid":0})";
    }
    out += "]}";
    return out;
}

}  // namespace tracing

// ---- user functions, one per form ---------------------------------------

void with_reflection() {
    tracing::scope_guard<^^with_reflection> _g;
    // work...
}

void with_nttp_named() {
    // define_static_string lifts the pointer from a string literal (not a
    // valid NTTP) into proper compile-time-stable storage.
    static constexpr char const* _fn = std::define_static_string(
        std::source_location::current().function_name());
    tracing::scope_guard_named<_fn> _g;
    // work...
}

void with_auto_scope() {
    AUTO_SCOPE();
    // work...
}

void with_trace_macro() {
    TRACE();
    // work...
}

int main() {
    // All four forms enabled
    for (int i = 0; i < 2; ++i) {
        with_reflection();
        with_nttp_named();
        with_auto_scope();
        with_trace_macro();
    }

    // Runtime disable — these invocations record nothing
    tracing::enabled = false;
    with_reflection();
    with_nttp_named();
    with_auto_scope();
    with_trace_macro();
    tracing::enabled = true;

    std::println("{}", tracing::as_chrome_trace());
}

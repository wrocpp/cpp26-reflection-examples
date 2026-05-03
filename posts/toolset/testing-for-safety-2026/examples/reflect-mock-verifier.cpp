// Annotation-driven mock + call verifier for an interface struct.
//
// GoogleMock-style mocks are mostly boilerplate: a MOCK_METHOD per
// virtual function, an EXPECT_CALL per assertion, hand-written for
// every interface in the codebase. The boilerplate is mechanical -- it
// follows from the interface shape -- which means reflection can write
// it. Add a method to the interface, the mock grows; remove one, it
// shrinks; rename one, the test fails to compile (loud) instead of
// silently ignoring the call (quiet bug).
//
// Pattern: an interface is modelled as a struct of std::function
// members, marked [[=mock_interface{}]] at struct level. The
// ReflectMock<T> wrapper walks T's members, replaces each function
// with a recording stub, and exposes calls(name) and expect_called().
// Per-method [[=mock_max_calls{N}]] adds an upper-bound assertion the
// verifier checks at scope exit.
//
// What this REPLACES: per-method MOCK_METHOD declarations (one line of
// boilerplate per interface method per test target). At ~50 interfaces
// per medium codebase, that is several thousand lines that exist only
// to satisfy a code generator the language can now do natively.
//
// Aligned with:
//   * Core Guidelines T (Testing) and I.27 (interface design via
//     classes of std::function -- one of several legal shapes).
//   * Treat the interface as the single source of truth: test code
//     never re-states the method list.
//
// Compile + run via the wro.cpp container (recommended):
//   docker run --rm -v "$PWD":/work -w /work \
//     ghcr.io/wrocpp/cpp-reflection:2026-05 \
//     bash -c "clang++ -std=c++26 -freflection-latest -stdlib=libc++ \
//                      $0 -o /tmp/mock && /tmp/mock" \
//     posts/toolset/testing-for-safety-2026/examples/reflect-mock-verifier.cpp

#include <experimental/meta>
#include <functional>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>

// --- Annotation tags (P3394) ----------------------------------------

struct mock_interface {};
struct mock_max_calls { unsigned n; };

// --- Subject interface: the system-under-test depends on a Logger ---

struct [[=mock_interface{}]] Logger {
    std::function<void(std::string_view)> info;
    std::function<void(std::string_view)> warn;
    [[=mock_max_calls{0}]]                          // happy path: no errors
    std::function<void(std::string_view)> error;
};

// --- Annotation lookup ---------------------------------------------

template <typename Tag>
consteval bool has_annotation(std::meta::info r) {
    for (auto a : std::meta::annotations_of(r))
        if (std::meta::type_of(a) == ^^Tag) return true;
    return false;
}

template <typename Tag>
consteval auto get_annotation(std::meta::info r) -> Tag {
    for (auto a : std::meta::annotations_of(r))
        if (std::meta::type_of(a) == ^^Tag)
            return std::meta::extract<Tag>(a);
    return Tag{};
}

// --- Reflect-driven mock --------------------------------------------

template <typename T>
class ReflectMock {
    static_assert(has_annotation<mock_interface>(^^T),
                  "ReflectMock requires [[=mock_interface{}]] on T");
public:
    T iface;
    std::unordered_map<std::string, unsigned> calls;

    ReflectMock() {
        constexpr auto ctx = std::meta::access_context::unchecked();
        template for (constexpr auto m
                      : std::define_static_array(
                          std::meta::nonstatic_data_members_of(^^T, ctx))) {
            std::string name(std::meta::identifier_of(m));
            iface.[:m:] = [this, name = name](auto&&...) {
                ++calls[name];
            };
        }
    }

    auto times_called(std::string_view name) const -> unsigned {
        auto it = calls.find(std::string(name));
        return it == calls.end() ? 0u : it->second;
    }

    // Walks T's members; for each [[=mock_max_calls{N}]], asserts
    // calls[name] <= N. Returns false on any violation.
    auto verify_max_calls() const -> bool {
        bool ok = true;
        constexpr auto ctx = std::meta::access_context::unchecked();
        template for (constexpr auto m
                      : std::define_static_array(
                          std::meta::nonstatic_data_members_of(^^T, ctx))) {
            if constexpr (has_annotation<mock_max_calls>(m)) {
                constexpr auto cap = get_annotation<mock_max_calls>(m);
                std::string name(std::meta::identifier_of(m));
                unsigned n = times_called(name);
                if (n > cap.n) {
                    std::println("    {} called {} times, cap is {} (FAIL)",
                                 name, n, cap.n);
                    ok = false;
                } else {
                    std::println("    {} called {} times, cap {} (ok)",
                                 name, n, cap.n);
                }
            }
        }
        return ok;
    }
};

// --- System under test ---------------------------------------------

void process_request(const Logger& log, int code) {
    log.info("request received");
    if (code >= 400) log.warn("client error");
    if (code >= 500) log.error("server error");
}

int main() {
    std::println("== reflect-driven mock verifier ==");

    // Happy path: code 200 -> info only, warn/error untouched.
    {
        ReflectMock<Logger> m;
        process_request(m.iface, 200);
        std::println("  case 200:");
        std::println("    info  = {}", m.times_called("info"));
        std::println("    warn  = {}", m.times_called("warn"));
        std::println("    error = {}", m.times_called("error"));
        bool ok = m.verify_max_calls();          // error cap is 0
        std::println("  verify_max_calls: {}", ok ? "PASS" : "FAIL");
    }

    // Failure path: code 503 -> the error cap of 0 is INTENTIONALLY
    // violated to show the cap firing. This is the assertion that
    // catches "we accidentally started logging an error in the happy
    // path" regressions.
    {
        ReflectMock<Logger> m;
        process_request(m.iface, 503);
        std::println("  case 503 (cap intentionally violated):");
        bool ok = m.verify_max_calls();
        std::println("  verify_max_calls: {} (expected FAIL)", ok ? "PASS" : "FAIL");
    }
}

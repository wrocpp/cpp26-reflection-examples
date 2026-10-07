// rqt: namespace test discovery by reflection. A test is a function in a namespace annotated [[=rqt::test]].
// Fixtures are resolved per PARAMETER from a fixtures namespace: by parameter NAME first, else by TYPE.
// [[=rqt::param(v)]] on a parameter parameterises the test (one case per annotation, cartesian product).
#pragma once
#include <meta>
#include <algorithm>
#include <cstdio>
#include <exception>
#include <source_location>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace rqt {

struct test_t {};
inline constexpr test_t test{};
struct xfail_t {};
inline constexpr xfail_t xfail{};  // the test is expected to fail
struct param {
    int value;
    consteval param(int v) : value(v) {}
};

struct failure : std::exception {
    std::string msg;
    explicit failure(std::string m) : msg(std::move(m)) {}
    const char* what() const noexcept override { return msg.c_str(); }
};
inline void expect(bool ok, std::string_view what = "expectation failed") {
    if (!ok) throw failure(std::string(what));
}

namespace detail {
using std::meta::info;
inline constexpr info param_type = ^^param;  // a named constant: `^^param && x` would parse as the type `param&&`

consteval bool has_annotation_of(info item, info type) {
    for (info a : std::meta::annotations_of(item)) {
        if (std::meta::remove_cvref(std::meta::type_of(a)) == type) return true;
    }
    return false;
}

consteval std::size_t param_count_of(info p) {
    std::size_t n = 0;
    for (info a : std::meta::annotations_of(p)) {
        if (std::meta::remove_cvref(std::meta::type_of(a)) == param_type) ++n;
    }
    return n;
}
consteval int param_value_of(info p, std::size_t k) {
    for (info a : std::meta::annotations_of(p)) {
        if (std::meta::remove_cvref(std::meta::type_of(a)) == param_type && k-- == 0) return std::meta::extract<param>(a).value;
    }
    return 0;
}
// number of cases of the parameters before p (the cartesian-product stride of p)
consteval std::size_t stride_before(info f, info p) {
    std::size_t s = 1;
    for (info q : std::meta::parameters_of(f)) {
        if (q == p) break;
        if (std::size_t n = param_count_of(q)) s *= n;
    }
    return s;
}
consteval std::size_t cases_of(info f) {
    std::size_t s = 1;
    for (info q : std::meta::parameters_of(f)) {
        if (std::size_t n = param_count_of(q)) s *= n;
    }
    return s;
}
consteval int value_for(info f, info p, std::size_t case_index) {
    return param_value_of(p, (case_index / stride_before(f, p)) % param_count_of(p));
}

template <info Ns>
consteval std::vector<info> test_functions() {
    std::vector<info> out;
    for (info m : std::meta::members_of(Ns, std::meta::access_context::unchecked())) {
        if (std::meta::is_function(m) && has_annotation_of(m, ^^test_t)) out.push_back(m);
    }
    return out;
}

enum class fixture_status { found, none, ambiguous };
struct fixture_pick {
    info fn;
    fixture_status status;
};
// by NAME (identifier of the parameter == identifier of a fixture function returning the type), else unique by TYPE
consteval fixture_pick pick_fixture(info fixtures_ns, info p) {
    info want = std::meta::remove_cvref(std::meta::type_of(p));
    info by_type{};
    std::size_t type_hits = 0;
    for (info m : std::meta::members_of(fixtures_ns, std::meta::access_context::unchecked())) {
        if (!std::meta::is_function(m) || !std::meta::has_identifier(m)) continue;
        if (std::meta::return_type_of(m) != want) continue;
        if (std::meta::has_identifier(p) && std::meta::identifier_of(p) == std::meta::identifier_of(m)) return {m, fixture_status::found};
        by_type = m;
        ++type_hits;
    }
    if (type_hits == 1) return {by_type, fixture_status::found};
    return {info{}, type_hits == 0 ? fixture_status::none : fixture_status::ambiguous};
}

template <info F, std::size_t I, std::size_t Case, info Fix>
auto make_arg() {
    constexpr info p = std::meta::parameters_of(F)[I];
    using T = std::remove_cvref_t<typename [:std::meta::type_of(p):]>;
    if constexpr (param_count_of(p) > 0) {
        return T(value_for(F, p, Case));
    } else {
        constexpr fixture_pick pick = pick_fixture(Fix, p);
        static_assert(pick.status != fixture_status::none, "rqt: no fixture function returns this parameter's type");
        static_assert(pick.status != fixture_status::ambiguous,
                      "rqt: several fixture functions return this parameter's type and none is named like the parameter");
        return T([:pick.fn:]());
    }
}

template <info F, std::size_t Case, info Fix>
void run_case() {
    constexpr std::size_t N = std::meta::parameters_of(F).size();
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        std::tuple<std::remove_cvref_t<typename [:std::meta::type_of(std::meta::parameters_of(F)[I]):]>...> args{
            make_arg<F, I, Case, Fix>()...};
        [:F:](std::get<I>(args)...);
    }(std::make_index_sequence<N>{});
}

template <info F, std::size_t Case>
std::string label() {
    std::string s{std::meta::identifier_of(F)};
    bool first = true;
    template for (constexpr info p : std::define_static_array(std::meta::parameters_of(F))) {
        if constexpr (param_count_of(p) > 0) {
            s += first ? "[" : ",";
            first = false;
            s += std::meta::identifier_of(p);
            s += "=" + std::to_string(value_for(F, p, Case));
        }
    }
    if (!first) s += "]";
    return s;
}
}  // namespace detail

struct case_info {
    std::string name;
    void (*run)();
    bool expect_fail;
};

namespace detail {
template <info F, info Fix>
void add_cases(std::vector<case_info>& out) {
    [&]<std::size_t... C>(std::index_sequence<C...>) {
        (out.push_back(case_info{label<F, C>(), &run_case<F, C, Fix>, has_annotation_of(F, ^^xfail_t)}), ...);
    }(std::make_index_sequence<cases_of(F)>{});
}
// Tag makes this instantiation (which enumerates the namespace as THIS translation unit sees it) unique per TU.
template <info TestNs, info Fix, class Tag>
std::vector<case_info> collect() {
    std::vector<case_info> out;
    template for (constexpr info f : std::define_static_array(test_functions<TestNs>())) {
        add_cases<f, Fix>(out);
    }
    return out;
}
}  // namespace detail

inline std::vector<case_info>& registry() {
    static std::vector<case_info> r;  // function-local: immune to static-init order
    return r;
}

// Single-TU use: walk the namespace the way THIS translation unit sees it. Tag defaults to a per-call-site lambda.
template <std::meta::info TestNs, std::meta::info Fix, class Tag = decltype([] {})>
std::vector<case_info> discover() {
    return detail::collect<TestNs, Fix, Tag>();
}

// Cross-TU: a namespace-scope object whose constructor feeds the global registry.
// The constructor is a template over a lambda Tag so that each TU gets its OWN instantiation.
template <std::meta::info TestNs, std::meta::info Fix>
struct registrar {
    template <class Tag>
    explicit registrar(Tag) {
        for (auto& c : detail::collect<TestNs, Fix, Tag>()) registry().push_back(std::move(c));
    }
};

// Run, print one line per case. Returns the number of failures.
inline int run(std::vector<case_info> cases) {
    std::ranges::sort(cases, {}, &case_info::name);
    int failures = 0;
    for (auto& c : cases) {
        try {
            c.run();
            if (c.expect_fail) { std::printf("FAIL   %s (expected to fail, passed)\n", c.name.c_str()); ++failures; }
            else std::printf("pass   %s\n", c.name.c_str());
        } catch (const failure& e) {
            if (c.expect_fail) std::printf("xfail  %s (%s)\n", c.name.c_str(), e.what());
            else { std::printf("FAIL   %s: %s\n", c.name.c_str(), e.what()); ++failures; }
        } catch (const std::exception& e) {
            std::printf("FAIL   %s: exception %s\n", c.name.c_str(), e.what());
            ++failures;
        }
    }
    std::printf("%zu cases, %d failed\n", cases.size(), failures);
    return failures;
}
}  // namespace rqt

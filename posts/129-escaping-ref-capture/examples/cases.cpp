// verify: gcc-only
// A lambda that captures by reference and outlives its frame: nine cases that dangle, five that do not.
//
// Build: g++ -std=c++23 -Wall -Wextra cases.cpp -o cases      (GCC 16.2; clang 22 and later also build it)
// Run:   ./cases        runs the five safe cases only, prints one line, exits 0
//        ./cases N      runs dangling case N (1 to 9) once: the escaped lambda is called after its frame
//                       ended. That is undefined behaviour: use it under -fsanitize=address.
// Limits: the captured string is longer than any small-string buffer, so its bytes live on the heap
// and a sanitizer can see them freed. Nothing here is a benchmark.

#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

constexpr std::string_view kText = "a text long enough to leave the small-string buffer";
constexpr std::size_t kDanglingCaseCount = 9;
constexpr int kArgIndex = 1;
constexpr int kArgsWithCase = 2;

using Handler = std::function<int()>;

struct Registry {
    std::vector<Handler> handlers;
    void add(Handler h) { handlers.push_back(std::move(h)); }
    int call_first() const { return handlers.front()(); }
};

template <class F>
struct Holder {
    F f;
};

int first_char(const std::string& s) { return static_cast<int>(s.front()); }

// Dangling cases 1 to 3: the lambda is returned, with an auto return type or through a std::function.
auto make_explicit() {
    std::string label{kText};
    return [&label] { return first_char(label); };
}

Handler make_default() {
    std::string label{kText};
    return [&] { return first_char(label); };
}

Handler make_init_capture() {
    std::string label{kText};
    return [&ref = label] { return first_char(ref); };
}

// Dangling cases 4 to 6: the lambda is stored in a registry that outlives the frame.
void fill_stored_variable(Registry& registry) {
    std::string label{kText};
    Handler h = [&label] { return first_char(label); };
    registry.add(h);
}

void fill_direct_argument(Registry& registry) {
    std::string label{kText};
    registry.add([&label] { return first_char(label); });
}

void fill_push_back(Registry& registry) {
    std::string label{kText};
    registry.handlers.push_back([&] { return first_char(label); });
}

// Dangling cases 7 to 9: the lambda escapes through a path the check does not follow.
auto make_named_variable() {
    std::string label{kText};
    auto f = [&label] { return first_char(label); };
    return f;
}

void fill_from_variable(Registry& registry) {
    std::string label{kText};
    auto f = [&label] { return first_char(label); };
    registry.add(f);
}

auto make_holder() {
    std::string label{kText};
    return Holder{[&label] { return first_char(label); }};
}

int run_explicit() { return make_explicit()(); }
int run_default() { return make_default()(); }
int run_init_capture() { return make_init_capture()(); }
int run_stored_variable() {
    Registry registry;
    fill_stored_variable(registry);
    return registry.call_first();
}
int run_direct_argument() {
    Registry registry;
    fill_direct_argument(registry);
    return registry.call_first();
}
int run_push_back() {
    Registry registry;
    fill_push_back(registry);
    return registry.call_first();
}
int run_named_variable() { return make_named_variable()(); }
int run_from_variable() {
    Registry registry;
    fill_from_variable(registry);
    return registry.call_first();
}
int run_holder() { return make_holder().f(); }

using Case = int (*)();
constexpr std::array<Case, kDanglingCaseCount> kDangling{
    run_explicit,        run_default,   run_init_capture,   run_stored_variable, run_direct_argument,
    run_push_back,       run_named_variable, run_from_variable, run_holder,
};

// Safe cases. The first two are not reported. The last three are correct programs that the check still reports:
// the lambda is converted to a std::function but never outlives its frame.
Handler safe_by_value() {
    std::string label{kText};
    return [label] { return first_char(label); };
}

int safe_algorithm() {
    const std::vector<int> values{1, 2, 3};
    int total = 0;
    for (const int v : values) {
        const auto add = [&total](int x) { total += x; };
        add(v);
    }
    return total;
}

int safe_this_owned() {
    struct Counter {
        int count = 1;
        std::function<int()> bump = [&] { return ++count; };
    };
    Counter counter;
    return counter.bump();
}

int safe_synchronous_function() {
    std::string label{kText};
    const Handler h = [&label] { return first_char(label); };
    return h();
}

int apply(const std::function<int()>& f) { return f(); }

int safe_synchronous_argument() {
    std::string label{kText};
    return apply([&label] { return first_char(label); });
}

int main(int argc, char** argv) {
    if (argc == kArgsWithCase) {
        const auto n = static_cast<std::size_t>(std::atoi(argv[kArgIndex]));
        if (n < 1 || n > kDanglingCaseCount) return 1;
        std::printf("case %zu read '%c'\n", n, kDangling[n - 1]());
        return 0;
    }
    const std::array<int, 5> results{safe_by_value()(), safe_algorithm(), safe_this_owned(), safe_synchronous_function(),
                                      safe_synchronous_argument()};
    std::size_t ran = 0;
    for (const int r : results) {
        if (r != 0) ++ran;
    }
    std::printf("%zu safe cases ran\n", ran);
    return 0;
}

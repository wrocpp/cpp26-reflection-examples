// verify: gcc-only
// Build: g++ -std=c++26 -Wall -Wextra -Wpedantic folds_and_template_for.cpp
// Prints: "left=5 right=9 third=42 links_for=21 links_seq=21 links_binding=21 sum_for=12 sum_binding=12"
// Limits: GCC 16.2 only. Clang 22.1 has no `template for`. The forms that must
// not compile (a temporary as the range of a constexpr `template for`, a
// structured binding pack outside a template) are quoted in the README.
#include <cstddef>
#include <print>
#include <tuple>
#include <utility>

constexpr int minuend = 10;
constexpr int first_subtrahend = 3;
constexpr int second_subtrahend = 2;
constexpr int left_expected = 5;
constexpr int right_expected = 9;
constexpr int third_index_value = 42;
constexpr std::size_t sum_expected = 12;
constexpr int links_expected = 21;

// 1. Left and right folds group differently.
template <class... T> constexpr auto left(T... v) { return (... - v); }
template <class... T> constexpr auto right(T... v) { return (v - ...); }
static_assert(left(minuend, first_subtrahend, second_subtrahend) == left_expected);
static_assert(right(minuend, first_subtrahend, second_subtrahend) == right_expected);

// 2. Pack indexing, C++26.
template <class... T> constexpr auto third(T... args) { return args...[2]; }
static_assert(third(1, 2, third_index_value) == third_index_value);

// 3. A wiring table that keeps member pointers, as in the outside post.
struct Step {
    void failed() {}
    void timedOut() {}
    void succeeded() {}
};
struct Procedure {
    void onStepFailed() {}
    void onStepSucceeded() {}
    static constexpr std::tuple wiring{std::pair{&Step::failed, &Procedure::onStepFailed},
                                       std::pair{&Step::timedOut, &Procedure::onStepFailed},
                                       std::pair{&Step::succeeded, &Procedure::onStepSucceeded}};
};
template <auto Signal, auto Slot> constexpr int link_id = 7;

// 3a. `template for` over a static constexpr variable: elements are constants.
constexpr int count_links_for() {
    int links = 0;
    template for (constexpr auto p : Procedure::wiring) {
        links += link_id<p.first, p.second>;
    }
    return links;
}

// 3b. The index_sequence workaround.
template <class Table> constexpr int count_links_seq() {
    return []<std::size_t... I>(std::index_sequence<I...>) {
        return (link_id<std::get<I>(Table::wiring).first, std::get<I>(Table::wiring).second> + ...);
    }(std::make_index_sequence<std::tuple_size_v<decltype(Table::wiring)>>{});
}

// 3c. A structured binding pack, only inside a template.
template <class Table> constexpr int count_links_binding() {
    static constexpr auto [...rows] = Table::wiring;
    return (link_id<rows.first, rows.second> + ...);
}

template <std::size_t I> constexpr std::size_t twice = 2 * I;

constexpr std::size_t sum_for() {
    static constexpr std::tuple values{1uz, 2uz, 3uz};
    std::size_t sum = 0;
    template for (constexpr auto v : values) {
        sum += twice<v>;
    }
    return sum;
}

struct Values {
    static constexpr std::tuple row{1uz, 2uz, 3uz};
};
template <class Table> constexpr std::size_t sum_binding() {
    static constexpr auto [...vs] = Table::row;
    return (twice<vs> + ...);
}

static_assert(count_links_for() == links_expected);
static_assert(count_links_seq<Procedure>() == links_expected);
static_assert(count_links_binding<Procedure>() == links_expected);
static_assert(sum_for() == sum_expected);
static_assert(sum_binding<Values>() == sum_expected);

int main() {
    std::println("left={} right={} third={} links_for={} links_seq={} links_binding={} sum_for={} sum_binding={}",
                 left(minuend, first_subtrahend, second_subtrahend),
                 right(minuend, first_subtrahend, second_subtrahend),
                 third(1, 2, third_index_value), count_links_for(), count_links_seq<Procedure>(),
                 count_links_binding<Procedure>(),
                 sum_for(), sum_binding<Values>());
}

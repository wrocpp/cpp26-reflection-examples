// verify: gcc-only
// Shapes of a local shared_ptr, for the wrocpp-needless-shared-ptr check of tidy-agent: three
// that the check flags, nine that it leaves alone, and three that show its limits. Every shape is
// its own function so clang-tidy reports one line per case.
//
// Build: g++ -std=c++23 -Wall -Wextra cases.cpp -o cases      (GCC 16.2; clang 22 and later also build it)
// Run:   ./cases   calls every case once, prints one line, exits 0
// Limits: the check is a clang-tidy plugin. It cannot run on Compiler Explorer, so its output is
// quoted in the README from a local run. Nothing here is a benchmark.

#include <cstdio>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

constexpr int kInitial = 3;
constexpr int kCaseCount = 15;

struct Widget {
    int value = kInitial;
    int get() const { return value; }
};

void take_by_value(std::shared_ptr<Widget> w) { (void)w; }
void take_by_const_ref(const std::shared_ptr<Widget>& w) { (void)w; }

// ---- Flagged: the pointer is only dereferenced, used with get(), or tested.

int flagged_arrow() {
    auto w = std::make_shared<Widget>();
    w->value = kInitial;
    return w->get() + (w ? 1 : 0);
}

int flagged_star_and_get() {
    auto w = std::make_shared<Widget>();
    const Widget* raw = w.get();
    return (*w).value + raw->value;
}

// The declared type names shared_ptr, so the check reports it but offers no fix-it.
int flagged_explicit_type() {
    std::shared_ptr<Widget> w = std::make_shared<Widget>();
    return w->get();
}

// ---- Not flagged: the pointer is shared, or might be.

std::shared_ptr<Widget> kept_returned() {
    auto w = std::make_shared<Widget>();
    return w;
}

void kept_stored(std::vector<std::shared_ptr<Widget>>& all) {
    auto w = std::make_shared<Widget>();
    all.push_back(w);
}

std::function<int()> kept_captured() {
    auto w = std::make_shared<Widget>();
    return [w] { return w->get(); };
}

void kept_passed_by_value() {
    auto w = std::make_shared<Widget>();
    take_by_value(w);
}

void kept_passed_by_const_ref() {
    auto w = std::make_shared<Widget>();
    take_by_const_ref(w);
}

std::weak_ptr<Widget> kept_weak() {
    auto w = std::make_shared<Widget>();
    return w;
}

void kept_moved(std::vector<std::shared_ptr<Widget>>& all) {
    auto w = std::make_shared<Widget>();
    all.push_back(std::move(w));
}

int kept_alias() {
    auto w = std::make_shared<Widget>();
    const auto& alias = w;
    return alias->get();
}

int kept_use_count() {
    auto w = std::make_shared<Widget>();
    return static_cast<int>(w.use_count());
}

// ---- Limits: not flagged although the pointer is never shared.

int missed_null_compare() {
    auto w = std::make_shared<Widget>();
    return w == nullptr ? 0 : w->get();
}

int missed_new_expression() {
    std::shared_ptr<Widget> w{new Widget{}};
    return w->get();
}

int missed_allocate_shared() {
    auto w = std::allocate_shared<Widget>(std::allocator<Widget>{});
    return w->get();
}

int main() {
    std::vector<std::shared_ptr<Widget>> sink;
    const int total = flagged_arrow() + flagged_star_and_get() + flagged_explicit_type() +
                      kept_returned()->get() + kept_captured()() + kept_alias() + kept_use_count() +
                      missed_null_compare() + missed_new_expression() + missed_allocate_shared();
    kept_stored(sink);
    kept_passed_by_value();
    kept_passed_by_const_ref();
    kept_moved(sink);
    (void)kept_weak();
    std::printf("%d cases ran, %zu pointers stored, total %d\n", kCaseCount, sink.size(), total);
    return 0;
}

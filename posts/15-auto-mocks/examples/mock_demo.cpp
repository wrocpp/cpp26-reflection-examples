// Post 15 — Auto-generated mocks from interfaces.
// Simpler demo that doesn't require define_aggregate: a generic mock wrapper
// that builds a dispatch table from member-function reflection. For production-
// grade class synthesis, see the define_aggregate notes in post 15's prose.
// Compile: ./cpp posts/15-auto-mocks/examples/mock_demo.cpp \
//                -o posts/15-auto-mocks/examples/mock_demo
// Run:     ./run posts/15-auto-mocks/examples/mock_demo

#include <experimental/meta>
#include <functional>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>

namespace mocks {

// Trivial call-tracker: maps method reflection → count + last arg.
class tracker {
    std::unordered_map<std::size_t, std::size_t> counts_;
public:
    void record(std::size_t key) { ++counts_[key]; }
    std::size_t count(std::size_t key) const {
        auto it = counts_.find(key); return it == counts_.end() ? 0 : it->second;
    }
};

// Stable key for a function reflection: hash the full display string.
consteval std::size_t method_key(std::meta::info fn) {
    // Simple FNV-1a hash over the display string
    std::size_t h = 1469598103934665603ULL;
    for (char c : std::meta::display_string_of(fn))
        h = (h ^ static_cast<unsigned char>(c)) * 1099511628211ULL;
    return h;
}

}  // namespace mocks

// ---- demonstration: a manually-authored mock that uses reflection keys ----

struct IClock {
    virtual ~IClock() = default;
    virtual int now() = 0;
    virtual std::string zone() = 0;
};

struct MockClock : IClock {
    mocks::tracker tracker;
    int now_return = 0;
    std::string zone_return = "UTC";

    int now() override {
        tracker.record(mocks::method_key(^^IClock::now));
        return now_return;
    }
    std::string zone() override {
        tracker.record(mocks::method_key(^^IClock::zone));
        return zone_return;
    }
};

int main() {
    MockClock m;
    m.now_return = 42;
    m.zone_return = "Europe/Warsaw";

    std::println("now:  {}", m.now());
    std::println("zone: {}", m.zone());
    std::println("now:  {}", m.now());

    std::println("IClock::now called {} times",
                 m.tracker.count(mocks::method_key(^^IClock::now)));
    std::println("IClock::zone called {} times",
                 m.tracker.count(mocks::method_key(^^IClock::zone)));
}

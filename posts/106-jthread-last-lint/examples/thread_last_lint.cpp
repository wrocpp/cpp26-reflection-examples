// A reflection check that a thread data member is declared after every other data member.
//
// Default build: the checked classes have the thread last, the program prints the result
//                of the check and exits 0.
// -DSHOW_ERROR   declares the thread of Meter before its counter, so the static_assert
//                fails and the compiler prints the message the check builds.
#include <atomic>
#include <cstdio>
#include <meta>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace lint {

consteval bool is_thread(std::meta::info type) {
    auto plain = std::meta::dealias(std::meta::remove_cv(type));
    return plain == ^^std::jthread || plain == ^^std::thread;
}

// An empty string when every thread member is last, otherwise a message that names the
// first thread member and the first member declared after it.
consteval std::string_view thread_last_message(std::meta::info cls) {
    std::string_view thread_name;
    for (auto member : std::meta::nonstatic_data_members_of(cls, std::meta::access_context::unchecked())) {
        if (is_thread(std::meta::type_of(member))) {
            if (thread_name.empty()) thread_name = std::meta::identifier_of(member);
            continue;
        }
        if (!thread_name.empty()) {
            std::string text = "thread member '";
            text += thread_name;
            text += "' is declared before '";
            text += std::meta::identifier_of(member);
            text += "'; declare the thread last";
            return std::string_view{std::define_static_string(text), text.size()};
        }
    }
    return {};
}

template <class T>
inline constexpr std::string_view thread_last = thread_last_message(^^T);

}  // namespace lint

struct Meter {
#ifdef SHOW_ERROR
    std::jthread worker{[this](std::stop_token st) { run(st); }};
    std::atomic<unsigned> ticks{0};
#else
    std::atomic<unsigned> ticks{0};
    std::jthread worker{[this](std::stop_token st) { run(st); }};
#endif

    void run(std::stop_token st) {
        while (!st.stop_requested()) ticks.fetch_add(1, std::memory_order_relaxed);
    }
};
static_assert(lint::thread_last<Meter>.empty(), lint::thread_last<Meter>);

struct Sampler {
    std::atomic<bool> ready{false};
    std::vector<int> samples;
    std::jthread worker{[this](std::stop_token) {}};
};
static_assert(lint::thread_last<Sampler>.empty(), lint::thread_last<Sampler>);

// Only reported, never asserted: a std::thread member declared before another member.
struct PlainFirst {
    std::thread worker;
    int count{};
};

int main() {
    std::printf("Meter: '%s'\n", std::string{lint::thread_last<Meter>}.c_str());
    std::printf("Sampler: '%s'\n", std::string{lint::thread_last<Sampler>}.c_str());
    std::printf("PlainFirst: '%s'\n", std::string{lint::thread_last<PlainFirst>}.c_str());
}

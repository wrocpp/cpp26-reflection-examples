// custom_formatter.cpp  (~40 lines)
// std::print's per-call atomicity extends to your own types: give a type a
// std::formatter and printing it from many threads still yields whole lines.
// C++26's P3107 is what keeps this both efficient and safe: it lets print write
// directly to the stream while holding the stream lock, and asks user formatters
// to opt in (enable_nonlocking_formatter_optimization) so a formatter that itself
// prints cannot deadlock. std formatters opt in automatically.
//
// Build (GCC 16.1):  g++ -std=c++23 -O2 -pthread custom_formatter.cpp

#include <print>
#include <format>
#include <thread>
#include <vector>
#include <string>

struct LogRecord {
    std::string thread;
    int seq;
};

template <>
struct std::formatter<LogRecord> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const LogRecord& r, std::format_context& ctx) const {
        return std::format_to(ctx.out(),
            "[thread {}] log record #{} -- formatted by a user formatter",
            r.thread, r.seq);
    }
};

void emit(const char* who) {
    for (int i = 0; i < 6; ++i)
        std::println("{}", LogRecord{who, i});   // one atomic call per line
}

int main() {
    std::vector<std::thread> t;
    for (const char* who : {"A", "B", "C"}) t.emplace_back(emit, who);
    for (auto& x : t) x.join();
}

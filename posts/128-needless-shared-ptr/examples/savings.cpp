// verify: gcc-only
// What a shared_ptr that is never shared costs over a unique_ptr: the size of the handle, the
// bytes of the one heap block, and the reference count. Operator new is replaced to count calls
// and bytes.
//
// Build: g++ -std=c++23 -O0 -Wall -Wextra savings.cpp -o savings      (GCC 16.2; clang 22 and later also build it)
// Run:   ./savings   prints three lines of sizes and counts, exits 0
// Limits: built at -O0, because an optimiser may elide an allocation nobody uses and report none.
// Sizes and counts are properties of the standard library in use (libstdc++ here). No time is
// measured, so nothing here says how much faster unique_ptr is.

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <new>

namespace {

std::size_t g_calls = 0;
std::size_t g_bytes = 0;

struct Snapshot {
    std::size_t calls;
    std::size_t bytes;
};

Snapshot snapshot() { return {.calls = g_calls, .bytes = g_bytes}; }

struct Widget {
    int value = 3;
};

}  // namespace

void* operator new(std::size_t size) {
    ++g_calls;
    g_bytes += size;
    if (void* block = std::malloc(size)) return block;
    throw std::bad_alloc{};
}

void operator delete(void* block) noexcept { std::free(block); }
void operator delete(void* block, std::size_t) noexcept { std::free(block); }

int main() {
    const Snapshot before_shared = snapshot();
    auto shared = std::make_shared<Widget>();
    const Snapshot after_shared = snapshot();
    auto unique = std::make_unique<Widget>();
    const Snapshot after_unique = snapshot();

    std::printf("sizeof(Widget) %zu, sizeof(shared_ptr) %zu, sizeof(unique_ptr) %zu\n", sizeof(Widget),
                sizeof(shared), sizeof(unique));
    std::printf("make_shared: %zu allocation, %zu bytes, use_count %ld\n", after_shared.calls - before_shared.calls,
                after_shared.bytes - before_shared.bytes, static_cast<long>(shared.use_count()));
    std::printf("make_unique: %zu allocation, %zu bytes\n", after_unique.calls - after_shared.calls,
                after_unique.bytes - after_shared.bytes);
    return 0;
}

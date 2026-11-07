// A generator and an exec::task side by side: pull versus async.
// exec::task is NVIDIA stdexec's reference implementation, not std::execution::task.
#include <stdexec/execution.hpp>
#include <exec/task.hpp>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <generator>
#include <new>

namespace {

std::size_t g_allocations = 0;
std::size_t g_bytes = 0;

}  // namespace

void* operator new(std::size_t size) {
  ++g_allocations;
  g_bytes += size;
  if (void* memory = std::malloc(size)) return memory;
  throw std::bad_alloc{};
}
void operator delete(void* memory) noexcept { std::free(memory); }
void operator delete(void* memory, std::size_t) noexcept { std::free(memory); }

// GCC 15+ assumes replaced operator new/delete have no side effects
// (-fassume-sane-operators-new-delete), so the counters are read through a call it cannot inline.
struct Counters {
  std::size_t allocations;
  std::size_t bytes;
};

[[gnu::noinline]] Counters read_counters() { return {.allocations = g_allocations, .bytes = g_bytes}; }
[[gnu::noinline]] void reset_counters() { g_allocations = 0; g_bytes = 0; }

void report(const char* label, int result) {
  const Counters counters = read_counters();
  std::printf("%-26s result %3d, %zu allocation(s), %zu bytes\n", label, result, counters.allocations, counters.bytes);
}

constexpr int answer = 42;
constexpr int count_to = 3;

std::generator<int> count_up(int last) {
  for (int value = 1; value <= last; ++value) co_yield value;
}

exec::task<int> fetch_answer() { co_return co_await stdexec::just(answer); }

exec::task<int> fetch_twice() {
  const int first = co_await fetch_answer();
  const int second = co_await fetch_answer();
  co_return first + second;
}

int main() {
  reset_counters();
  int sum = 0;
  for (int value : count_up(count_to)) sum += value;
  report("generator, 3 values", sum);

  reset_counters();
  auto [one] = stdexec::sync_wait(fetch_answer()).value();
  report("task, one", one);

  reset_counters();
  auto [two] = stdexec::sync_wait(fetch_twice()).value();
  report("task awaiting two tasks", two);
}

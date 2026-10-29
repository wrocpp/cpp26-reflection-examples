// verify: gcc-only
// Coroutines as a debugger, part 2: what turning every function into a coroutine costs. The same
// recursive Fibonacci is written three ways: a plain function; a coroutine with no checkpoints, which
// isolates the cost of the coroutine frame; and a coroutine with a checkpoint at entry that never stops,
// which adds the cost of the checkpoint itself. Heap allocations are counted by replacing operator new,
// so that column is exact and the same on every machine. Times are the fastest of seven wall-clock runs
// on whatever runs this file. They are noisy: run it several times and expect them to move.
// This is a toy model. Qumir instruments only with --debug-points and shares one await loop per
// function, so its numbers are not these numbers.
// Measured on GCC 16.2 (arm64, Apple M2 Max): at -O0 and -O1 the 635,621-call run crashes with the default
// 8 MB stack (fib(20) runs, fib(22) does not) and completes on a 1 GiB stack, so it is stack use. Why -O2 does
// not need the stack is not known: -O2 -fno-optimize-sibling-calls still runs, and -O1 -foptimize-sibling-calls
// still crashes, so sibling-call optimization is not the explanation.
// Build: g++-16 -std=c++23 -O2 -Wall -Wextra -Werror coroutine_cost.cpp   (Compiler Explorer: g162)
#include <algorithm>
#include <chrono>
#include <coroutine>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <new>
#include <utility>

namespace {

constexpr int kFibArgument = 27;   // 635,621 calls
constexpr int kRepeats = 7;        // wall-clock runs per variant; the fastest is reported

std::size_t g_allocations = 0;
std::size_t g_bytes = 0;

// Read through volatile so the optimizer cannot fold the recursion or drop an unused result.
volatile int g_argument = kFibArgument;
volatile int g_sink = 0;

}  // namespace

void* operator new(std::size_t size) {
  ++g_allocations;
  g_bytes += size;
  if (void* p = std::malloc(size)) return p;
  throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

namespace {

// The checkpoint. With stop == false it takes the fast path: await_ready() is true, nothing suspends.
struct Point {
  bool stop;
  bool await_ready() const noexcept { return !stop; }
  void await_suspend(std::coroutine_handle<>) const noexcept {}
  void await_resume() const noexcept {}
};

template <class T>
struct Task {
  struct promise_type;
  using handle = std::coroutine_handle<promise_type>;
  struct promise_type {
    T value{};
    std::coroutine_handle<> continuation;
    Task get_return_object() { return Task{handle::from_promise(*this)}; }
    std::suspend_always initial_suspend() noexcept { return {}; }
    struct Final {
      bool await_ready() const noexcept { return false; }
      std::coroutine_handle<> await_suspend(handle h) const noexcept {
        return h.promise().continuation ? h.promise().continuation : std::noop_coroutine();
      }
      void await_resume() const noexcept {}
    };
    Final final_suspend() noexcept { return {}; }
    void return_value(T v) { value = std::move(v); }
    void unhandled_exception() { std::terminate(); }
  };

  handle h;
  explicit Task(handle handle_) : h(handle_) {}
  Task(Task&& other) noexcept : h(std::exchange(other.h, {})) {}
  ~Task() { if (h) h.destroy(); }

  bool await_ready() const noexcept { return false; }
  std::coroutine_handle<> await_suspend(std::coroutine_handle<> caller) noexcept {
    h.promise().continuation = caller;
    return h;
  }
  T await_resume() { return std::move(h.promise().value); }
};

int fib_plain(int n) { return n < 2 ? n : fib_plain(n - 1) + fib_plain(n - 2); }

Task<int> fib_frame_only(int n) {
  if (n < 2) co_return n;
  const int a = co_await fib_frame_only(n - 1);
  const int b = co_await fib_frame_only(n - 2);
  co_return a + b;
}

Task<int> fib_with_point(int n) {
  co_await Point{false};                       // a checkpoint at function entry that never stops
  if (n < 2) co_return n;
  const int a = co_await fib_with_point(n - 1);
  const int b = co_await fib_with_point(n - 2);
  co_return a + b;
}

// Drives a root task to completion. Nothing ever stops, so one resume runs the whole call tree.
template <class MakeTask>
int run_coroutine(MakeTask make) {
  auto root = make();
  root.h.resume();
  return root.h.promise().value;
}

std::size_t calls_for(int n) {  // calls in the naive recursion: 2 * fib(n + 1) - 1
  std::size_t a = 0, b = 1;
  for (int i = 0; i <= n; ++i) { const auto next = a + b; a = b; b = next; }
  return 2 * a - 1;
}

template <class Run>
double fastest_ms(Run run) {
  double ms[kRepeats];
  for (double& m : ms) {
    const auto start = std::chrono::steady_clock::now();
    g_sink = run();
    m = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
  }
  return *std::min_element(ms, ms + kRepeats);
}

template <class Run>
void measure(const char* name, std::size_t calls, double plain_ms, Run run) {
  const std::size_t allocations_before = g_allocations;
  const std::size_t bytes_before = g_bytes;
  const int result = run();                    // one counted run
  const std::size_t allocations = g_allocations - allocations_before;
  const std::size_t bytes = g_bytes - bytes_before;
  const double ms = fastest_ms(run);
  std::printf("%-24s result=%d allocations=%zu (%.2f per call) bytes/allocation=%zu  fastest %.2f ms (%.1fx)\n",
              name, result, allocations, static_cast<double>(allocations) / static_cast<double>(calls),
              allocations ? bytes / allocations : std::size_t{0}, ms, plain_ms > 0 ? ms / plain_ms : 1.0);
}

}  // namespace

int main() {
  const std::size_t calls = calls_for(kFibArgument);
  std::printf("fib(%d): %zu calls\n", kFibArgument, calls);
  const double plain_ms = fastest_ms([] { return fib_plain(g_argument); });
  measure("plain function", calls, 0, [] { return fib_plain(g_argument); });
  measure("coroutine, no checkpoint", calls, plain_ms, [] { return run_coroutine([] { return fib_frame_only(g_argument); }); });
  measure("coroutine + checkpoint", calls, plain_ms, [] { return run_coroutine([] { return fib_with_point(g_argument); }); });
}

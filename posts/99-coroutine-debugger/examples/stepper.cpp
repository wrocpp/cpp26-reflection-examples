// verify: gcc-only
// Coroutines as a debugger, part 1: a single-stepper built from suspend points. This is a toy that models
// the mechanism the Qumir debugger design note describes (a checkpoint before each statement, state kept
// next to the program, a fast path that does not suspend), written with C++20 coroutines. It is not
// Qumir's code: Qumir lowers its own coroutine/Future ABI through LLVM coroutine intrinsics.
// What is modelled: breakpoints, step into, step over, step out, continue, and a call stack at each stop.
// What is not: variables, a UI, a browser. The "program" is a pretend source with numbered lines.
// Build: g++-16 -std=c++23 -Wall -Wextra -Werror stepper.cpp   (Compiler Explorer: g162; native, not WebAssembly)
#include <coroutine>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace dbg {

enum class Mode { Continue, Into, Over, Out };

// All debugger state lives here, next to the program. The pretend source it steps through is:
//   1 alg square(x)            5   total := 0
//   2   result := x * x        6   for i from 1 to 3
//   3   return result          7     total := total + square(i)
//   4 alg main                 8   return total
enum Line : int { kSquareMul = 2, kSquareReturn = 3, kMainInit = 5, kMainLoop = 6, kMainAdd = 7, kMainReturn = 8 };

struct Session {
  Mode mode = Mode::Continue;
  std::size_t target_depth = 0;            // frame depth at the last stop, for Over and Out
  std::set<int> breakpoints;
  std::vector<const char*> stack;          // function names, innermost last
  std::coroutine_handle<> paused;          // the innermost coroutine, suspended at the current stop
  int paused_line = 0;
  long points_evaluated = 0;
  long points_stopped = 0;

  bool should_stop(int line) const {
    if (breakpoints.contains(line)) return true;
    switch (mode) {
      case Mode::Continue: return false;
      case Mode::Into:     return true;
      case Mode::Over:     return stack.size() <= target_depth;   // same frame or shallower
      case Mode::Out:      return stack.size() < target_depth;    // strictly shallower
    }
    return false;
  }
};
inline Session session;

// A checkpoint before a statement. The fast path is await_ready() returning true: no suspension, no
// handle stored, nothing allocated. Only a stopping point suspends and hands control back to the driver.
struct Point {
  int line;
  bool await_ready() const noexcept {
    ++session.points_evaluated;
    if (!session.should_stop(line)) return true;
    ++session.points_stopped;
    return false;
  }
  void await_suspend(std::coroutine_handle<> h) const noexcept {
    session.paused = h;
    session.paused_line = line;
  }
  void await_resume() const noexcept {}
};

// Keeps the call stack current while a function body is alive, including across suspension.
struct Frame {
  explicit Frame(const char* name) { session.stack.push_back(name); }
  ~Frame() { session.stack.pop_back(); }
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

  // Awaiting a Task starts the callee and resumes the caller when it finishes (symmetric transfer),
  // so a stop deep in the call chain returns control straight to whoever called resume().
  bool await_ready() const noexcept { return false; }
  std::coroutine_handle<> await_suspend(std::coroutine_handle<> caller) noexcept {
    h.promise().continuation = caller;
    return h;
  }
  T await_resume() { return std::move(h.promise().value); }
};

}  // namespace dbg

using dbg::Line;
using dbg::Point;

// The program under debug. Every function that contains a checkpoint, or calls one that does, is a coroutine.
dbg::Task<int> square(int x) {
  dbg::Frame frame{"square"};
  co_await Point{Line::kSquareMul};
  const int result = x * x;
  co_await Point{Line::kSquareReturn};
  co_return result;
}

dbg::Task<int> program() {
  dbg::Frame frame{"main"};
  co_await Point{Line::kMainInit};
  int total = 0;
  for (int i = 1; i <= 3; ++i) {
    co_await Point{Line::kMainLoop};
    co_await Point{Line::kMainAdd};
    total += co_await square(i);
  }
  co_await Point{Line::kMainReturn};
  co_return total;
}

namespace {

std::string stack_text() {
  std::string out;
  for (const char* name : dbg::session.stack) out += (out.empty() ? "" : " > ") + std::string(name);
  return out;
}

// Runs until the next stop or the end. Returns true while the program is still alive.
bool run(dbg::Task<int>& root, dbg::Mode mode) {
  auto& s = dbg::session;
  s.mode = mode;
  s.target_depth = s.stack.size();
  const auto next = s.paused ? s.paused : std::coroutine_handle<>(root.h);
  s.paused = {};
  next.resume();
  if (root.h.done()) return false;
  std::printf("  stop at line %d  [%s]\n", s.paused_line, stack_text().c_str());
  return true;
}

}  // namespace

int main() {
  auto root = program();
  auto& s = dbg::session;

  std::puts("break at line 3 (return result), continue:");
  s.breakpoints = {Line::kSquareReturn};
  run(root, dbg::Mode::Continue);

  std::puts("step out of square:");
  run(root, dbg::Mode::Out);

  std::puts("step into, three times (the last two enter square):");
  for (int n = 0; n < 3; ++n) run(root, dbg::Mode::Into);

  std::puts("clear the breakpoint, step into twice to reach the call on line 7:");
  s.breakpoints.clear();
  run(root, dbg::Mode::Into);
  run(root, dbg::Mode::Into);

  std::puts("step over the call: square runs, no stop inside it:");
  run(root, dbg::Mode::Over);

  std::puts("continue to the end:");
  const bool alive = run(root, dbg::Mode::Continue);
  std::printf("  finished=%d result=%d\n", !alive, root.h.promise().value);

  // The fast path: a fresh run with no breakpoints evaluates every checkpoint and stops at none.
  std::puts("a fresh run with no breakpoints:");
  auto fresh = program();
  s.points_evaluated = 0;
  s.points_stopped = 0;
  run(fresh, dbg::Mode::Continue);
  std::printf("  result=%d, checkpoints evaluated=%ld stopped=%ld\n",
              fresh.h.promise().value, s.points_evaluated, s.points_stopped);
}

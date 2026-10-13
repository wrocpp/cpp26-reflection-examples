// verify: gcc-only
// Manual mint, not the pipeline: needs -fsanitize=thread -pthread, and libc++ on the clang side.
// g++ -std=c++20 -fsanitize=thread -pthread
// clang++ -std=c++20 -stdlib=libc++ -fsanitize=thread -pthread
#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

// Keep the exit code 0 so the demo can run on Compiler Explorer.
extern "C" const char* __tsan_default_options() { return "exitcode=0"; }

using Task = std::function<void()>;

struct WorkerState {
  int count = 0;  // only the worker thread may touch it
};

struct Resource {
  WorkerState* state;
  explicit Resource(WorkerState* s) : state(s) { ++state->count; }
  ~Resource() { --state->count; }  // cleanup that assumes the worker thread
};

struct Queue {
  std::mutex m;
  std::condition_variable cv;
  Task slot;
  std::atomic<bool> dropped{false};  // relaxed: gives TSan no ordering
};

void work(Queue& q, WorkerState& st) {
  std::unique_lock lk(q.m);
  q.cv.wait(lk, [&] { return q.slot != nullptr; });
  Task task;
  task.swap(q.slot);
  lk.unlock();
  task();
  task = nullptr;  // the worker drops its reference
  q.dropped.store(true, std::memory_order_relaxed);
  ++st.count;  // later work on the worker's own state
}

namespace naive {
void push(Queue& q, Task f) {
  std::scoped_lock lk(q.m);
  q.slot = std::move(f);
  q.cv.notify_all();
}
void layer(Queue& q, Task f) { push(q, std::move(f)); }
}  // namespace naive

namespace fixed {
Task take(Task& f) { return std::exchange(f, nullptr); }
void push(Queue& q, Task&& f) {
  std::scoped_lock lk(q.m);
  q.slot = take(f);
  q.cv.notify_all();
}
void layer(Queue& q, Task&& f) { push(q, take(f)); }
}  // namespace fixed

template <class Layer>
void run(const char* name, Layer layer) {
  std::fprintf(stderr, "--- %s\n", name);
  WorkerState st;
  Queue q;
  std::thread worker;
  {
    auto res = std::make_shared<Resource>(&st);
    Task task = [res] {};
    res.reset();
    worker = std::thread(work, std::ref(q), std::ref(st));
    layer(q, std::move(task));
    while (!q.dropped.load(std::memory_order_relaxed)) std::this_thread::yield();
    // Scope exit: a leftover copy of the task dies here, on this thread.
  }
  worker.join();
}

int main() {
  run("by-value chain", [](Queue& q, Task&& t) { naive::layer(q, std::move(t)); });
  run("exchange at each layer", [](Queue& q, Task&& t) { fixed::layer(q, std::move(t)); });
  std::fprintf(stderr, "--- done\n");
}

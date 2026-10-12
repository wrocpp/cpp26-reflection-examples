// verify: gcc-only
// Manual mint, not the pipeline: needs -pthread, and libc++ on the clang side.
// g++ -std=c++20 -pthread   and   clang++ -std=c++20 -stdlib=libc++ -pthread
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

using Task = std::function<void()>;

const char* g_destroyed_on = "never";  // written by the destroying thread

struct Resource {
  std::thread::id producer;
  explicit Resource(std::thread::id p) : producer(p) {}
  ~Resource() {
    g_destroyed_on = std::this_thread::get_id() == producer
                         ? "PRODUCER (wrong)"
                         : "WORKER (right)";
  }
};

struct Queue {
  std::mutex m;
  std::condition_variable cv;
  Task slot;
  bool consumed = false;
};

// Worker: take the task out of the slot, run it, drop it, signal.
void work(Queue& q) {
  std::unique_lock lk(q.m);
  q.cv.wait(lk, [&] { return q.slot != nullptr; });
  Task task;
  task.swap(q.slot);
  lk.unlock();
  task();
  task = nullptr;
  lk.lock();
  q.consumed = true;
  q.cv.notify_all();
}

// The three hand-off styles. Each has two layers above the queue.
namespace naive {
void push(Queue& q, Task f) {
  std::scoped_lock lk(q.m);
  q.slot = std::move(f);
  q.cv.notify_all();
}
void layer(Queue& q, Task f) { push(q, std::move(f)); }
}  // namespace naive

namespace swap_out {
Task take(Task& f) { return std::exchange(f, nullptr); }
void push(Queue& q, Task&& f) {
  std::scoped_lock lk(q.m);
  q.slot = take(f);
  q.cv.notify_all();
}
void layer(Queue& q, Task&& f) { push(q, take(f)); }
}  // namespace swap_out

namespace indirect {
using Boxed = std::shared_ptr<Task>;
void push(Queue& q, Boxed b) {
  std::scoped_lock lk(q.m);
  q.slot = [b = std::move(b)] { Task t = std::move(*b); t(); };
  q.cv.notify_all();
}
void layer(Queue& q, Task&& f) {
  push(q, std::make_shared<Task>(std::exchange(f, nullptr)));
}
}  // namespace indirect

template <class Layer>
void run(const char* name, Layer layer) {
  Queue q;
  std::thread worker(work, std::ref(q));
  {
    auto res = std::make_shared<Resource>(std::this_thread::get_id());
    Task task = [res] {};  // one shared_ptr: fits libc++'s inline buffer
    res.reset();           // the task holds the only reference
    layer(q, std::move(task));
    std::unique_lock lk(q.m);
    q.cv.wait(lk, [&] { return q.consumed; });
  }  // leftover copies on this thread die here, after the worker finished
  worker.join();
  std::printf("%-9s last reference dropped on: %s\n", name, g_destroyed_on);
}

int main() {
  run("naive", [](Queue& q, Task&& t) { naive::layer(q, std::move(t)); });
  run("swap_out", [](Queue& q, Task&& t) { swap_out::layer(q, std::move(t)); });
  run("indirect", [](Queue& q, Task&& t) { indirect::layer(q, std::move(t)); });
}

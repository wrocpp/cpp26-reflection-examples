// verify: gcc-only
// Manual mint, not the pipeline: needs -pthread, and libc++ on the clang side.
// g++ -std=c++20 -pthread   and   clang++ -std=c++20 -stdlib=libc++ -pthread
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

struct Resource {
  std::thread::id producer;
  explicit Resource(std::thread::id p) : producer(p) {}
  ~Resource() {
    std::puts(std::this_thread::get_id() == producer
                  ? "last reference dropped on: PRODUCER thread (wrong)"
                  : "last reference dropped on: WORKER thread (right)");
  }
};

int main() {
  std::mutex m;
  std::condition_variable cv;
  bool task_destroyed = false;
  std::function<void()> queued;  // the one-slot "queue"

  std::thread worker([&] {
    std::unique_lock lk(m);
    cv.wait(lk, [&] { return queued != nullptr; });
    auto task = std::move(queued);  // take it out of the queue
    lk.unlock();
    task();
    task = nullptr;  // the worker drops its copy: task destroyed here
    lk.lock();
    task_destroyed = true;
    cv.notify_all();
  });

  {
    auto res = std::make_shared<Resource>(std::this_thread::get_id());
    // Capture one shared_ptr: small enough for libc++'s inline buffer.
    std::function<void()> task = [res] { std::puts("task ran"); };
    res.reset();  // the task now holds the only reference

    std::function<void()> slot = std::move(task);  // hand off to the queue layer
    {
      std::scoped_lock lk(m);
      queued = std::move(slot);
    }
    cv.notify_all();

    std::unique_lock lk(m);
    cv.wait(lk, [&] { return task_destroyed; });
    lk.unlock();
    // The worker has finished and released its task. If `task` or `slot`
    // still hold a copy, the Resource dies when this scope ends, here.
  }
  worker.join();
}

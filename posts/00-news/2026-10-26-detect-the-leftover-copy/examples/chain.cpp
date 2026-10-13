#include <functional>
#include <memory>
#include <mutex>
#include <utility>

using Task = std::function<void()>;
struct Queue { std::mutex m; Task slot; };

namespace naive {
void push(Queue& q, Task f) {
  std::scoped_lock lk(q.m);
  q.slot = std::move(f);
}
void layer(Queue& q, Task f) { push(q, std::move(f)); }
}  // namespace naive

namespace fixed {
Task take(Task& f) { return std::exchange(f, nullptr); }
void push(Queue& q, Task&& f) {
  std::scoped_lock lk(q.m);
  q.slot = take(f);
}
void layer(Queue& q, Task&& f) { push(q, take(f)); }
}  // namespace fixed

void caller(Queue& q) {
  auto sp = std::make_shared<int>(1);
  Task t = [sp] {};
  naive::layer(q, std::move(t));
  Task u = [sp] {};
  fixed::layer(q, std::move(u));
}

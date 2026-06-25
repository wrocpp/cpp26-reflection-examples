// when_all.cpp  (~40 lines)
// "Structured concurrency in C++26": the std::execution feature that std::future
// never had -- composing concurrent work and joining it without a single manual
// lock, future, or thread handle.
//
// `static_thread_pool` is a scheduler: a handle to where work runs. `on(sched,
// snd)` moves a sender's work onto that pool. `when_all(a, b)` is itself a
// sender that runs a and b concurrently and completes with BOTH results once
// both finish. `sync_wait` blocks the calling thread until the whole graph is
// done. The pool's destructor joins -- no leaked threads, no detach.
//
// This is the headline over std::future/std::promise: senders compose. You can
// hand `when_all(a, b)` to another adaptor and keep building; a future is a
// dead end you can only .get() once.
//
// Uses the NVIDIA stdexec reference implementation (std::execution is not yet in
// shipping standard-library headers in June 2026); namespaces map 1:1 to std.
//
// verify: ce-libs: stdexec@trunk
// Compile (GCC 16.1):              g++-16 -std=c++26 -I<stdexec>/include when_all.cpp -o demo
// Compile (Bloomberg clang-p2996): clang++ -std=c++26 -stdlib=libc++ -I<stdexec>/include when_all.cpp -o demo

#include <stdexec/execution.hpp>
#include <exec/static_thread_pool.hpp>
#include <print>

int main() {
    exec::static_thread_pool pool{2};
    auto sched = pool.get_scheduler();

    // Two independent pipelines, each scheduled onto the pool.
    auto a = stdexec::on(sched, stdexec::just(10) | stdexec::then([](int x) { return x + 1; }));
    auto b = stdexec::on(sched, stdexec::just(20) | stdexec::then([](int x) { return x + 2; }));

    // Run both concurrently; complete with both results once both finish.
    auto [x, y] = stdexec::sync_wait(stdexec::when_all(a, b)).value();

    std::println("a={} b={} sum={}", x, y, x + y);
}

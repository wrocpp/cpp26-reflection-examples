// A coroutine promise with both return_value and return_void.
// GCC 16.2 rejects the promise type; GCC trunk accepts it (P3950R1).
#include <coroutine>
#include <cstdio>
#include <exception>

struct Task {
    struct promise_type {
        int value = -1;
        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(int v) { value = v; }
        void return_void() { value = 0; }
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;
    explicit Task(std::coroutine_handle<promise_type> h) : handle{h} {}
    Task(Task&& other) noexcept : handle{other.handle} { other.handle = nullptr; }
    ~Task() { if (handle) handle.destroy(); }
    int result() const { return handle.promise().value; }
};

Task compute(bool early) {
    if (early) {
        co_return;
    }
    co_return 42;
}

int main() {
    std::printf("__cpp_impl_coroutine = %ldL\n", static_cast<long>(__cpp_impl_coroutine));
    std::printf("co_return;    -> %d\n", compute(true).result());
    std::printf("co_return 42; -> %d\n", compute(false).result());
}

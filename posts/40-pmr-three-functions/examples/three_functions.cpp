// three_functions.cpp  (~45 lines)
// The whole std::pmr framework rests on ONE abstract class,
// std::pmr::memory_resource, with three private virtuals to override:
// do_allocate, do_deallocate, do_is_equal. Everything else in
// <memory_resource> (monotonic_buffer_resource, the pool resources, every
// pmr:: container) is built on that one seam.
//
// This demo writes a resource in a dozen lines that logs every request, watches
// a pmr::vector grow through it, then drops a monotonic_buffer_resource IN FRONT
// of the same logger as its upstream. Now the vector's many small requests are
// served from a 512-byte stack buffer and the upstream logger is never called.
// One interface; the resources compose.
//
// Build (GCC 16.1):  g++ -std=c++17 -O2 three_functions.cpp

#include <memory_resource>
#include <vector>
#include <iostream>

// A memory_resource is three functions. This one forwards to an upstream
// resource and prints every allocation it is asked for.
class logging_resource : public std::pmr::memory_resource {
public:
    explicit logging_resource(std::pmr::memory_resource* up = std::pmr::new_delete_resource())
        : upstream_{up} {}

private:
    void* do_allocate(std::size_t bytes, std::size_t align) override {
        std::cout << "  allocate " << bytes << " bytes\n";
        return upstream_->allocate(bytes, align);
    }
    void do_deallocate(void* p, std::size_t bytes, std::size_t align) override {
        upstream_->deallocate(p, bytes, align);
    }
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

    std::pmr::memory_resource* upstream_;
};

int main() {
    std::cout << "vector straight on the logger:\n";
    logging_resource log;
    std::pmr::vector<int> v(&log);
    for (int i = 0; i < 8; ++i) v.push_back(i);   // each growth is one request

    std::cout << "\nvector on a monotonic arena, logger as upstream:\n";
    logging_resource log2;
    std::byte buffer[512];
    std::pmr::monotonic_buffer_resource arena(buffer, sizeof(buffer), &log2);
    std::pmr::vector<int> w(&arena);
    for (int i = 0; i < 8; ++i) w.push_back(i);   // served from the stack buffer
    std::cout << "  (the arena served every push from its buffer; upstream untouched)\n";
}

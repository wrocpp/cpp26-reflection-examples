// lifetime.cpp  (~45 lines)
// A pmr container deallocates back into its memory_resource when it is
// destroyed, so the resource must OUTLIVE the container. Because destructors run
// in reverse declaration order, that means: declare the resource first, the
// container second. Get it backwards and the container's destructor deallocates
// into a resource that is already gone, which is undefined behavior and the
// classic pmr crash.
//
// This demo prints the destruction order to make the rule concrete.
//
// Build (GCC 16.1):  g++ -std=c++17 -O2 lifetime.cpp

#include <memory_resource>
#include <vector>
#include <cstdio>

struct announcing_resource : std::pmr::memory_resource {
    const char* name;
    explicit announcing_resource(const char* n) : name(n) {}
    ~announcing_resource() { std::printf("  destroy resource(%s)\n", name); }
    void* do_allocate(std::size_t b, std::size_t a) override {
        return std::pmr::new_delete_resource()->allocate(b, a);
    }
    void do_deallocate(void* p, std::size_t b, std::size_t a) override {
        std::printf("  vector deallocates into resource(%s)\n", name);
        std::pmr::new_delete_resource()->deallocate(p, b, a);
    }
    bool do_is_equal(const std::pmr::memory_resource& o) const noexcept override { return this == &o; }
};

int main() {
    std::printf("resource declared first, so it is destroyed last:\n");
    {
        announcing_resource res{"arena"};      // declared first
        std::pmr::vector<int> v(&res);         // declared second
        v.assign({1, 2, 3});
        // Scope exit: v is destroyed first (it deallocates into a live resource),
        // then res. Swap these two lines and the deallocation above would hit an
        // already-destroyed resource.
    }
    std::printf("done: the vector freed its memory while the resource was still alive\n");
}

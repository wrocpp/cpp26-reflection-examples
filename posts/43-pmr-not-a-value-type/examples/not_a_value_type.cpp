// not_a_value_type.cpp  (~35 lines)
// A pmr::vector carries its memory_resource, but copy construction does NOT copy
// it. select_on_container_copy_construction returns a default-constructed
// polymorphic_allocator, so the copy silently allocates from the DEFAULT
// resource instead of the source's arena. Two vectors that compare equal can
// live in completely different memory.
//
// Build (GCC 16.1):  g++ -std=c++17 -O2 not_a_value_type.cpp

#include <memory_resource>
#include <vector>
#include <cstdio>

int main() {
    std::byte buffer[4096];
    std::pmr::monotonic_buffer_resource arena(buffer, sizeof(buffer));

    std::pmr::vector<int> original(&arena);
    original.assign({1, 2, 3, 4});

    std::pmr::vector<int> copy = original;      // copy-construct

    auto res = [](const std::pmr::vector<int>& v){ return v.get_allocator().resource(); };

    std::printf("copy == original           : %s\n", (copy == original) ? "true" : "false");
    std::printf("copy shares original's arena: %s\n", (res(copy) == res(original)) ? "true" : "false");
    std::printf("original uses the arena     : %s\n", (res(original) == &arena) ? "true" : "false");
    std::printf("copy uses the arena         : %s\n", (res(copy) == &arena) ? "true" : "false");
    std::printf("copy fell back to default   : %s\n",
                (res(copy) == std::pmr::get_default_resource()) ? "true" : "false");
}

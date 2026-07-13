// allocator_aware.cpp  (~45 lines)
// To put a pmr container INSIDE your own type and have it honor the arena, the
// type must become allocator-aware: expose allocator_type, take an optional
// allocator argument in its constructors, and forward it to the members. This is
// "uses-allocator construction". Skip it and the member silently allocates from
// the default resource even when the enclosing object lives in an arena.
//
// Build (GCC 16.1):  g++ -std=c++17 -O2 allocator_aware.cpp

#include <memory_resource>
#include <string>
#include <string_view>
#include <vector>
#include <cstdio>

struct Record {
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

    std::pmr::string name;

    // Normal constructor, with an optional allocator argument.
    explicit Record(std::string_view n, allocator_type alloc = {})
        : name(n, alloc) {}

    // Allocator-extended copy constructor (the "secret twin"): same value, but
    // built with the caller's allocator so the member lands in the arena.
    Record(const Record& other, allocator_type alloc)
        : name(other.name, alloc) {}

    allocator_type get_allocator() const { return name.get_allocator(); }
};

int main() {
    std::byte buffer[8192];
    std::pmr::monotonic_buffer_resource arena(buffer, sizeof(buffer));

    std::pmr::polymorphic_allocator<std::byte> alloc(&arena);
    Record r("a name long enough to actually allocate on the heap", alloc);
    std::printf("record's string uses the arena              : %s\n",
                r.name.get_allocator().resource() == &arena ? "true" : "false");

    // pmr::vector passes its allocator into each Record via the allocator-extended
    // constructor: uses-allocator construction, forwarded to the string member.
    std::pmr::vector<Record> records(&arena);
    records.emplace_back("another string long enough to allocate on the heap");
    std::printf("record inside pmr::vector uses the arena    : %s\n",
                records[0].name.get_allocator().resource() == &arena ? "true" : "false");
}

// verify: gcc-only
// The one false positive of wrocpp-needless-shared-ptr that I found: a class that shares itself.
// `register_node` only dereferences its pointer, so the check flags it, but the object hands out
// shared_ptr copies of itself through shared_from_this(). `register_node_after_fix` is what the
// fix-it produces from it.
//
// Build: g++ -std=c++23 -Wall -Wextra shared_from_this.cpp -o shared_from_this      (GCC 16.2)
// Run:   ./shared_from_this   prints what each version did, exits 0
// Limits: the exception is caught and printed; in real code it would end the program.

#include <cstddef>
#include <cstdio>
#include <exception>
#include <memory>
#include <vector>

struct Node;
using Registry = std::vector<std::shared_ptr<Node>>;

struct Node : std::enable_shared_from_this<Node> {
    void announce(Registry& registry) { registry.push_back(shared_from_this()); }
};

// Flagged by the check: only operator-> is used on `n` in this function.
std::size_t register_node(Registry& registry) {
    auto n = std::make_shared<Node>();
    n->announce(registry);
    return registry.size();
}

// The same function after `clang-tidy --fix`.
std::size_t register_node_after_fix(Registry& registry) {
    auto n = std::make_unique<Node>();
    n->announce(registry);
    return registry.size();
}

int main() {
    Registry registry;
    std::printf("before the fix: registry holds %zu\n", register_node(registry));
    try {
        register_node_after_fix(registry);
    } catch (const std::exception& error) {
        std::printf("after the fix: %s\n", error.what());
    }
    return 0;
}

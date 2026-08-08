// move_out_of_top.cpp  (~26 lines)
// std::priority_queue::top() returns a CONST reference, because mutating the
// top element could break the heap invariant. That is defensible, and it also
// means you cannot move an element out of the queue: the copy constructor is
// selected, and for a move-only payload the code simply does not compile.
//
// The escape hatch is a const_cast, and it is safe here for one specific
// reason: pop() runs immediately afterwards, so the moved-from element is
// destroyed before the queue can ever compare it again. Move and pop must stay
// adjacent; anything between them is a bug.
//
// Compile (GCC 16.1): g++ -std=c++23 -O2 move_out_of_top.cpp

#include <memory>
#include <print>
#include <queue>
#include <string>

struct Task {
    int priority;
    std::unique_ptr<std::string> payload;   // move-only, so no copying
    bool operator<(const Task& o) const { return priority < o.priority; }
};

int main() {
    std::priority_queue<Task> pq;
    pq.push(Task{1, std::make_unique<std::string>("low")});
    pq.push(Task{9, std::make_unique<std::string>("high")});
    pq.push(Task{5, std::make_unique<std::string>("mid")});

    while (!pq.empty()) {
        Task t = std::move(const_cast<Task&>(pq.top()));  // cast, then
        pq.pop();                                         // pop immediately
        std::println("priority {} -> {}", t.priority, *t.payload);
    }
}

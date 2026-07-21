// tsan_data_race.cpp  (~18 lines)
// ThreadSanitizer catches the unsynchronized ++counter from two threads and
// prints both stacks plus the conflicting accesses. TSan reports and continues,
// setting a nonzero exit by default, so the __tsan_default_options hook sets
// exitcode=0 to keep this runnable. The final counter is nondeterministic; the
// point is that TSan flags the race whatever this particular run adds up to.
//
// Compile (GCC 16.1): g++ -std=c++20 -O1 -g -fsanitize=thread -pthread tsan_data_race.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++20 -O1 -g -fsanitize=thread -pthread

#include <cstdio>
#include <thread>

extern "C" const char* __tsan_default_options() { return "exitcode=0"; }

int main() {
    int counter = 0;
    auto bump = [&] { for (int i = 0; i < 100000; ++i) ++counter; };
    std::thread t1(bump), t2(bump);
    t1.join();
    t2.join();
    std::printf("counter = %d (a data race; ThreadSanitizer flags it either way)\n", counter);
    return 0;
}

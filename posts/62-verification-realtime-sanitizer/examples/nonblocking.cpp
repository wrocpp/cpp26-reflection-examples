// nonblocking.cpp  (~22 lines)
// RealtimeSanitizer (Clang, -fsanitize=realtime) checks a promise the type
// system cannot: that a function marked [[clang::nonblocking]] never calls
// anything that might block. Allocating, locking, or making a syscall inside
// one is reported at runtime with a full stack, the way ASan reports a bad
// access. This is the tool for audio callbacks, control loops, and any hot
// path where a hidden malloc is a missed deadline.
//
// RTSan exits nonzero on the first report, so halt_on_error=false keeps this a
// runnable demo. In your own build you want the default.
//
// Compile (clang): clang++ -std=c++20 -O1 -g -fsanitize=realtime nonblocking.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++20 -O1 -g -fsanitize=realtime

#include <cstdio>
#include <mutex>
#include <vector>

extern "C" const char* __rtsan_default_options() { return "halt_on_error=false"; }

std::mutex m;

// The promise: this function will never block.
void audio_callback(std::vector<float>& buf) [[clang::nonblocking]] {
    buf.push_back(1.0f);               // grows the vector -> malloc -> reported
    std::lock_guard<std::mutex> g(m);  // takes a lock          -> reported
}

int main() {
    std::vector<float> buf;            // empty, so push_back must allocate
    audio_callback(buf);
    std::printf("callback returned, size=%zu\n", buf.size());
    return 0;
}

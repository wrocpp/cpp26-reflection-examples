// feature_test_fallback.cpp  (~35 lines)
// Ship one file that uses std::osyncstream where the library has it and falls
// back to a mutex-guarded write where it does not. <version> exposes the
// feature-test macro __cpp_lib_syncbuf without pulling in <syncstream>. This is
// the portable pattern from the original Qt-logging code review.
//
// Build (GCC 16.1):  g++ -std=c++20 -O2 -pthread feature_test_fallback.cpp

#include <version>
#include <iostream>
#include <thread>
#include <vector>
#if defined(__cpp_lib_syncbuf)
  #include <syncstream>
#else
  #include <mutex>
#endif

void log_line(const char* who, int i) {
#if defined(__cpp_lib_syncbuf)
    std::osyncstream(std::cout)
        << "[thread " << who << "] log record #" << i << " (syncstream path)\n";
#else
    static std::mutex mtx;
    const std::lock_guard lock(mtx);
    std::cout << "[thread " << who << "] log record #" << i << " (mutex path)\n";
#endif
}

int main() {
#if defined(__cpp_lib_syncbuf)
    std::cout << "compiled with __cpp_lib_syncbuf = " << __cpp_lib_syncbuf << "\n\n";
#else
    std::cout << "no __cpp_lib_syncbuf: using the mutex fallback\n\n";
#endif
    std::vector<std::thread> t;
    for (const char* who : {"A", "B", "C"})
        t.emplace_back([who]{ for (int i = 0; i < 6; ++i) log_line(who, i); });
    for (auto& x : t) x.join();
}

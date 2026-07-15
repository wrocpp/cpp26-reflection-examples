// off_thread_logger.cpp  (~55 lines)
// The fastest loggers do not format on the calling thread. This mini logger has
// application threads push raw records into a queue; a single background thread
// pops them, formats, and writes. The caller pays only an enqueue, never the
// formatting or the I/O. Real libraries (spdlog, Quill, NanoLog) are this idea
// with lock-free queues and formatting deferred even further.
//
// Build (GCC 16.1):  g++ -std=c++20 -O2 -pthread off_thread_logger.cpp

#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

struct Record { std::string thread; int seq; };

class Logger {
    std::queue<Record> q;
    std::mutex m;
    std::condition_variable cv;
    bool done = false;
    std::thread worker;

    void run() {                          // the only thread that formats + writes
        for (;;) {
            std::unique_lock lk(m);
            cv.wait(lk, [this] { return done || !q.empty(); });
            while (!q.empty()) {
                const Record r = std::move(q.front());
                q.pop();
                lk.unlock();
                std::cout << "[thread " << r.thread << "] log record #" << r.seq
                          << " -- formatted on the logging thread\n";
                lk.lock();
            }
            if (done && q.empty()) return;
        }
    }

public:
    Logger() : worker([this] { run(); }) {}
    ~Logger() {
        { const std::lock_guard lk(m); done = true; }
        cv.notify_one();
        worker.join();
    }
    void log(Record r) {                  // called on app threads: just enqueue
        { const std::lock_guard lk(m); q.push(std::move(r)); }
        cv.notify_one();
    }
};

int main() {
    Logger log;
    std::vector<std::thread> t;
    for (const char* who : {"A", "B", "C"})
        t.emplace_back([&log, who] { for (int i = 0; i < 4; ++i) log.log({who, i}); });
    for (auto& x : t) x.join();
    // The Logger destructor drains the queue and joins the worker.
}

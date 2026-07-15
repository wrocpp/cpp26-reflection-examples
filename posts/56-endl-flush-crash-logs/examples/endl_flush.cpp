// endl_flush.cpp  (~35 lines)
// std::endl is '\n' PLUS a flush. The flush is the expensive part, and on a hot
// logging path it is usually wasted work. We prove endl flushes and '\n' does
// not by wrapping a streambuf that counts flushes (sync() calls).
//
// Build (GCC 16.1):  g++ -std=c++20 -O2 endl_flush.cpp

#include <iostream>
#include <streambuf>

// A filtering streambuf: forwards characters to a wrapped buffer and counts how
// many times it is flushed.
struct counting_buf : std::streambuf {
    std::streambuf* wrapped;
    int flushes = 0;
    explicit counting_buf(std::streambuf* w) : wrapped(w) {}
    int_type overflow(int_type c) override { return wrapped->sputc(static_cast<char>(c)); }
    int sync() override { ++flushes; return wrapped->pubsync(); }
};

int main() {
    counting_buf buf(std::cout.rdbuf());
    std::ostream out(&buf);

    for (int i = 0; i < 5; ++i)
        out << "line " << i << '\n';           // newline only, no flush
    const int newline_flushes = buf.flushes;

    for (int i = 0; i < 5; ++i)
        out << "line " << i << std::endl;       // endl flushes every time
    const int endl_flushes = buf.flushes - newline_flushes;

    std::cout << "\nflushes after five '\\n' lines:      " << newline_flushes << "\n";
    std::cout << "flushes after five std::endl lines:  " << endl_flushes << "\n";
}

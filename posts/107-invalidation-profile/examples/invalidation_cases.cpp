// P4418R0 invalidation profile, run on the experimental GCC branch "gcontracts-p4324-trunk"
// (Compiler Explorer, options -std=c++23 -pedantic-errors).
// The default build holds the forms the profile accepts. Each SHOW_* macro adds one case that
// the profile rejects, or that I expected it to reject.
[[profiles::enforce(std::invalidation)]];

#include <cstdio>
#include <functional>
#include <thread>
#include <utility>
#include <vector>

namespace {

constexpr int kValue = 7;
constexpr int kOther = 9;
constexpr int kStep = 1;

// ---- accepted forms ----------------------------------------------------------------------

int owner_roundtrip() {
    int* q [[owner]] = new int{kValue};
    int result = *q;
    delete q;
    return result;
}

void take(int*, int* q [[owner]]) { delete q; }

int owner_transfer() {
    int* q [[owner]] = new int{kValue};
    take(q, new int{kOther});
    int result = *q + kStep;  // take() received an alias of q and the ownership of the other int
    delete q;
    return result;
}

int read_only_call(const std::vector<int>& v) { return static_cast<int>(v.size()); }

int const_call_keeps_iterator() {
    std::vector<int> v{kStep, kValue};
    auto it = v.begin();
    int n = read_only_call(v);
    return *it + n;
}

int count_only(std::vector<int>& v [[not_invalidating]]) { return static_cast<int>(v.size()); }

int not_invalidating_keeps_iterator() {
    std::vector<int> v{kStep, kValue};
    auto it = v.begin();
    int n = count_only(v);
    return *it + n;
}

struct Reader {
    std::vector<int> v{kStep, kValue};
    [[not_invalidating]] int peek() { return v.empty() ? 0 : v[0]; }
};

int member_not_invalidating_keeps_pointer(Reader& r) {
    const int* p = &r.v[0];
    int peeked = r.peek();
    return *p + peeked;
}

auto by_value_capture() {
    int n = kValue;
    return [n] { return n; };
}

// ---- section 1 and 2: the paper's own bad examples ----------------------------------------

#ifdef SHOW_PAPER_SECTION_1
int* section_1() {
    int x = kValue;
    int* p = new int{kOther};
    delete p;
    *p = kOther;
    return &x;
}
#endif

#ifdef SHOW_DELETE_THEN_READ
int delete_then_read() {
    int* q [[owner]] = new int{kValue};
    delete q;
    return *q;
}
#endif

#ifdef SHOW_CONDITIONAL_DELETE
void section_2(int x, int y) {
    int* q [[owner]] = new int{kValue};
    delete q;
    if (0 < x) *q = kOther;
    *q = kOther;
    int* p [[owner]] = new int{kValue};
    if (0 < x) delete p;
    *p = kOther;
    if (0 < y) *p = kStep;
}
#endif

#ifdef SHOW_OWNER_THROWN_AWAY
void section_8() {
    int* p = new int{kValue};
    int* q [[owner]] = new int{kValue};
    take(p, q);
    *p = kValue;
    delete q;
}
#endif

// ---- section 3: invalidation through a call -----------------------------------------------

#ifdef SHOW_NOT_INVALIDATING_ON_FUNCTION
[[not_invalidating]] void free_function(std::vector<int>& v) { (void)v.size(); }
#endif

#ifdef SHOW_NO_ATTRIBUTE_CONTROL
int count_plain(std::vector<int>& v) { return static_cast<int>(v.size()); }
int plain_call_invalidates() {
    std::vector<int> v{kStep, kValue};
    auto it = v.begin();
    int n = count_plain(v);
    return *it + n;
}
#endif

#ifdef SHOW_MEMBER_NO_ATTRIBUTE_CONTROL
struct PlainReader {
    std::vector<int> v{kStep, kValue};
    int peek() { return v.empty() ? 0 : v[0]; }
};
int plain_member_invalidates(PlainReader& r) {
    const int* p = &r.v[0];
    int peeked = r.peek();
    return *p + peeked;
}
#endif

#ifdef SHOW_NOT_INVALIDATING_NOT_VERIFIED
void claims_not_to_invalidate(std::vector<int>& v [[not_invalidating]]) { v.push_back(kStep); }
#endif

#ifdef SHOW_NOT_INVALIDATING_MEMBER
struct Holder {
    std::vector<int> v;
    [[not_invalidating]] void grow() { v.push_back(kStep); }
};
#endif

// ---- section 6: pointers leaving their scope ----------------------------------------------

#ifdef SHOW_RETURN_LOCAL_ADDRESS
int* return_local() {
    int x = kValue;
    return &x;
}
#endif

#ifdef SHOW_STORE_LOCAL_IN_GLOBAL
int* global_ptr = nullptr;
void store_local() {
    int x = kValue;
    global_ptr = &x;
}
#endif

#ifdef SHOW_VECTOR_OF_POINTERS
std::vector<int*> pass_through(int* p) {
    std::vector<int*> v1{p};
    return v1;
}
std::vector<int*> with_local(int* p) {
    int x = kValue;
    std::vector<int*> v2{p, &x};
    return v2;
}
#endif

// ---- section 4: lambdas, this and threads -------------------------------------------------

#ifdef SHOW_LAMBDA_RETURNED
auto by_reference_capture() {
    int n = kValue;
    return [&n] { return n; };
}
#endif

#ifdef SHOW_LAMBDA_STORED
std::function<int()> keep;
void store_lambda() {
    int n = kValue;
    keep = [&n] { return n; };
}
#endif

#ifdef SHOW_THIS_STORED
struct Widget {
    std::function<int()> callback;
    int value = kStep;
    void arm() {
        callback = [this] { return value; };
    }
};
std::function<int()> escapes() {
    Widget w;
    w.arm();
    return w.callback;
}
#endif

#ifdef SHOW_THIS_DETACHED
struct Counter {
    int n = 0;
    void run_detached() {
        std::thread{[this] { n += kStep; }}.detach();
    }
};
void outlived() {
    Counter c;
    c.run_detached();
}
#endif

#ifdef SHOW_THREAD_DETACHED_REFERENCE
void detach_reference() {
    int n = 0;
    std::thread t{[&n] { n += kStep; }};
    t.detach();
}
#endif

#ifdef SHOW_JTHREAD_RETURNED
std::jthread returned_thread() {
    int n = 0;
    return std::jthread{[&n] { n += kStep; }};
}
#endif

// ---- section 7: the factory and no_dangling -----------------------------------------------

struct Window {};
struct Logger {
    void log(const char*) {}
};
struct Widget2 {
    Window* window;
    explicit Widget2(Window& w) : window(&w) {}
};

class MarkedFactory {
    Window* window_;
    Logger* logger_ = nullptr;

public:
    MarkedFactory(Window& w, Logger& l) : window_(&w), logger_(&l) {}
    Widget2 create_widget() {
        if (logger_) logger_->log("creating");
        return std::no_dangling(Widget2{*window_});
    }
};

Widget2 logger_passed_in(Window& window, Logger& log) {
    MarkedFactory factory{window, log};
    return factory.create_widget();
}

#ifdef SHOW_FACTORY_UNMARKED
class UnmarkedFactory {
    Window* window_;
    Logger* logger_ = nullptr;

public:
    UnmarkedFactory(Window& w, Logger& l) : window_(&w), logger_(&l) {}
    Widget2 create_widget() {
        if (logger_) logger_->log("creating");
        return Widget2{*window_};
    }
};
#endif

#ifdef SHOW_LOCAL_LOGGER
Widget2 local_logger(Window& window) {
    Logger log;
    MarkedFactory factory{window, log};
    return factory.create_widget();
}
#endif

#ifdef SHOW_LOCAL_LOGGER_NO_DANGLING
Widget2 local_logger_asserted(Window& window) {
    Logger log;
    MarkedFactory factory{window, log};
    return std::no_dangling(factory.create_widget());
}
#endif

#ifdef SHOW_NO_DANGLING_UNQUALIFIED
Widget2 unqualified(Window& window) { return no_dangling(Widget2{window}); }
#endif

}  // namespace

int main() {
    Reader reader;
    Window window;
    Logger logger;
    std::printf("%d %d %d %d %d %d %d\n", owner_roundtrip(), owner_transfer(), const_call_keeps_iterator(),
                not_invalidating_keeps_iterator(), member_not_invalidating_keeps_pointer(reader),
                by_value_capture()(), logger_passed_in(window, logger).window == &window);
}

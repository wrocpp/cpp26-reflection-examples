// verify: gcc-only
// A closure type in a default template argument has internal linkage. In a header class it draws
// -Wsubobject-linkage. The line marker below makes GCC treat everything after it as header code,
// which is where the warning applies.
# 1 "signal_header.h" 1
template <class Tag = decltype([] {})>
struct sig_by_closure {};

struct ByClosure {
  sig_by_closure<> a;
  sig_by_closure<> b;
};

int main() {}

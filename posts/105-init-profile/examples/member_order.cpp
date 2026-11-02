// Compile only: reading b in a's initializer is an uninitialized read in one thread.
[[profiles::enforce(std::init)]];

namespace {

constexpr int kSeed = 1;

struct Pair {
    int a;
    int b;
    Pair() : a(b), b(kSeed) {}
};

int use() {
    Pair p;
    return p.a;
}

}  // namespace

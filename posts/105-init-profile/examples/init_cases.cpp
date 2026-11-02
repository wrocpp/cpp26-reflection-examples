[[profiles::enforce(std::init)]];

#include <cstdio>
#include <memory>

namespace {

struct Point {
    int x;
    int y;
};

constexpr int kStartX = 3;
constexpr int kStartY = 4;

// The paper's loc6: an [[uninit]] scalar, assigned, then read.
int delayed_scalar() {
    int v [[uninit]];
    v = kStartX;
    return v;
}

// An [[uninit]] class object is initialized as a whole.
Point whole_object() {
    Point p [[uninit]];
    std::construct_at(&p, Point{kStartX, kStartY});
    return p;
}

// The callee constructs into memory it was told is uninitialized.
void put(Point* slot [[ref_to_uninit]]) {
    std::construct_at(slot, Point{kStartX, kStartY});
}

int use_put() {
    Point p [[uninit]];
    put(&p);
    return kStartY;
}

// Rule-level suppression with a justification. The rule name uses an underscore.
int suppressed_decl() {
    [[profiles::suppress(std::init, rule: "uninit_decl", justification: "assigned on the next line")]]
    int raw;
    raw = kStartY;
    return raw;
}

#ifdef SHOW_MEMBER_WRITE
Point memberwise() {
    Point p [[uninit]];
    p.x = kStartX;
    p.y = kStartY;
    return p;
}
#endif

#ifdef SHOW_STILL_UNINIT
int still_uninit() {
    Point p [[uninit]];
    put(&p);
    Point copy = p;
    return copy.x;
}
#endif

#ifdef SHOW_WRONG_STATE
void takes_slot(Point* slot [[ref_to_uninit]]);
void takes_value(Point* value);

void wrong_state() {
    Point fresh [[uninit]];
    takes_slot(&fresh);
    takes_value(&fresh);
    Point ready{kStartX, kStartY};
    takes_slot(&ready);
    takes_value(&ready);
}
#endif

#ifdef SHOW_STATIC_INIT
int seed();
int first = seed();
int seed() { return first; }
#endif

#ifdef SHOW_WRONG_RULE
int misspelled_rule() {
    [[profiles::suppress(std::init, rule: "uninit-decl", justification: "dash instead of underscore")]]
    int raw;
    raw = kStartY;
    return raw;
}
#endif

}  // namespace

int main() {
    std::printf("%d %d %d %d\n", delayed_scalar(), whole_object().y, use_put(), suppressed_decl());
}

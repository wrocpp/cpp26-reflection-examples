// contracts_pre_post.cpp
// One function, one pre, one post, and an ordinary if that guards the same
// condition. What the pre does when it is violated is chosen by the compile
// flag, not by the source; the if always runs.
//
//   -fcontract-evaluation-semantic=ignore    the pre is not evaluated
//   -fcontract-evaluation-semantic=observe   the pre reports and execution continues
//   -fcontract-evaluation-semantic=enforce   the pre reports and terminates (default)
//
// Compile (GCC 16.2): g++ -std=c++26 -fcontracts -fcontract-evaluation-semantic=ignore -O2 contracts_pre_post.cpp
// verify: gcc-only
// verify: gcc-options: -std=c++26 -fcontracts -fcontract-evaluation-semantic=ignore -O2

#include <cstdio>

int per_item(const int total, const int count)
    pre (count > 0)
    post (r: r <= total)
{
    if (count <= 0) {          // ordinary control flow: runs under every semantic
        return -1;
    }
    return total / count;
}

int main() {
    std::printf("per_item(100, 4) = %d\n", per_item(100, 4));
    std::fflush(stdout);
    std::printf("per_item(100, 0) = %d\n", per_item(100, 0));   // violates the pre
    std::printf("reached the end\n");
    return 0;
}

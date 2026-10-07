// contract_assert in front of the access it guards.
// GCC 16.2: g++ -std=c++26 -Wall -Wextra [-O2] [-fcontract-evaluation-semantic=S]
//           contract_assert_then_access.cpp -lstdc++exp
// Not a mint candidate: the access is out of range, so under ignore and observe
// the result depends on -O and on _GLIBCXX_ASSERTIONS, and under enforce the
// program terminates (exit 134) by design.
#include <cstddef>
#include <cstdio>
#include <vector>

int main()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::vector<int> v{10, 20, 30};
    std::size_t i = 5;

    std::printf("v[1] = %d\n", v[1]);
    contract_assert(i < v.size());
    std::printf("v[%zu] = %d\n", i, v[i]);
}

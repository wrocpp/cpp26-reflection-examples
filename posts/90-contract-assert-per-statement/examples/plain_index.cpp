// The same out-of-range access with no contract_assert.
// GCC 16.2: g++ -std=c++26 -Wall -Wextra [-O2] [-D_GLIBCXX_ASSERTIONS] plain_index.cpp
// Not a mint candidate: the access is out of range by design (exit 0 at -O2,
// exit 134 at -O0 or with _GLIBCXX_ASSERTIONS).
#include <cstddef>
#include <cstdio>
#include <vector>

int main()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::vector<int> v{10, 20, 30};
    std::size_t i = 5;

    std::printf("v[1] = %d\n", v[1]);
    std::printf("v[%zu] = %d\n", i, v[i]);
}

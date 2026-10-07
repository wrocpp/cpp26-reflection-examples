// contract_assert as a per-statement bounds check.
// GCC 16.2: g++ -std=c++26 -Wall -Wextra -fcontract-evaluation-semantic=observe
//           contract_assert_bounds.cpp -lstdc++exp
// Under observe the violation is reported and the program continues, so this
// demo exits 0. The default semantic (enforce) terminates at i = 5 with exit 134.
#include <cstddef>
#include <cstdio>
#include <vector>

int main()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::vector<int> v{10, 20, 30};

    for (std::size_t i : {1uz, 5uz}) {
        contract_assert(i < v.size());
        std::printf("i = %zu passed the contract_assert\n", i);
    }
}

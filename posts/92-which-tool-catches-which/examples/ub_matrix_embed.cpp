// The statements of the matrix that finish without a trap, each behind a contract_assert.
// GCC 16.2: g++ -std=c++26 -O2 -Wall -Wextra -fcontract-evaluation-semantic=observe
//           ub_matrix_embed.cpp -lstdc++exp
// Under observe each violation is reported and the program carries on, so this exits 0.
// The cases that trap or abort are in ub_cases_matrix.cpp and run_matrix.sh.
#include <climits>
#include <cstddef>
#include <cstdio>
#include <vector>

int main() {
  std::setvbuf(stdout, nullptr, _IONBF, 0);

  std::vector<int> v{10, 20, 30};
  v.reserve(8);  // the read below stays inside the allocation, so it does not trap
  volatile std::size_t vi = 3;
  std::size_t i = vi;
  contract_assert(i < v.size());
  [[maybe_unused]] int sink = v[i];
  std::puts("index: statement ran");

  volatile int vmax = INT_MAX;
  int x = vmax;
  contract_assert(x < INT_MAX);
  std::printf("overflow: INT_MAX + 1 = %d\n", x + 1);

  int u;  // C++26 erroneous behaviour: GCC 16 gives it the value 0
  std::printf("uninitialised: u = %d\n", u);
}

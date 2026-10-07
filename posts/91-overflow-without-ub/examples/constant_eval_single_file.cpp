// What a constant-evaluated overflow and x / 0 do. Does not compile on purpose.
// Build: g++ -std=c++26 -Wall -Wextra -DCASE=<n> -c constant_eval_single_file.cpp
#include <climits>
#include <numeric>

#ifndef CASE
#define CASE 1
#endif

#if CASE == 1
constexpr int over = INT_MAX + 1;
#elif CASE == 2
constexpr int div0 = 1 / 0;
#elif CASE == 3
constexpr int over_sat = std::saturating_add(INT_MAX, 1);   // fine: constexpr
static_assert(over_sat == INT_MAX);
constexpr int div0_sat = std::saturating_div(1, 0);         // precondition violated
#elif CASE == 4
constexpr double fdiv = 1.0 / 0.0;
#endif

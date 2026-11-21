// verify: ce-only
// The syntax of P3568R4 in C++ mode: `outer: for (...)`, `break outer;`, `continue outer;`. Clang implements the
// C2y syntax (N3355) in C++ mode behind an internal front-end flag that section 6 of the paper names. Without the
// flag, Clang 23.1 says "named 'break' is only supported in C2y"; GCC 16.2 and GCC trunk say "expected ';' before
// 'search'"; MSVC says C2146. The loops also work in constant evaluation, which a goto cannot
// (see break_label_today.cpp).
// Build: clang++ -std=c++26 -Xclang -fnamed-loops -O1   (Compiler Explorer: clang2310 and clang_trunk)
// Limit: the flag implements N3355 only. The adopted paper also lets a function reuse a label (N3658), and with
// this flag Clang rejects a second `search:` in one function with "redefinition of label". That case is not in
// this file. The program has no macros and exits 0.
#include <cstdio>

constexpr int kRows = 3;
constexpr int kCols = 3;
constexpr int kTarget = 5;
constexpr int kNotFound = -1;
constexpr int kExpectedRow = 1;
constexpr int kExpectedCol = 1;
constexpr int kExpectedRowsVisited = 2;
constexpr int kExpectedRowsWithout = 2;

struct Search {
  int row;
  int col;
  int rows_visited;
};

constexpr int value_at(int row, int col) { return row * kCols + col + 1; }

constexpr Search find_target() {
  Search result{.row = kNotFound, .col = kNotFound, .rows_visited = 0};
  search: for (int row = 0; row < kRows; ++row) {
    ++result.rows_visited;
    for (int col = 0; col < kCols; ++col) {
      if (value_at(row, col) == kTarget) {
        result.row = row;
        result.col = col;
        break search;
      }
    }
  }
  return result;
}

constexpr int rows_without_target() {
  int kept = 0;
  rows: for (int row = 0; row < kRows; ++row) {
    for (int col = 0; col < kCols; ++col) {
      if (value_at(row, col) == kTarget) continue rows;
    }
    ++kept;
  }
  return kept;
}

static_assert(find_target().row == kExpectedRow && find_target().col == kExpectedCol);
static_assert(find_target().rows_visited == kExpectedRowsVisited);
static_assert(rows_without_target() == kExpectedRowsWithout);

int main() {
  constexpr Search found = find_target();
  std::printf("found %d at row %d col %d after visiting %d rows\n", kTarget, found.row, found.col, found.rows_visited);
  std::printf("rows without %d: %d\n", kTarget, rows_without_target());
  return 0;
}

// verify: gcc-only
// What a reader writes today for `continue outer;`: count the rows of a 3 by 3 grid that do not contain a value,
// leaving the inner loop early. Three techniques, one answer: a goto to the end of the outer body, an immediately
// invoked lambda that returns, and a flag. The lambda is usable in constant evaluation (a static_assert checks it);
// the goto version is not, because a goto is not a constant expression on GCC 16.2 or Clang 23.1.
// Build: g++ -std=c++26 -O1   (Compiler Explorer: g162; also builds with clang2310 and clang_trunk)
// Prints the count for each technique. Exits 0 when all three equal the expected count. The program has no macros.
#include <cstdio>

constexpr int kRows = 3;
constexpr int kCols = 3;
constexpr int kTarget = 5;
constexpr int kExpectedRowsWithout = 2;

constexpr int value_at(int row, int col) { return row * kCols + col + 1; }

int rows_without_goto() {
  int kept = 0;
  for (int row = 0; row < kRows; ++row) {
    for (int col = 0; col < kCols; ++col) {
      if (value_at(row, col) == kTarget) goto next_row;
    }
    ++kept;
  next_row:;
  }
  return kept;
}

constexpr int rows_without_lambda() {
  int kept = 0;
  for (int row = 0; row < kRows; ++row) {
    bool lacks = [&] {
      for (int col = 0; col < kCols; ++col) {
        if (value_at(row, col) == kTarget) return false;
      }
      return true;
    }();
    if (lacks) ++kept;
  }
  return kept;
}

constexpr int rows_without_flag() {
  int kept = 0;
  for (int row = 0; row < kRows; ++row) {
    bool lacks = true;
    for (int col = 0; col < kCols && lacks; ++col) {
      if (value_at(row, col) == kTarget) lacks = false;
    }
    if (lacks) ++kept;
  }
  return kept;
}

static_assert(rows_without_lambda() == kExpectedRowsWithout);
static_assert(rows_without_flag() == kExpectedRowsWithout);

int main() {
  int by_goto = rows_without_goto();
  int by_lambda = rows_without_lambda();
  int by_flag = rows_without_flag();
  std::printf("goto: %d, lambda: %d, flag: %d\n", by_goto, by_lambda, by_flag);
  bool ok = by_goto == kExpectedRowsWithout && by_lambda == kExpectedRowsWithout && by_flag == kExpectedRowsWithout;
  return ok ? 0 : 1;
}

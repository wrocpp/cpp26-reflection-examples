// verify: ce-only
// P3568R4 gives C++ the loop-label syntax that C2y already has: a label in front of a loop, then
// `break label;` and `continue label;`. This file is C, because the C compilers accept the syntax at -std=c2y
// without a flag. The C++ counterparts are break_label_clang.cpp and break_label_today.cpp.
// Build: gcc -std=c2y -O1 break_label.c   (Compiler Explorer: cg162, cgsnapshot, cclang2310, cclang_trunk)
// Prints the cell where the target value sits in a 3 by 3 grid (found by leaving both loops at once) and the number
// of rows without that value (counted by skipping the rest of a row). Exits 0 when both results are as expected.
// The program has no macros.
#include <stdio.h>

enum {
  kRows = 3,
  kCols = 3,
  kTarget = 5,
  kNotFound = -1,
  kExpectedRow = 1,
  kExpectedCol = 1,
  kExpectedRowsVisited = 2,
  kExpectedRowsWithout = 2,
};

struct Search {
  int row;
  int col;
  int rows_visited;
};

static int value_at(int row, int col) { return row * kCols + col + 1; }

static struct Search find_target(void) {
  struct Search result = {.row = kNotFound, .col = kNotFound, .rows_visited = 0};
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

static int rows_without_target(void) {
  int kept = 0;
  rows: for (int row = 0; row < kRows; ++row) {
    for (int col = 0; col < kCols; ++col) {
      if (value_at(row, col) == kTarget) continue rows;
    }
    ++kept;
  }
  return kept;
}

int main(void) {
  struct Search found = find_target();
  int without = rows_without_target();
  printf("found %d at row %d col %d after visiting %d rows\n", kTarget, found.row, found.col, found.rows_visited);
  printf("rows without %d: %d\n", kTarget, without);
  int ok = found.row == kExpectedRow && found.col == kExpectedCol && found.rows_visited == kExpectedRowsVisited &&
           without == kExpectedRowsWithout;
  return ok ? 0 : 1;
}

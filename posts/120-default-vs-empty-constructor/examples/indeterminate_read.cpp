// verify: gcc-only
// Reading a value that no constructor set. Each mode reads one length and prints it; the mode is the first argument, so a
// default run reads nothing and exits 0. Defaulted leaves its length alone in every form except () and {}; Braced leaves
// it alone in all of them. Which reads are defined depends on the storage: an automatic variable holds an erroneous value
// in C++26 (P2795R5), a variable marked [[indeterminate]] or an object from new holds an indeterminate one (undefined).
// Build: g++ -std=c++26 -O1 indeterminate_read.cpp   (Compiler Explorer: g162, clang2210)
// Run:   ./a.out automatic | braced | marked | heap | heap-value
// The quoted outputs of the reading modes are in the README. The program has no macros.
#include <cstdio>
#include <cstring>
#include <memory>

struct Defaulted {
  const char* start;
  int length;
  const char* end;
  Defaulted() = default;
};

struct Braced {
  const char* start;
  int length;
  const char* end;
  Braced() {}
};

int read_automatic() {
  Defaulted d;
  return d.length;
}

int read_braced() {
  Braced b;
  return b.length;
}

int read_marked() {
  [[indeterminate]] int x;
  return x;
}

int read_heap() {
  const auto owned = std::make_unique_for_overwrite<Defaulted>();
  return owned->length;
}

int read_heap_value() {
  const auto owned = std::make_unique<Defaulted>();
  return owned->length;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    std::puts("modes: automatic braced marked heap heap-value (no mode: nothing is read)");
    return 0;
  }
  const char* mode = argv[1];
  int length = 0;
  if (std::strcmp(mode, "automatic") == 0) length = read_automatic();
  else if (std::strcmp(mode, "braced") == 0) length = read_braced();
  else if (std::strcmp(mode, "marked") == 0) length = read_marked();
  else if (std::strcmp(mode, "heap") == 0) length = read_heap();
  else if (std::strcmp(mode, "heap-value") == 0) length = read_heap_value();
  std::printf("%s: length %d\n", mode, length);
  return 0;
}

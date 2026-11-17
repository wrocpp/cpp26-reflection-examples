// verify: gcc-only
// Build: g++ -std=c++26 -O1 -g -pthread race.cpp                       (exit 0)
//        g++ -std=c++26 -O1 -g -fsanitize=thread -pthread race.cpp     (exit 0)
// Run:   ./a.out          readers load from an atomic<shared_ptr>; prints "atomic: reads=16000"
//        ./a.out racy     readers copy a plain shared_ptr while the writer assigns it: a data race.
//                         Under ThreadSanitizer this reports "data race" and exits with 66.
// Without ThreadSanitizer the racy path may still print a count, because a data race is undefined
// behaviour and not a guaranteed failure. Do not run "racy" outside a sanitizer except to look.
#include <atomic>
#include <cstdio>
#include <cstring>
#include <memory>
#include <thread>
#include <vector>

struct Data {
  int value;
};

constexpr int kWrites = 1000;
constexpr int kReaders = 8;
constexpr int kReadsPerReader = 2000;
constexpr const char* kRacyArg = "racy";

std::shared_ptr<Data> plain = std::make_shared<Data>(0);
std::atomic<std::shared_ptr<Data>> safe{std::make_shared<Data>(0)};

template <bool Racy>
void write_all() {
  for (int i = 0; i < kWrites; ++i) {
    if constexpr (Racy) {
      plain = std::make_shared<Data>(i);
    } else {
      safe.store(std::make_shared<Data>(i));
    }
  }
}

template <bool Racy>
long read_all() {
  long ok = 0;
  for (int i = 0; i < kReadsPerReader; ++i) {
    const std::shared_ptr<Data> p = Racy ? plain : safe.load();
    ok += p->value >= 0 ? 1 : 0;
  }
  return ok;
}

template <bool Racy>
long run() {
  std::atomic<long> total{0};
  {
    std::vector<std::jthread> threads;
    threads.emplace_back([] { write_all<Racy>(); });
    for (int r = 0; r < kReaders; ++r) {
      threads.emplace_back([&total] { total += read_all<Racy>(); });
    }
  }
  return total.load();
}

int main(int argc, char** argv) {
  const bool racy = argc > 1 && std::strcmp(argv[1], kRacyArg) == 0;
  const long reads = racy ? run<true>() : run<false>();
  std::printf("%s: reads=%ld\n", racy ? "racy" : "atomic", reads);
  return 0;
}

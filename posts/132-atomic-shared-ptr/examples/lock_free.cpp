// verify: gcc-only
// Build: g++ -std=c++26 -O2 lock_free.cpp
// Prints what std::atomic<std::shared_ptr<Data>> reports about itself: lock-freedom, size, and the
// feature-test macro. The static_asserts hold on libstdc++ (GCC 16.2). Limit: libc++ does not compile
// this file (see the README), and the MSVC result comes from a separate run recorded there.
#include <atomic>
#include <cstddef>
#include <cstdio>
#include <memory>
#include <version>

struct Data {
  int value;
};

using AtomicPtr = std::atomic<std::shared_ptr<Data>>;
using PlainPtr = std::shared_ptr<Data>;

static_assert(__cpp_lib_atomic_shared_ptr >= 201711L);
static_assert(!AtomicPtr::is_always_lock_free);
static_assert(sizeof(AtomicPtr) == sizeof(PlainPtr));

int main() {
  const AtomicPtr slot;
  std::printf("is_lock_free=%d is_always_lock_free=%d\n", static_cast<int>(slot.is_lock_free()),
              static_cast<int>(AtomicPtr::is_always_lock_free));
  std::printf("sizeof(atomic<shared_ptr>)=%zu sizeof(shared_ptr)=%zu\n", sizeof(AtomicPtr), sizeof(PlainPtr));
  std::printf("__cpp_lib_atomic_shared_ptr=%ld\n", static_cast<long>(__cpp_lib_atomic_shared_ptr));
  return 0;
}

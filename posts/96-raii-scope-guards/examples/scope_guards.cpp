// verify: gcc-only
// RAII for C APIs, part 1: the three scope guards from Library Fundamentals (P0052), as shipped in
// libstdc++'s <experimental/scope>. scope_exit runs on every exit, scope_fail only when an exception
// is unwinding, scope_success only on normal exit. The point of this file: scope_success is the one
// guard whose action may throw, because it does not run during unwinding. A header that requires every
// action to be noexcept is safe, but gives that up.
// Build: g++-16 -std=c++23 -Wall -Wextra -Werror scope_guards.cpp   (Compiler Explorer: g162)
#include <experimental/scope>
#include <cstdio>
#include <stdexcept>
#include <type_traits>

namespace sx = std::experimental;

int main() {
  // 1. which guard fires on which exit
  std::puts("normal scope:");
  {
    auto e = sx::scope_exit   ([] { std::puts("  exit    (fires)"); });
    auto f = sx::scope_fail   ([] { std::puts("  fail    (does not fire)"); });
    auto s = sx::scope_success([] { std::puts("  success (fires)"); });
  }
  std::puts("scope left by an exception:");
  try {
    auto e = sx::scope_exit   ([] { std::puts("  exit    (fires)"); });
    auto f = sx::scope_fail   ([] { std::puts("  fail    (fires)"); });
    auto s = sx::scope_success([] { std::puts("  success (does not fire)"); });
    throw std::runtime_error("boom");
  } catch (...) {
    std::puts("  caught");
  }

  // 2. the destructors differ. exit and fail can run while an exception unwinds, so their destructors
  //    are unconditionally noexcept: a throwing action would call std::terminate. success runs only on
  //    normal exit, so its destructor is conditionally noexcept and can propagate.
  std::printf("exit    dtor noexcept with a throwing action = %d\n",
              std::is_nothrow_destructible_v<decltype(sx::scope_exit   ([] {}))>);
  std::printf("fail    dtor noexcept with a throwing action = %d\n",
              std::is_nothrow_destructible_v<decltype(sx::scope_fail   ([] {}))>);
  std::printf("success dtor noexcept with a throwing action = %d\n",
              std::is_nothrow_destructible_v<decltype(sx::scope_success([] {}))>);

  // 3. so scope_success can carry an action that fails on a normal exit, e.g. a commit. The exception
  //    propagates out of the scope. A header that static_asserts every action noexcept rejects this.
  std::puts("a committing success action that throws:");
  try {
    auto commit = sx::scope_success([] {
      std::puts("  committing...");
      throw std::runtime_error("commit failed");
    });
  } catch (const std::exception& ex) {
    std::printf("  propagated: %s\n", ex.what());
  }
}

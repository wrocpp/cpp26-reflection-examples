// verify: gcc-only
// Manual mint, not the pipeline: libc++ on the clang side, plain -std=c++26.
// g++ -std=c++26   and   clang++ -std=c++26 -stdlib=libc++
#include <concepts>
#include <cstdio>
#include <functional>
#include <memory>
#include <utility>
#include <version>

// A wrapper that states its own move: the source is empty on every library.
template <class F>
struct Emptying {
  F f;
  template <class L>
    requires(!std::same_as<std::remove_cvref_t<L>, Emptying>)
  Emptying(L l) : f(std::move(l)) {}
  Emptying(Emptying&& o) noexcept : f(std::exchange(o.f, nullptr)) {}
  Emptying& operator=(Emptying&& o) noexcept {
    f = std::exchange(o.f, nullptr);
    return *this;
  }
  explicit operator bool() const { return static_cast<bool>(f); }
};

template <class F>
void probe(const char* name) {
  auto sp = std::make_shared<int>(1);
  F a = [sp] {};
  F b = std::move(a);
  std::printf("%-30s source after move: %s\n", name,
              a ? "holds a copy" : "empty");
}

int main() {
#ifdef _LIBCPP_VERSION
  std::puts("library: libc++");
#else
  std::puts("library: libstdc++");
#endif
  probe<std::function<void()>>("std::function");
  probe<Emptying<std::function<void()>>>("Emptying<std::function>");
#ifdef __cpp_lib_move_only_function
  probe<std::move_only_function<void()>>("std::move_only_function");
  probe<Emptying<std::move_only_function<void()>>>("Emptying<move_only_function>");
#else
  std::puts("std::move_only_function        not available");
#endif
#ifdef __cpp_lib_copyable_function
  probe<std::copyable_function<void()>>("std::copyable_function");
#else
  std::puts("std::copyable_function         not available");
#endif
#ifdef __cpp_lib_function_ref
  std::puts("std::function_ref              available (non-owning)");
#else
  std::puts("std::function_ref              not available");
#endif
}

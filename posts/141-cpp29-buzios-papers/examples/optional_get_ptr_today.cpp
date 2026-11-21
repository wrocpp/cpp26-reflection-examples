// verify: gcc-only
// P4189R0 (the revision I opened) proposes a member get_ptr() on optional<T> and optional<T&>: a pointer
// to the contained object, or a null pointer when the optional is empty. The paper's effect for optional<T> is
// "return has_value() ? std::addressof(val) : nullptr;". Here are free functions with that effect, a pointer-taking
// legacy function standing in for a C API, and the one-liner the paper gives as today's spelling for optional<T&>.
// Build: g++ -std=c++26 -O1   (Compiler Explorer: g162). optional<T&> is C++26 and libstdc++ 16.2 has it. None
// of the library builds I tried on 2026-11-21 has the member get_ptr: see the README for the compilers and flags.
// Prints what each spelling returns. Exits 0 when they agree. The program has no macros.
#include <cstdio>
#include <memory>
#include <optional>

constexpr int kStored = 7;

template <class T>
constexpr T* get_ptr(std::optional<T>& o) noexcept {
  return o.has_value() ? std::addressof(*o) : nullptr;
}

template <class T>
constexpr T* get_ptr(const std::optional<T&>& o) noexcept {
  return o.has_value() ? std::addressof(*o) : nullptr;
}

int legacy_api(const int* value) { return value == nullptr ? -1 : *value; }

int main() {
  std::optional<int> owned{kStored};
  std::optional<int> empty;
  int target = kStored;
  std::optional<int&> reference{target};
  const auto one_liner = reference.transform([](auto& r) { return std::addressof(r); }).value_or(nullptr);

  std::printf("optional<int>: %d, empty: %d\n", legacy_api(get_ptr(owned)), legacy_api(get_ptr(empty)));
  std::printf("optional<int&>: %d, same address as the one-liner: %s\n", legacy_api(get_ptr(reference)),
              get_ptr(reference) == one_liner ? "yes" : "no");
  bool ok = legacy_api(get_ptr(owned)) == kStored && get_ptr(empty) == nullptr && get_ptr(reference) == &target &&
            get_ptr(reference) == one_liner;
  return ok ? 0 : 1;
}

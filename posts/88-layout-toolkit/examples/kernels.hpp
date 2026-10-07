// kernels.hpp - the passes the benchmark times (and loops.cpp compiles to assembly). Each is noinline so the loop is a separate function.
//   if:   sum of price*quantity over active elements, written with an if
//   mask: the same sum, branch-free (multiply by active)
//   all:  every field is read (price*quantity + side + active), no skipping possible
#pragma once
#include <cstddef>
#include <cstdint>

namespace kernels {
using u64 = std::uint64_t;
using u8 = std::uint8_t;

template <class T> [[gnu::noinline]] u64 k_if(T const* p, std::size_t n) {
  u64 s = 0;
  for (std::size_t i = 0; i < n; ++i)
    if (p[i].active) s += p[i].price * p[i].quantity;
  return s;
}
template <class T> [[gnu::noinline]] u64 k_mask(T const* p, std::size_t n) {
  u64 s = 0;
  for (std::size_t i = 0; i < n; ++i) s += p[i].price * p[i].quantity * static_cast<u64>(p[i].active);
  return s;
}
template <class T> [[gnu::noinline]] u64 k_all(T const* p, std::size_t n) {
  u64 s = 0;
  for (std::size_t i = 0; i < n; ++i) s += p[i].price * p[i].quantity + static_cast<u64>(p[i].side) + static_cast<u64>(p[i].active);
  return s;
}
[[gnu::noinline]] inline u64 soa_if(u64 const* price, u64 const* qty, u8 const* act, std::size_t n) {
  u64 s = 0;
  for (std::size_t i = 0; i < n; ++i)
    if (act[i]) s += price[i] * qty[i];
  return s;
}
[[gnu::noinline]] inline u64 soa_mask(u64 const* price, u64 const* qty, u8 const* act, std::size_t n) {
  u64 s = 0;
  for (std::size_t i = 0; i < n; ++i) s += price[i] * qty[i] * static_cast<u64>(act[i]);
  return s;
}
[[gnu::noinline]] inline u64 soa_all(u64 const* price, u64 const* qty, u8 const* side, u8 const* act, std::size_t n) {
  u64 s = 0;
  for (std::size_t i = 0; i < n; ++i) s += price[i] * qty[i] + side[i] + act[i];
  return s;
}
template <class H, class C> [[gnu::noinline]] u64 split_all(H const* h, C const* c, std::size_t n) {
  u64 s = 0;
  for (std::size_t i = 0; i < n; ++i) s += h[i].price * h[i].quantity + static_cast<u64>(c[i].side) + static_cast<u64>(h[i].active);
  return s;
}
}  // namespace kernels

# zip(iota, r) against std::views::enumerate

Companion to the wro.cpp post "views::enumerate is sized and common, and zip(iota, r) is neither". It compares the hand-written
C++23 enumerate that Alex Dathskovsky showed on LinkedIn (`zip(iota, r)` inside a `range_adaptor_closure`; the index type there
is `std::size_t`, from a `using Index` in the screenshot) with `std::views::enumerate` (C++23, `__cpp_lib_ranges_enumerate` 202302), and
with an adaptor that bounds the `iota` by the size of the range. The example contains no macros.

- `examples/enumerate_zip_or_views.cpp`: prints the range properties, asserts them with `static_assert`, and counts allocations
  and iterator steps of `std::ranges::to<std::vector>`. Compiler Explorer (g162, `-std=c++26 -O1`): https://godbolt.org/z/71qY8qvYx

All results were taken on Compiler Explorer on 2026-11-17. The program exits 0 on g162.

## Output, g162 (libstdc++ 16, `__GLIBCXX__` 20260807), `-std=c++26 -O1`

```text
zip(iota, r)       sized=false common=false random_access=true borrowed=true index_signed=false
views::enumerate   sized=true common=true random_access=true borrowed=true index_signed=true
zip(bounded iota)  sized=true common=true random_access=true borrowed=true index_signed=true
0:10 1:20 2:30 0:10 1:20 2:30 0:10 1:20 2:30
size of views::enumerate = 3, of the bounded adaptor = 3
ranges::to<vector> over 1000 elements, allocations and iterator steps:
  zip(iota, r)      allocations 1, steps 2000
  views::enumerate  allocations 1, steps 1000
  zip(bounded iota) allocations 1, steps 1001
```

The same table and the same counts (allocations 1, steps 2000 / 1000 / 1001) came out at `-O0`, `-O1` and `-O3` on g162 and at
`-O1` on gsnapshot. The counter and the buffer pointer are `volatile` in the file: without that, GCC 16.2 at `-O1` and `-O3`
reported 0 allocations for all three, because it may assume a replaced `operator new` changes no observable state.

## Other libraries

| compiler and library | result |
|---|---|
| clang_trunk, libc++ 24.0 (`_LIBCPP_VERSION` 240000), `-std=c++26 -O1` and `-O3` | builds, same table; steps 2000 / 1000 / 1000 (the bounded adaptor takes 1000 here, not 1001) |
| clang2210, libstdc++ 15 (`__GLIBCXX__` 20250808), `-std=c++26 -O1` | the table and counts above, but the file's last `static_assert` fails: a `std::vector` cannot be built from the iterator pair of the bounded adaptor (the zip iterator has no `iterator_category` there). Without that one assertion the output is the same as on g162 |
| clang2210, libc++ 22.1 (`_LIBCPP_VERSION` 220100) | does not compile: `error: no member named 'enumerate' in namespace 'std::ranges::views'; did you mean simply 'enumerate'?` |

`__cpp_lib_ranges_enumerate` printed from a separate scratch program (it uses `#ifdef`, so it is not in the examples): 202302 on
g162, on clang2210 with libstdc++ 15 and on clang_trunk with libc++; not defined on clang2210 with libc++ 22.1.

## What the unbounded adaptor cannot do (scratch programs, not in the example)

On g162 and on clang_trunk with libc++, `-std=c++26`, a `std::vector<int>` through the hand-written `enumerate`:

- `z.size()`: `error: no matching function for call to 'std::ranges::zip_view<std::ranges::iota_view<long unsigned int, std::unreachable_sentinel_t>, std::ranges::ref_view<std::vector<int, ...> > >::size()'` (clang: `no matching member function for call to 'size'`).
- `std::vector<std::tuple<std::size_t, int>> w(z.begin(), z.end())`: g162 `no matching function for call to 'std::vector<...>::vector(..._Iterator<true>, ..._Sentinel<true>)'`; libc++ trunk `candidate template ignored: deduced conflicting types for parameter '_InputIterator' ('__iterator<true>' vs. '__sentinel<true>')`.
- `std::count_if(z.begin(), z.end(), pred)`: g162 `no matching function for call to 'count_if(..._Iterator<true>, ..._Sentinel<true>, ...)'`; clang `no matching function for call to 'count_if'`.

## Sources

- https://en.cppreference.com/w/cpp/ranges/zip_view: `size` only if every adapted range is sized; common if every view is random access and sized, among other cases.
- https://en.cppreference.com/w/cpp/ranges/enumerate_view: sized when the underlying view is; common when it is common, forward and sized; macro 202302; C++23 [range.enumerate].
- https://en.cppreference.com/w/cpp/ranges/iota_view: with one argument it is unbounded and not sized.

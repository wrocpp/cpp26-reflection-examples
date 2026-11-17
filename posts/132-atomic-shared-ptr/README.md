# What std::atomic<std::shared_ptr<T>> does

Companion to the wro.cpp post on `std::atomic<std::shared_ptr<T>>` (C++20, P0718R2). The three programs ask what the type
guarantees about waiting, what a data race on a plain `shared_ptr` looks like under ThreadSanitizer, and what one `load()`
costs. The examples contain no macros. All results were taken on Compiler Explorer on 2026-11-17.

- `examples/lock_free.cpp`: lock-freedom, size and the feature-test macro. Compiler Explorer (g162, `-std=c++26 -O2`):
  https://godbolt.org/z/3sK6zvzs1
- `examples/race.cpp`: one writer and eight readers, on an `atomic<shared_ptr>` by default and on a plain `shared_ptr` with the
  argument `racy`. Compiler Explorer (g162, `-std=c++26 -O1 -g -fsanitize=thread -pthread`): https://godbolt.org/z/rxYT8ajr8
- `examples/load_cost.cpp`: nanoseconds per `load()` and per plain copy in one thread, 9 runs per case.
  Compiler Explorer (g162, `-std=c++26 -O2 -pthread`): https://godbolt.org/z/xMMGGdqGx

All three programs exit 0 by default. `race.cpp racy` exits 66 under ThreadSanitizer.

## lock_free.cpp

```text
is_lock_free=0 is_always_lock_free=0
sizeof(atomic<shared_ptr>)=16 sizeof(shared_ptr)=16
__cpp_lib_atomic_shared_ptr=201711
```

| compiler and options | result |
|---|---|
| g162 `-std=c++26 -O2` | the output above |
| gsnapshot (GCC trunk) `-std=c++26 -O2` | the output above |
| clang2210 `-std=c++26 -O2 -stdlib=libstdc++` | the output above |
| vcpp_v19_latest_x64 `/std:c++latest /O2 /EHsc` | the output above |
| clang2210 and clang_trunk `-std=c++26 -O2 -stdlib=libc++` | does not compile: `use of undeclared identifier '__cpp_lib_atomic_shared_ptr'` |

On libc++ the earlier probe `std::atomic<std::shared_ptr<Data>> a;` failed in `__atomic/atomic.h:238` with a static assertion
that `std::is_trivially_copyable` holds (clang2210 and clang_trunk, same options). The libc++ C++20 status table lists P0718R2
without a status or a release.

## race.cpp

Default on g162 and gsnapshot with `-fsanitize=thread`: `atomic: reads=16000`, exit 0. With the argument `racy` on g162:

```text
WARNING: ThreadSanitizer: data race (pid=2)
  Read of size 8 at 0x000000407180 by thread T2:
    #0 std::__shared_ptr<Data, (__gnu_cxx::_Lock_policy)2>::__shared_ptr(std::__shared_ptr<Data, (__gnu_cxx::_Lock_policy)2> const&)
    #1 std::shared_ptr<Data>::shared_ptr(std::shared_ptr<Data> const&)
    #2 long read_all<true>() /app/example.cpp:43
```

Exit code 66. Without the sanitizer a data race is undefined behaviour: the racy path may print a count or fail, so do not
read a clean run as proof of anything.

## load_cost.cpp

Four executions of the minted program plus one earlier, nine runs each, on g162 `-std=c++26 -O2 -pthread`
(`hardware_concurrency=2` on the executor). Nanoseconds per operation, lowest and highest run over all five executions:

| case | lowest run | highest run |
|---|---|---|
| 1 thread, plain copy | 19.3 | 31.3 |
| 1 thread, `load()` | 26.9 | 283.5 |

Assembly of `a.load()` against a plain copy (g162 `-std=c++26 -O2 -pthread`, scratch file, not an example): `load()` has
three locked instructions on the multi-threaded path (`lock cmpxchg` on the lock bit, `lock xadd` on the use count, `lock sub`
to unlock), a plain copy has one (`lock xadd`).

A contended run with readers and a writer was left out: the executor has two hardware threads and the contended numbers ranged
over a factor of 18 between runs.

## Library sources read (2026-11-17)

- libstdc++ `libstdc++-v3/include/bits/shared_ptr_atomic.h`, branch `releases/gcc-16`: `_Sp_atomic::_Atomic_count::lock()`
  flips the least significant bit of the control-block pointer with a compare-exchange and spins (`__thread_relax()`) while it is
  set; `load()` takes that bit, adds a reference and clears it with `fetch_sub`.
- MSVC STL `stl/inc/atomic` (`_Locked_pointer`) and `stl/inc/memory` (`atomic<shared_ptr<_Ty>>`), branch `main`: the two low
  bits of the control-block pointer are the lock state, a waiter sets a "notify needed" state and calls `atomic::wait`.
- Headers `<hazard_pointer>` and `<rcu>`: not found on g162, gsnapshot, clang2210 and clang_trunk (libc++), and
  vcpp_v19_latest_x64, with `-std=c++26` or `/std:c++latest`. The libc++ C++26 status table lists P2530R3 and P2545R4 without
  a status.

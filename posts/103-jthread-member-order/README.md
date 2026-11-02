# A thread member declared before the data it uses

Companion to the wro.cpp post "A thread that starts before its counter exists". The bug comes from
case 2, "Timing Is Everything", of Jelle Bakker's ebook "25 C++ Cases That Will Humble You" (Jelle
Bakker is C++ Doctor on LinkedIn). Both files here are written for this post with different names and
a different loop. They are not the ebook's listing.

- `examples/meter_counter.cpp`: a `std::jthread` member that increments a `std::atomic<unsigned>`
  member. `-DWORKER_FIRST` declares the thread first (the bug). `-DGUARDED` adds a ready flag declared
  before the thread, so only the destruction end of the lifetime runs. `-DREPORT_ONLY` makes
  ThreadSanitizer print its report and exit 0.
- `examples/sampler_owner.cpp`: a `std::jthread` member that sums a `std::vector<int>` member, with a
  ready flag declared first. `-DWORKER_BEFORE_DATA` declares the thread before the vector.

## Compiler Explorer (g162, x86-64, runs live)

| Link | Options | Result |
|---|---|---|
| https://godbolt.org/z/6qEGh1Wj9 | `-std=c++20 -O1 -pthread` | thread declared last, prints `ticks > 0: yes`, exit 0 |
| https://godbolt.org/z/1j8e9nerW | `-std=c++20 -O1 -g -pthread -DWORKER_FIRST -DREPORT_ONLY -fsanitize=thread` | TSan data race at the constructor, exit 0 because of `REPORT_ONLY` |
| https://godbolt.org/z/6hrced7ce | `-std=c++20 -O1 -g -pthread -DWORKER_BEFORE_DATA -DREPORT_ONLY -fsanitize=thread` | TSan data race between `operator delete` in the destructor and the worker's read, exit 0 because of `REPORT_ONLY` |

## Measured (2026-11-02, Compiler Explorer, `-std=c++20 -O1 -g -pthread`)

Exit codes, 5 runs per cell, identical in all 5 runs of every cell. Compilers: g162 and clang_trunk (one
run each on g152 and clang 23.1.0 gave the same exit codes for the thread-last build, the TSan cells of
`meter_counter`, and the ASan and TSan cells of the buggy `sampler_owner`).

| Program and order | none | `-fsanitize=address` | `-fsanitize=thread` |
|---|---|---|---|
| `meter_counter`, thread last | 0 | 0 | 0 |
| `meter_counter -DWORKER_FIRST` | 0 | 0 | 66 |
| `meter_counter -DWORKER_FIRST -DGUARDED` | 0 | 0 | 0 |
| `sampler_owner`, thread last | 0 | 0 | 0 |
| `sampler_owner -DWORKER_BEFORE_DATA` | 0 | 1 (heap-use-after-free) | 66 |

Warnings, `-O2` on both orders: g162 `-Wall -Wextra -Weffc++` prints nothing. clang_trunk `-Weverything`
(with `-Wno-c++98-compat -Wno-c++98-compat-pedantic`) prints only `-Wpadded`, which also fires on the
correct order. clang-tidy trunk on Compiler Explorer (`clangtidytrunk`, `-checks=*`) gives 24
diagnostics on the buggy `meter_counter` and 35 on the buggy `sampler_owner`, none about member order,
lifetime or a race.

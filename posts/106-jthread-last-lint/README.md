# A reflection check that the thread member goes last

Companion to the wro.cpp post "A reflection lint: the thread member goes last". It follows "A thread
that starts before its counter exists" (`posts/103-jthread-member-order`), which uses a bug from case 2,
"Timing Is Everything", of Jelle Bakker's ebook "25 C++ Cases That Will Humble You". The class shapes
`Meter` and `Sampler` are reused from that post; the check is written for this one.

- `examples/thread_last_lint.cpp`: `lint::thread_last<T>` reflects `T`'s non-static data members with
  `std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::unchecked())`. It returns an
  empty string, or a message naming the first thread member and the first member declared after it.
  `static_assert(lint::thread_last<Meter>.empty(), lint::thread_last<Meter>)` fails with that text.
  `-DSHOW_ERROR` declares the thread of `Meter` before its counter.

## Compiler Explorer (g162, x86-64, `-std=c++26 -freflection -O1 -pthread`, runs live)

| Link | Options | Result |
|---|---|---|
| https://godbolt.org/z/s3f5181zW | as above | prints the check for three classes, exit 0 |
| https://godbolt.org/z/EcW84eona | as above plus `-DSHOW_ERROR` | `static assertion failed: thread member 'worker' is declared before 'ticks'; declare the thread last` |

Also compiled on GCC trunk (`gsnapshot`, 2026-11-02) with the same output. Not run: clang-p2996, MSVC.

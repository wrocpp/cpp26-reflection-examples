# RAII for C APIs, part 2: owning FFmpeg's libav objects

Companion code for the wro.cpp post on wrapping `AVPacket`, `AVFrame` and `AVCodecContext`.

- `examples/ffmpeg_raii.cpp` builds against real FFmpeg (9.0.2, libavcodec 63.1.102) with `g++-16`.
  Compiler Explorer has no FFmpeg library, so this one is not runnable there.
- `examples/c_api_raii.cpp` is a stand-in with the same alloc/free shape and live counters. It runs on
  Compiler Explorer (g162).

## Measured (macOS `leaks`, FFmpeg 9.0.2)

| path | manual cleanup | wrappers |
|---|---|---|
| runs to the end | 0 leaks | 0 leaks |
| stops after the packet | 1 leak, 112 bytes | 0 leaks |
| stops after the frame | 1 leak, 448 bytes | 0 leaks |
| stops after the codec opened | 33 leaks, 64720 bytes | 0 leaks |

`leaks` scans the stack conservatively. Without overwriting the dead stack before exit it reported 0
leaks for the last manual row, so the example scrubs the stack first.

A stateless deleter adds no storage: each wrapper is `sizeof(T*)` (`static_assert` in the file).

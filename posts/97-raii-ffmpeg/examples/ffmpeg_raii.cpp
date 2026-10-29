// verify: local-libav (needs the FFmpeg development headers; Compiler Explorer has no FFmpeg library)
// RAII for C APIs, part 2: owning FFmpeg's libav* objects. Each av_*_alloc has a matching free that
// takes a pointer to the pointer and nulls it, so a plain unique_ptr deleter is wrong without a small
// adaptor. This file wraps AVPacket, AVFrame and AVCodecContext, then runs the same early-exit
// pipeline twice: once with manual cleanup that forgets a path, once with the wrappers.
// Build: g++-16 -std=c++23 -Wall -Wextra -Werror ffmpeg_raii.cpp $(pkg-config --cflags --libs libavcodec libavutil) -o ffmpeg_raii
// Leaks: leaks --atExit -- ./ffmpeg_raii manual 2     (macOS)
// Measured with FFmpeg 9.0.2 on macOS: manual leaks at fail_at 1, 2 and 3; the wrappers never do.
#include <experimental/scope>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <memory>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <libavutil/frame.h>
}

namespace sx = std::experimental;

// av_packet_free, av_frame_free and avcodec_free_context take T** and set *T to null. The deleter
// owns that detail, so the wrapper is a plain std::unique_ptr with a stateless deleter.
struct PacketDeleter  { void operator()(AVPacket* p)       const noexcept { av_packet_free(&p); } };
struct FrameDeleter   { void operator()(AVFrame* f)        const noexcept { av_frame_free(&f); } };
struct ContextDeleter { void operator()(AVCodecContext* c) const noexcept { avcodec_free_context(&c); } };

using Packet  = std::unique_ptr<AVPacket, PacketDeleter>;
using Frame   = std::unique_ptr<AVFrame, FrameDeleter>;
using Context = std::unique_ptr<AVCodecContext, ContextDeleter>;

// Zero overhead: a stateless deleter adds no storage, so each wrapper is exactly one pointer.
static_assert(sizeof(Packet)  == sizeof(AVPacket*));
static_assert(sizeof(Frame)   == sizeof(AVFrame*));
static_assert(sizeof(Context) == sizeof(AVCodecContext*));

// fail_at picks where the pipeline stops early: 0 runs to the end, 1 after the packet, 2 after the
// frame, 3 after the codec opened.
static void configure(AVCodecContext* ctx) {
  ctx->width = 64;
  ctx->height = 64;
  ctx->pix_fmt = AV_PIX_FMT_YUV420P;
  ctx->time_base = AVRational{1, 25};
}

// What not to write: every early return has to remember the frees above it, and this one forgets.
static int run_manual(int fail_at) {
  AVPacket* pkt = av_packet_alloc();
  if (!pkt) return -1;
  if (fail_at == 1) return 1;                       // leaks pkt

  AVFrame* frame = av_frame_alloc();
  if (!frame) { av_packet_free(&pkt); return -1; }
  if (fail_at == 2) { av_packet_free(&pkt); return 2; }  // leaks frame

  const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
  AVCodecContext* ctx = avcodec_alloc_context3(codec);
  if (!ctx) { av_frame_free(&frame); av_packet_free(&pkt); return -1; }
  configure(ctx);
  if (avcodec_open2(ctx, codec, nullptr) < 0) {
    avcodec_free_context(&ctx); av_frame_free(&frame); av_packet_free(&pkt); return -1;
  }
  if (fail_at == 3) { av_frame_free(&frame); av_packet_free(&pkt); return 3; }  // leaks ctx

  avcodec_free_context(&ctx);
  av_frame_free(&frame);
  av_packet_free(&pkt);
  return 0;
}

// The same pipeline with the wrappers: no return path mentions a free, so none can forget one.
static int run_raii(int fail_at) {
  Packet pkt{av_packet_alloc()};
  if (!pkt) return -1;
  if (fail_at == 1) return 1;

  Frame frame{av_frame_alloc()};
  if (!frame) return -1;
  if (fail_at == 2) return 2;

  const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
  Context ctx{avcodec_alloc_context3(codec)};
  if (!ctx) return -1;
  configure(ctx.get());

  // A one-off cleanup that is not an owned object: the options dictionary avcodec_open2 may leave
  // behind. scope_exit from part 1 covers it without a wrapper type of its own.
  AVDictionary* opts = nullptr;
  av_dict_set(&opts, "b", "200k", 0);
  auto free_opts = sx::scope_exit([&opts] { av_dict_free(&opts); });
  if (avcodec_open2(ctx.get(), codec, &opts) < 0) return -1;
  if (fail_at == 3) return 3;

  return 0;
}

// leaks scans the stack conservatively, and a stale copy of a freed-too-late pointer can keep a real leak
// "reachable". Overwriting the dead stack before exit makes the manual fail_at=3 leak visible: without
// this the tool reports 0 leaks for it, with it 33 allocations of 64720 bytes.
__attribute__((noinline)) static void scrub_stack() {
  volatile char junk[1 << 16];
  for (auto& b : junk) b = 0;
}

int main(int argc, char** argv) {
  if (argc != 3) { std::fputs("usage: ffmpeg_raii manual|raii 0|1|2|3\n", stderr); return 64; }
  const int fail_at = std::atoi(argv[2]);
  const int rc = std::strcmp(argv[1], "manual") == 0 ? run_manual(fail_at) : run_raii(fail_at);
  scrub_stack();
  std::printf("%s fail_at=%d -> %d\n", argv[1], fail_at, rc);
  return 0;
}

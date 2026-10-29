// verify: gcc-only
// RAII for C APIs, part 2, stand-in edition. Compiler Explorer has no FFmpeg library, so this file
// models the shape that matters in libav: an alloc that returns a pointer, and a free that takes a
// pointer to the pointer and nulls it. Counters make an early exit visible without a leak tool. The
// real AVPacket, AVFrame and AVCodecContext version is ffmpeg_raii.cpp, built against FFmpeg 9.0.2.
// Build: g++-16 -std=c++23 -Wall -Wextra -Werror c_api_raii.cpp   (Compiler Explorer: g162)
#include <cstdio>
#include <memory>

// A stand-in for a libav object family: three kinds, one live counter each.
struct Packet  { int id; };
struct Frame   { int id; };
struct Context { int id; };

static int live_packets, live_frames, live_contexts;

Packet*  packet_alloc()  { ++live_packets;  return new Packet{1}; }
Frame*   frame_alloc()   { ++live_frames;   return new Frame{2}; }
Context* context_alloc() { ++live_contexts; return new Context{3}; }
// Like av_packet_free: takes T** and nulls the caller's pointer.
void packet_free(Packet** p)   { if (*p) { --live_packets;  delete *p; *p = nullptr; } }
void frame_free(Frame** f)     { if (*f) { --live_frames;   delete *f; *f = nullptr; } }
void context_free(Context** c) { if (*c) { --live_contexts; delete *c; *c = nullptr; } }

struct PacketDeleter  { void operator()(Packet* p)  const noexcept { packet_free(&p); } };
struct FrameDeleter   { void operator()(Frame* f)   const noexcept { frame_free(&f); } };
struct ContextDeleter { void operator()(Context* c) const noexcept { context_free(&c); } };

using PacketPtr  = std::unique_ptr<Packet,  PacketDeleter>;
using FramePtr   = std::unique_ptr<Frame,   FrameDeleter>;
using ContextPtr = std::unique_ptr<Context, ContextDeleter>;

// Zero overhead: a stateless deleter adds no storage.
static_assert(sizeof(PacketPtr)  == sizeof(Packet*));
static_assert(sizeof(FramePtr)   == sizeof(Frame*));
static_assert(sizeof(ContextPtr) == sizeof(Context*));

// fail_at picks where the pipeline stops early: 0 runs to the end.
int run_manual(int fail_at) {
  Packet* pkt = packet_alloc();
  if (fail_at == 1) return 1;                            // leaks pkt
  Frame* frame = frame_alloc();
  if (fail_at == 2) { packet_free(&pkt); return 2; }     // leaks frame
  Context* ctx = context_alloc();
  if (fail_at == 3) { frame_free(&frame); packet_free(&pkt); return 3; }  // leaks ctx
  context_free(&ctx); frame_free(&frame); packet_free(&pkt);
  return 0;
}

int run_raii(int fail_at) {
  PacketPtr pkt{packet_alloc()};
  if (fail_at == 1) return 1;
  FramePtr frame{frame_alloc()};
  if (fail_at == 2) return 2;
  ContextPtr ctx{context_alloc()};
  if (fail_at == 3) return 3;
  return 0;
}

static void report(const char* who, int fail_at, int rc) {
  std::printf("%-6s fail_at=%d -> %d, live: packets=%d frames=%d contexts=%d\n",
              who, fail_at, rc, live_packets, live_frames, live_contexts);
  live_packets = live_frames = live_contexts = 0;   // reset so each line stands alone
}

int main() {
  for (int n = 0; n <= 3; ++n) report("manual", n, run_manual(n));
  for (int n = 0; n <= 3; ++n) report("raii",   n, run_raii(n));
}

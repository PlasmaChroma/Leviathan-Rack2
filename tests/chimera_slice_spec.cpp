#include "ChimeraSlice.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>

static void need(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}
static chimera::CoreInput input(float left, float right, float sos = 0.f, float rate = 5.f/6.f) {
    chimera::CoreInput in{};
    in.live = chimera::StereoFrame{left, right};
    in.controls.sos = sos;
    in.controls.rate = rate;
    in.controls.morph = 1.f/6.f;
    return in;
}
static bool near(float a, float b, float eps = 0.0002f) {
    return std::fabs(a-b) <= eps;
}

int main() {
    chimera::Reel reel(2, 2);
    chimera::Slice slice(&reel);
    slice.setConditioning(false); // Exact unconditioned TLA reference.
    need(slice.startCurrent(), "empty Current becomes initial Append");
    need(slice.recordState() == chimera::Slice::Append, "initial Append state");
    for (int i = 0; i < 10; ++i) {
        const chimera::Slice::Output out = slice.step(input(5.f, -5.f, 1.f));
        need(near(out.audio.l, 0.f) && near(out.audio.r, 0.f), "no growing initial feedback");
        need(reel.validFrames() == static_cast<std::uint32_t>(i+1), "one frame per core tick");
        need(near(reel.readActive(i).l, 0.f), "initial capture uses post-S.O.S. bus");
    }
    slice.stopRecord();
    need(reel.markerCount() == 1 && reel.region(0).end == 10, "initial finalize region");

    // Direct live routing records regardless of Play or Vari-Speed Stop.
    slice.setPlay(false);
    slice.setInop(true);
    need(slice.startCurrent(), "Current starts on populated Reel");
    for (int i = 0; i < 12; ++i) {
        slice.step(input(float(i+1), -float(i+1), 0.f, 0.5f));
        need(slice.writerPosition() == static_cast<std::uint32_t>((i+1)%10),
             "writer advances at fixed 48 kHz and wraps independent of Play/rate");
    }
    need(near(reel.readActive(0).l, 11.f/5.f), "Current replaced frame zero on wrap");
    slice.stopRecord();

    // Append extends only the valid region, and a final marker selects it.
    need(slice.startAppend(), "Append starts");
    for (int i = 0; i < 4; ++i) {
        slice.step(input(2.f, -2.f, 0.f));
        need(reel.validFrames() == static_cast<std::uint32_t>(10+i+1),
             "valid length follows exact Append writes");
    }
    slice.stopRecord();
    need(reel.markerCount() == 2 && reel.region(0).end == 10 &&
         reel.region(1).begin == 10 && reel.region(1).end == 14,
         "Append finalization creates stable new region");
    need(near(reel.readActive(13).l, 0.4f), "Append includes final frame");

    // With a constant old Reel, Current writes the linear post-S.O.S. bus.
    chimera::Reel old(1, 1);
    for (int i = 0; i < 16; ++i)
        need(old.write(i, chimera::StereoFrame{0.25f, -0.25f}, i), "old buffer fill");
    chimera::Slice tla(&old);
    tla.setConditioning(false);
    for (int i = 0; i < 800; ++i) tla.step(input(5.f, -5.f, 0.5f));
    need(tla.startCurrent(), "TLA Current starts");
    const chimera::Slice::Output first = tla.step(input(5.f, -5.f, 0.5f));
    need(near(first.audio.l, 0.625f, 0.002f) && near(old.readActive(0).l, 0.625f, 0.002f),
         "TLA read-before-write replacement (no extra old sample)");
    tla.setInop(true);
    tla.step(input(5.f, -5.f, 0.5f));
    need(near(old.readActive(1).l, 0.625f * 47.f/48.f + 1.f/48.f, 0.003f),
         "inop transition begins with one 48-frame crossfade step");
    for (int i = 0; i < 47; ++i) tla.step(input(5.f, -5.f, 0.5f));
    need(near(old.readActive(0).l, 1.f), "inop transition reaches live input on frame 48");

    chimera::Slice conditioned;
    const float firstDc = conditioned.step(input(5.f, 5.f)).audio.l;
    float settledDc = firstDc;
    for (int i = 0; i < 48000; ++i)
        settledDc = conditioned.step(input(5.f, 5.f)).audio.l;
    need(firstDc > 0.99f && std::fabs(settledDc) < 0.0001f,
         "default input and output 5 Hz DC conditioning");
    std::puts("PASS: Chimera 48 kHz slice initial capture, Current, Append, and TLA");
}

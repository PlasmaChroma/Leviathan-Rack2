#include "ChimeraCore.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <initializer_list>
#include <new>

static bool trapAllocations = false;
static std::size_t trappedAllocations = 0;
void* operator new(std::size_t size) {
    if (trapAllocations) ++trappedAllocations;
    void* p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}
void* operator new[](std::size_t size) {
    if (trapAllocations) ++trappedAllocations;
    void* p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }

static void check(bool condition, const char* message) {
    if (!condition) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
static bool near(double a, double b, double tolerance = 1e-6) {
    return std::fabs(a-b) <= tolerance;
}

static chimera::ControlFrame controls() {
    chimera::ControlFrame c = {};
    c.rate = 5.f/6.f;
    c.morph = 1.f/6.f;
    return c;
}

int main() {
    using namespace chimera;
    using namespace chimera::profile1;
    static_assert(SOS_PARAM == 0 && SHIFT_PARAM == 11 && AUDIO_L_INPUT == 0 &&
                  SHIFT_INPUT == 12 && EOSG_OUTPUT == 3 && ERROR_LIGHT == 8,
                  "Chimera ID ordering changed");
    check(kMaxReelFrames == 8352000 && kMaxPages == 32625 && kDspProfile == 1,
          "profile capacities");

    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    check(!finite(nan) && !finite(inf) && finite(0.f) && audio(nan) == 0.f,
          "bit-level finite checks and audio sanitation");
    FiniteHold hold(0.25f);
    check(hold.take(nan) == 0.25f && hold.invalidCount == 1 &&
          hold.take(0.75f) == 0.75f && hold.take(inf) == 0.75f,
          "non-finite control holds last finite/default");
    check(near(sos(0.75, false, 0), 0.75) && near(sos(0.75, true, 4), 0.375) &&
          sos(0.75, true, -4) == 0 && sos(0.75, true, 16) == 1,
          "S.O.S. patched normalization");
    check(additive8(0.25, 1, 4) == 0.75 && additive8(0.25, -1, 4) == 0 &&
          additive5(0.25, 2.5) == 0.75, "additive CV transfer laws");
    check(controlVoltage(100.f) == 24 && controlVoltage(-100.f) == -24 &&
          additive8(0.25, 1, 100) == 1 && roundNonnegative(2.5) == 3,
          "overvoltage clamp and nonnegative half-up rounding");
    check(near(classicRate(0), -2) && near(classicRate(1.0/6), -1) &&
          classicRate(0.5) == 0 && near(classicRate(5.0/6), 1) &&
          near(classicRate(1), 2), "classic Vari-Speed anchors");
    check(near(pitchRate(1, 5.0/6, 1, -2), 0.25) &&
          near(pitchRate(1, 1.0/6, 1, 1), -2) &&
          pitchRate(1, 0.5, 1, 8) == 0 &&
          pitchRate(2, 0, 1, 8) == 0 && near(pitchRate(2, 0.75, 1, 0), 1),
          "pitch modes preserve Stop and direction");
    check(near(rate(0, 0.5, 1, 4), 2) && near(rate(0, 0.5, -1, 4), -2),
          "classic CV crosses zero before curve");

    OnePole pole;
    pole.setTau(0.001);
    check(pole.step(0.25) == 0.25, "first finite smoother observation initializes");
    check(near(pole.step(1), 0.25 + (1-std::exp(-1.0/48)) * 0.75),
          "one-pole response");
    for (int i = 0; i < 1500; ++i) pole.step(1);
    check(pole.value == 1, "smoother settles exactly");
    LinearFade fade;
    fade.initialize(0);
    fade.begin(1, 4);
    check(fade.step() == 0.25 && fade.step() == 0.5, "linear fade frame positions");
    fade.begin(0, 4);
    check(fade.step() == 0.375 && fade.step() == 0.25 &&
          fade.step() == 0.125 && fade.step() == 0, "interrupted fade restarts from current value");
    GeneMode mode;
    check(mode.observe(199.f/4095.f) && !mode.observe(200.f/4095.f) &&
          mode.observe(199.f/4095.f), "MG204 whole-splice endpoint without hysteresis");
    check(finiteGeneFrames(0, 0.5) == 0 && finiteGeneFrames(1, 0.5) == 8 &&
          finiteGeneFrames(3, 1) == 8 && finiteGeneFrames(480, 0) == 480 &&
          finiteGeneFrames(480, 1) == 8, "finite Gene output-sample floor and whole-splice span");
    check(window(1, 0, false) == 1 && window(2, 1, true) == 1 &&
          window(16, 0, false) == 0 && window(16, 15, true) == 0 &&
          window(480, 240, false) == 1 && windowEdge(4800, true) > windowEdge(4800, false),
          "window tiny cases, symmetry and smooth edge");
    check(near(morphDensity(0), 0.9) && near(morphDensity(1.0/6), 1) &&
          near(morphDensity(0.5), 2) && near(morphDensity(5.0/6), 3) &&
          near(morphDensity(1), 4) && effectiveWindow(0, 1, false) == 1 &&
          effectiveWindow(0, 1, true) == 0, "Morph and unity envelope anchors");

    const StereoFrame reel[] = {{100, -100}, {1, 10}, {2, 20}, {3, 30}, {200, -200}};
    for (std::uint32_t length = 1; length <= 3; ++length) {
        const Region region = {1, 1 + length};
        for (double coordinate : {-1024.25, -1.25, 0.0, 1.5, 8e6 + 0.25}) {
            const StereoFrame sample = readCubic(reel, region, coordinate);
            check(finite(sample.l) && finite(sample.r) && near(sample.r, 10 * sample.l),
                  "short region taps stay in stereo region");
        }
    }
    check(near(readCubic(reel, Region{1,4}, 2.0).l, 2.0) &&
          near(readCubic(reel, Region{1,4}, 2.5).l, 2.6875),
          "cubic integer and fractional positions");
    check(wrapPosition(-1, Region{1,4}) == 2 &&
          wrapPosition(1 + 3*512, Region{1,4}) == 1,
          "bounded coordinate wrapping");

    Xorshift32 random;
    check(random.next() == 1085196063u && random.next() == 2447379481u,
          "xorshift32 default sequence");
    random = Xorshift32(0);
    const double ratios[3] = {2, -3, 4};
    const OnsetChoice low = chooseOnset(random, 1, 0.1, ratios);
    check(!low.chord && low.pan == 0 && random.state == 2447379481u,
          "low Morph onset consumes two draws");
    const OnsetChoice high = chooseOnset(random, 2, 1, ratios);
    check(high.chord && high.ratio == -3 && random.state == 1701901981u,
          "high Morph onset retains signed ratio");
    double left, right;
    stereoBalance(0, left, right);
    check(near(left, 1) && near(right, 1), "center pan preserves stereo channels");
    stereoBalance(-1, left, right);
    check(near(left, std::sqrt(2.0)) && near(right, 0), "hard-left balance");
    check(finiteCompletionFrame(100, 480) == 580, "finite completion is due after last rendered frame");

    Core core;
    CoreInput input = {};
    input.controls = controls();
    input.live = StereoFrame{5.f, nan};
    input.events = kOnsetEvent;
    const CoreOutput first = core.step(input);
    check(first.frame == 0 && first.onsetCount == 1 &&
          near(first.live.l, 1) && first.live.r == 0 && core.invalidAudio() == 1 &&
          core.rateEvaluations() == 1,
          "core skeleton timestamps, events and sanitized monitoring");
    core.setSourcePosition(8000000.25);
    check(core.sourcePosition() == 8000000.25, "double source coordinate retains quarter-frame");
    input.events = 0;
    input.live = StereoFrame{0, 0};
    trapAllocations = true;
    const std::size_t before = trappedAllocations;
    for (int i = 0; i < 10000; ++i) core.step(input);
    trapAllocations = false;
    check(before == trappedAllocations && core.rateEvaluations() == 1,
          "steady core callback allocates no heap and reuses mapped rate");
    std::puts("PASS: Chimera Phase 1 controls, profile math, regions, PRNG, core harness, allocation trap");
}

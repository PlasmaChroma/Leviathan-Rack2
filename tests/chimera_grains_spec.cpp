#include "ChimeraGrains.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>

static void need(bool ok, const char* name) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", name); std::exit(1); }
}

int main() {
    chimera::Reel reel(20, 20);
    for (std::uint32_t i = 0; i < 4800; ++i)
        need(reel.write(i, chimera::StereoFrame{1.f, 1.f}, i), "constant source prepared");
    const chimera::Region region{0, 4800};
    const double gene = std::log(480.0 / 4800.0) / std::log(16.0 / 4800.0);
    need(chimera::profile1::finiteGeneFrames(4800, gene) == 480, "480-frame Gene fixture");
    const float rates[] = {0.5f, 1.f, 2.f, -1.f};
    for (int r = 0; r < 4; ++r) {
        chimera::Grains grains;
        chimera::CoreOutput c{};
        c.gene = static_cast<float>(gene);
        c.morph = 1.f/6.f;
        c.rate = rates[r];
        std::uint32_t firstCompletion = 1000, firstBoundary = 1000;
        double traveledAddress = -1;
        for (std::uint32_t frame = 0; frame < 960; ++frame) {
            if (r == 2 && frame == 200) c.rate = -0.5f;
            const chimera::Grains::Result out = grains.step(reel, region, c);
            if (out.completions && firstCompletion == 1000) firstCompletion = frame;
            if (out.primaryBoundary && firstBoundary == 1000) firstBoundary = frame;
            if (frame == 479) traveledAddress = out.primaryPosition;
            need(out.readers <= 4, "bounded musical readers");
            need(std::isfinite(out.audio.l) && std::isfinite(out.audio.r), "finite stereo audio");
            if (frame >= 120 && frame < 900)
                need(std::fabs(out.audio.l - 1.f) < 1e-5f &&
                     std::fabs(out.audio.r - 1.f) < 1e-5f,
                     "constant source unity plateau without periodic holes");
        }
        need(firstCompletion == 480 && firstBoundary == 480,
             "Gene completion and primary cycle ignore playback speed");
        const double expectedAddress[] = {240.0, 480.0, 260.0, 4319.0};
        need(std::fabs(traveledAddress - expectedAddress[r]) < 1e-5,
             "primary source excursion reflects speed despite fixed Gene duration");
        need(grains.onsetCount() == 2, "one onset per 480-frame unity cycle");
    }
    chimera::Grains overlap;
    chimera::CoreOutput c{};
    c.gene = static_cast<float>(gene);
    c.morph = 1.f;
    c.rate = 1.f;
    std::uint8_t peakReaders = 0;
    for (int frame = 0; frame < 4800; ++frame) {
        const chimera::Grains::Result out = overlap.step(reel, region, c);
        if (out.readers > peakReaders) peakReaders = out.readers;
        if (frame >= 480)
            need(out.audio.l >= 0.f && out.audio.r >= 0.f &&
                 out.audio.l < 1.6f && out.audio.r < 1.6f,
                 "common stereo normalization prevents fourfold overlap gain");
    }
    need(peakReaders == 4 && overlap.onsetCount() == 40,
         "maximum density reaches four bounded slots at fractional-hop cadence");
    chimera::Grains gap;
    c.morph = 0.f;
    bool foundGap = false;
    for (int frame = 0; frame < 550; ++frame) {
        const chimera::Grains::Result out = gap.step(reel, region, c);
        if (frame >= 480 && frame < 530 && out.readers == 0 && out.audio.l == 0.f)
            foundGap = true;
    }
    need(foundGap, "density below unity leaves an actual no-voice gap");
    chimera::Grains smooth;
    smooth.setSmooth(true);
    c.morph = 1.f/6.f;
    float smoothBoundary = 1.f;
    for (int frame = 0; frame <= 480; ++frame) {
        const chimera::Grains::Result out = smooth.step(reel, region, c);
        if (frame == 480) smoothBoundary = out.audio.l;
        if (frame < 480)
            need(std::fabs(out.audio.l - chimera::profile1::window(480, frame, true)) < 1e-5,
                 "off-audio cosine table matches reference window within profile tolerance");
    }
    need(smoothBoundary < 0.05f, "smooth window keeps audible unity-boundary dip");
    chimera::Reel discontinuous(20, 20);
    for (int i = 0; i < 4800; ++i)
        need(discontinuous.write(i, chimera::StereoFrame{i % 480 < 240 ? -1.f : 1.f,
                                                         i % 480 < 240 ? -1.f : 1.f}, i),
             "prepare discontinuous source");
    chimera::Grains declick;
    float before = 0.f, at = 0.f, after = 0.f;
    for (int frame = 0; frame <= 576; ++frame) {
        const chimera::Grains::Result out = declick.step(discontinuous, region, c);
        if (frame == 479) before = out.audio.l;
        if (frame == 480) at = out.audio.l;
        if (frame == 576) after = out.audio.l;
    }
    need(std::fabs(before - at) < 1e-5f && after < -0.99f,
         "causal unity residual begins at previous wet frame and reaches new source");
    chimera::Grains immediate;
    immediate.setImmediateTransitions(true);
    float immediateBoundary = 0.f;
    for (int frame = 0; frame <= 480; ++frame) {
        const chimera::Grains::Result out = immediate.step(discontinuous, region, c);
        if (frame == 480) immediateBoundary = out.audio.l;
    }
    need(immediateBoundary < -0.99f,
         "immediate mode bypasses the unity-boundary residual");
    chimera::Grains toggled;
    for (int frame = 0; frame <= 480; ++frame)
        toggled.step(discontinuous, region, c);
    toggled.setImmediateTransitions(true);
    need(toggled.step(discontinuous, region, c).audio.l < -0.99f,
         "enabling immediate mode clears an already-active residual");
    chimera::Grains chord;
    chord.setChordRatios(2.0, -3.0, 4.0);
    c.morph = 1.f;
    c.rate = -1.f;
    for (int frame = 0; frame <= 365; ++frame) chord.step(reel, region, c);
    need(chord.slotRatio(0) == 1.0 && chord.slotRatio(1) == 2.0 &&
         chord.slotRatio(2) == -3.0 && chord.slotRatio(3) == 4.0,
         "signed configured ratios latch in four musical slots");
    chord.setChordRatios(5.0, -6.0, 7.0);
    need(chord.slotRatio(1) == 2.0,
         "changing mcr does not retune an already sounding voice");
    for (int frame = 366; frame <= 605; ++frame) chord.step(reel, region, c);
    need(chord.slotRatio(1) == 5.0,
         "new onset adopts the newly configured signed ratio");
    std::uint8_t stressPeak = 0;
    c.gene = 0.05f;
    for (int frame = 0; frame < 500; ++frame) {
        const chimera::Grains::Result out = chord.step(reel, region, c);
        if (out.readers > stressPeak) stressPeak = out.readers;
        need(out.readers <= 8 && std::isfinite(out.audio.l),
             "duration change and slot replacement stay within eight cursors");
    }
    chimera::Reel malformed(1, 1);
    need(malformed.write(0, chimera::StereoFrame{
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::infinity()}, 0), "prepare malformed Gene source");
    chimera::Grains guarded;
    c.gene = 1.f;
    for (int frame = 0; frame < 64; ++frame) {
        const chimera::Grains::Result out = guarded.step(malformed, chimera::Region{0, 1}, c);
        need(std::isfinite(out.audio.l) && std::isfinite(out.audio.r),
             "malformed Reel samples cannot poison finite voices or residual");
    }
    const float fullRates[] = {0.5f, 1.f, 2.f, -1.f};
    const int fullCycles[] = {9600, 4800, 2400, 4800};
    for (int r = 0; r < 4; ++r) {
        chimera::Grains full;
        chimera::CoreOutput fc{};
        fc.morph = 1.f/6.f;
        fc.rate = fullRates[r];
        int firstBoundary = -1, firstCompletion = -1;
        for (int frame = 0; frame <= fullCycles[r]; ++frame) {
            const chimera::Grains::Result out = full.step(reel, region, fc, false, true);
            if (out.primaryBoundary && firstBoundary < 0) firstBoundary = frame;
            if (out.completions && firstCompletion < 0) firstCompletion = frame;
            need(out.readers <= 8 && std::isfinite(out.audio.l),
                 "full-Splice Morph scheduler remains bounded and finite");
        }
        need(firstBoundary == fullCycles[r] && firstCompletion == fullCycles[r],
             "full-Splice primary and voice expire by source travel at each speed");
    }
    chimera::Grains fullChord;
    fullChord.setChordRatios(2, -3, 4);
    chimera::CoreOutput fc{};
    fc.morph = 1.f;
    fc.rate = -1.f;
    bool sawSecondaryCompletion = false;
    for (int frame = 0; frame < 4000; ++frame) {
        const chimera::Grains::Result out = fullChord.step(reel, region, fc, false, true);
        if (out.completions && !out.primaryBoundary) sawSecondaryCompletion = true;
        need(!out.primaryBoundary && out.readers <= 8,
             "secondary full-Splice voices cannot advance primary cycle");
    }
    need(sawSecondaryCompletion,
         "signed high-Morph secondary full-Splice voice completes before primary");
    need(std::fabs(fullChord.primaryPosition() - 799.0) < 1e-5,
         "secondary chord motion never changes independent primary marker address");
    for (int length = 1; length <= 3; ++length) {
        chimera::Reel tiny(1, 1);
        for (int i = 0; i < length; ++i)
            need(tiny.write(i, chimera::StereoFrame{float(i), -float(i)}, i),
                 "prepare tiny high-rate full-Splice fixture");
        chimera::Grains extreme;
        extreme.setChordRatios(2, -3, 16);
        fc.rate = 32.f;
        for (int frame = 0; frame < 1000; ++frame) {
            const chimera::Grains::Result out = extreme.step(tiny,
                chimera::Region{0, static_cast<std::uint32_t>(length)}, fc, false, true);
            need(out.readers <= 8 && out.completions <= 4 &&
                 std::isfinite(out.audio.l) && std::isfinite(out.primaryPhase),
                 "tiny full-Splice and high ratios remain bounded without catch-up loops");
        }
    }
    chimera::Grains modeSwitch;
    fc.rate = 1.f;
    fc.morph = 1.f/6.f;
    for (int frame = 0; frame < 100; ++frame)
        modeSwitch.step(reel, region, fc, false, true);
    need(std::fabs(modeSwitch.primaryPosition() - 100.0) < 1e-5,
         "full primary reaches mode-switch source coordinate");
    fc.gene = static_cast<float>(gene);
    const chimera::Grains::Result modeTransition =
        modeSwitch.step(reel, region, fc, false, false);
    need(std::fabs(modeSwitch.primaryPosition() - 101.0) < 1e-5,
         "full-to-finite transition begins at current source coordinate");
    need(modeTransition.readers == 2 && !modeTransition.completions,
         "full-to-finite mode change retains an outgoing transition reader");
    chimera::Grains retriggered;
    chimera::CoreOutput transitionControl{};
    transitionControl.gene = static_cast<float>(gene);
    transitionControl.morph = 1.f/6.f;
    transitionControl.rate = 1.f;
    for (int frame = 0; frame < 100; ++frame)
        retriggered.step(reel, region, transitionControl);
    const chimera::Grains::Result retriggerStart =
        retriggered.step(reel, region, transitionControl, true);
    need(retriggerStart.readers == 2 && !retriggerStart.completions &&
         std::fabs(retriggerStart.audio.l - 1.f) < 1e-5f,
         "forced Play retrigger retains the old reader during its 48-frame tail");
    for (int frame = 1; frame < 48; ++frame) {
        const chimera::Grains::Result out = retriggered.step(reel, region, transitionControl);
        need(out.readers == 2 && !out.completions &&
             std::fabs(out.audio.l - 1.f) < 1e-5f,
             "forced tail crossfades without artificial completions or gain swell");
    }
    need(retriggered.step(reel, region, transitionControl).readers == 1,
         "forced transition reader retires after 48 frames");
    for (int frame = 0; frame < 120; ++frame) {
        const chimera::Grains::Result out = retriggered.step(reel, region, transitionControl, true);
        need(out.readers <= 8 && !out.completions &&
             std::isfinite(out.audio.l) && out.audio.l >= 0.f && out.audio.l <= 1.5f,
             "repeated retriggers use bounded readers and scalar emergency tails");
    }
    chimera::Reel twoRegions(40, 40);
    for (std::uint32_t i = 0; i < 9600; ++i)
        need(twoRegions.write(i, chimera::StereoFrame{i < 4800 ? 1.f : -1.f,
                                                       i < 4800 ? 1.f : -1.f}, i),
             "prepare two-region forced transition fixture");
    chimera::Grains selected;
    for (int frame = 0; frame < 100; ++frame)
        selected.step(twoRegions, chimera::Region{0, 4800}, transitionControl);
    const chimera::Grains::Result selectionStart = selected.step(twoRegions,
        chimera::Region{4800, 9600}, transitionControl);
    need(selectionStart.readers == 2 && selectionStart.audio.l > 0.99f &&
         !selectionStart.completions,
         "selection fade retains the outgoing voice's original source region");
    for (int frame = 1; frame < 48; ++frame)
        selected.step(twoRegions, chimera::Region{4800, 9600}, transitionControl);
    need(selected.step(twoRegions, chimera::Region{4800, 9600}, transitionControl).audio.l < -0.99f,
         "selection fade reaches the new region on frame 49");
    chimera::Grains immediateSelection;
    immediateSelection.setImmediateTransitions(true);
    for (int frame = 0; frame < 100; ++frame)
        immediateSelection.step(twoRegions, chimera::Region{0, 4800}, transitionControl);
    const chimera::Grains::Result immediateChange = immediateSelection.step(twoRegions,
        chimera::Region{4800, 9600}, transitionControl);
    need(immediateChange.readers == 1 && immediateChange.audio.l < -0.99f,
         "immediate Organize bypasses the extra selection transition cursor");
    chimera::Reel maximum(chimera::kMaxPages, chimera::kMaxPages);
    const chimera::Region maximumRegion{0, chimera::kMaxReelFrames};
    const double maximumGene = std::log(480.0 / chimera::kMaxReelFrames) /
        std::log(16.0 / chimera::kMaxReelFrames);
    need(chimera::profile1::finiteGeneFrames(chimera::kMaxReelFrames, maximumGene) == 480,
         "full-size Reel fixture maps to a 480-frame finite Gene");
    chimera::Grains maximumFinite, maximumFull;
    chimera::CoreOutput mc{};
    mc.gene = static_cast<float>(maximumGene);
    mc.morph = 1.f/6.f;
    mc.rate = 2.f;
    int finiteAt = -1;
    double finiteBefore = 0, fullBefore = 0;
    for (int frame = 0; frame <= 480; ++frame) {
        const chimera::Grains::Result finiteOut = maximumFinite.step(maximum, maximumRegion, mc);
        const chimera::Grains::Result fullOut = maximumFull.step(maximum, maximumRegion, mc, false, true);
        if (finiteOut.completions && finiteAt < 0) finiteAt = frame;
        if (frame == 479) { finiteBefore = finiteOut.primaryPosition; fullBefore = fullOut.primaryPosition; }
        need(!fullOut.primaryBoundary && !fullOut.completions,
             "full-Splice traversal remains far from completion beside finite Gene");
    }
    need(finiteAt == 480 && std::fabs(finiteBefore - 960.0) < 1e-5 &&
         std::fabs(fullBefore - 960.0) < 1e-5 &&
         std::fabs(maximumFinite.primaryPosition() - 2.0) < 1e-5 &&
         std::fabs(maximumFull.primaryPosition() - 962.0) < 1e-5,
         "full-size finite cycle resets while full-Splice source travel continues");
    const float anchors[] = {0.f, 1.f/6.f, 0.5f, 5.f/6.f, 1.f};
    const double densities[] = {0.9, 1.0, 2.0, 3.0, 4.0};
    for (int a = 0; a < 5; ++a) {
        chimera::Grains cadence;
        chimera::CoreOutput ac{};
        ac.gene = static_cast<float>(gene);
        ac.morph = anchors[a];
        ac.rate = 1.f;
        for (int frame = 0; frame < 480000; ++frame)
            cadence.step(reel, region, ac);
        const double expected = 1.0 + 479999.0 * densities[a] / 480.0;
        need(std::fabs(double(cadence.onsetCount()) - expected) <= 1.1,
             "ten-second Morph anchor cadence retains fractional hop phase");
    }
    chimera::Grains randomCadence;
    chimera::CoreOutput rc{};
    rc.gene = 1.f;
    rc.morph = 0.f;
    rc.rate = 1.f;
    for (int frame = 0; frame < 400; ++frame) randomCadence.step(reel, region, rc);
    rc.morph = 1.f;
    for (int frame = 0; frame < 400; ++frame) randomCadence.step(reel, region, rc);
    chimera::profile1::Xorshift32 reference;
    for (std::uint64_t i = 0; i < 2 * randomCadence.onsetCount(); ++i) reference.next();
    need(reference.state == randomCadence.randomState(),
         "every low/high Morph onset consumes exactly two deterministic PRNG draws");
    chimera::Grains latchedWindow;
    chimera::CoreOutput lc{};
    lc.gene = static_cast<float>(gene);
    lc.morph = 1.f/6.f;
    lc.rate = 1.f;
    for (int frame = 0; frame < 240; ++frame) latchedWindow.step(reel, region, lc);
    latchedWindow.setSmooth(true);
    for (int frame = 240; frame < 480; ++frame)
        need(std::fabs(latchedWindow.step(reel, region, lc).audio.l - 1.f) < 1e-5f,
             "existing Gene retains its unity/window policy after gnsm change");
    need(latchedWindow.step(reel, region, lc).audio.l < 0.05f,
         "next Gene adopts newly selected smooth window");
    std::puts("PASS: Chimera finite-Gene timing, unity plateau, and four-slot density");
}

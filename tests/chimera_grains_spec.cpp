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
    const double gene = 1564.0 / 4095.0;
    {
        chimera::Grains quality, original;
        quality.setBandlimited(true);
        quality.setChordRatios(2, 4, 16); original.setChordRatios(2, 4, 16);
        chimera::CoreOutput controls{};
        controls.gene = 1; controls.morph = 1;
        for (unsigned i = 0; i < 3000; ++i) {
            controls.rate = i < 1500 ? 32 : -32;
            controls.slide = float(i%997)/997;
            const double pm = 960*std::sin(i*0.03);
            const auto filtered = quality.step(reel, region, controls, false, false, false, pm);
            const auto reference = original.step(reel, region, controls, false, false, false, pm);
            need(std::isfinite(filtered.audio.l) && std::isfinite(filtered.audio.r) &&
                 filtered.completions == reference.completions &&
                 filtered.primaryBoundary == reference.primaryBoundary &&
                 quality.onsetCount() == original.onsetCount(),
                 "quality playback keeps grain/chord/reverse/PM scheduling unchanged");
        }
    }
    need(chimera::profile1::finiteGeneFrames(4800, gene) == 600, "600-frame Gene fixture");
    const float rates[] = {0.5f, 1.f, 2.f, -1.f};
    for (int r = 0; r < 4; ++r) {
        chimera::Grains grains;
        chimera::CoreOutput c{};
        c.gene = static_cast<float>(gene);
        c.morph = 1.f/6.f;
        c.rate = rates[r];
        std::uint32_t firstCompletion = 1000, firstBoundary = 1000;
        double traveledAddress = -1;
        for (std::uint32_t frame = 0; frame < 1200; ++frame) {
            if (r == 2 && frame == 200) c.rate = -0.5f;
            const chimera::Grains::Result out = grains.step(reel, region, c);
            if (out.completions && firstCompletion == 1000) firstCompletion = frame;
            if (out.primaryBoundary && firstBoundary == 1000) firstBoundary = frame;
            if (frame == 599) traveledAddress = out.primaryPosition;
            need(out.readers <= 4, "bounded musical readers");
            need(std::isfinite(out.audio.l) && std::isfinite(out.audio.r), "finite stereo audio");
            if (frame >= 120 && frame < 900)
                need(std::fabs(out.audio.l - 1.f) < 1e-5f &&
                     std::fabs(out.audio.r - 1.f) < 1e-5f,
                     "constant source unity plateau without periodic holes");
        }
        need(firstCompletion == 600 && firstBoundary == 600,
             "Gene completion and primary cycle ignore playback speed");
        const double expectedAddress[] = {300.0, 600.0, 200.0, 4199.0};
        need(std::fabs(traveledAddress - expectedAddress[r]) < 1e-5,
             "primary source excursion reflects speed despite fixed Gene duration");
        need(grains.onsetCount() == 2, "one onset per 600-frame unity cycle");
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
        if (frame >= 600)
            need(out.audio.l >= 0.f && out.audio.r >= 0.f &&
                 out.audio.l < 1.6f && out.audio.r < 1.6f,
                 "common stereo normalization prevents fourfold overlap gain");
    }
    need(peakReaders == 4 && overlap.onsetCount() == 32,
         "maximum density reaches four bounded slots at fractional-hop cadence");
    chimera::Reel stereo(20, 20);
    for (std::uint32_t i = 0; i < 4800; ++i)
        need(stereo.write(i, chimera::StereoFrame{1.f, -0.25f}, i),
             "prepare independent stereo channels");
    chimera::Grains centered;
    c.morph = 1.f/6.f;
    for (int frame = 0; frame < 1200; ++frame) {
        const chimera::Grains::Result out = centered.step(stereo, region, c);
        if (frame >= 120)
            need(std::fabs(out.audio.l - 1.f) < 1e-5f &&
                 std::fabs(out.audio.r + 0.25f) < 1e-5f,
                 "center pan preserves independent channels at unity gain");
    }
    double hardLeft = 0, hardRight = 0;
    chimera::profile1::stereoBalance(-1.0, hardLeft, hardRight);
    need(std::fabs(hardLeft - std::sqrt(2.0)) < 1e-12 &&
         std::fabs(hardRight) < 1e-12,
         "hard-left balance preserves left source and mutes right");
    chimera::profile1::stereoBalance(1.0, hardLeft, hardRight);
    need(std::fabs(hardLeft) < 1e-12 &&
         std::fabs(hardRight - std::sqrt(2.0)) < 1e-12,
         "hard-right balance preserves right source and mutes left");
    chimera::Grains centeredOverlap;
    c.morph = 5.f/6.f; // Three centered voices, with no random pan.
    bool sawMultipleReaders = false;
    for (int frame = 0; frame < 4800; ++frame) {
        const chimera::Grains::Result out = centeredOverlap.step(reel, region, c);
        if (out.readers >= 3) sawMultipleReaders = true;
        need(out.audio.l <= 1.f + 1e-5f && out.audio.r <= 1.f + 1e-5f,
             "common normalization bounds centered constant-source overlap");
    }
    need(sawMultipleReaders, "centered normalization fixture exercised overlap");
    chimera::Grains gap;
    c.morph = 0.f;
    bool foundGap = false;
    for (int frame = 0; frame < 680; ++frame) {
        const chimera::Grains::Result out = gap.step(reel, region, c);
        if (frame >= 600 && frame < 650 && out.readers == 0 && out.audio.l == 0.f)
            foundGap = true;
    }
    need(foundGap, "density below unity leaves an actual no-voice gap");
    chimera::Grains smooth;
    smooth.setSmooth(true);
    c.morph = 1.f/6.f;
    float smoothBoundary = 1.f;
    for (int frame = 0; frame <= 600; ++frame) {
        const chimera::Grains::Result out = smooth.step(reel, region, c);
        if (frame == 600) smoothBoundary = out.audio.l;
        if (frame < 600)
            need(std::fabs(out.audio.l - chimera::profile1::window(600, frame, true)) < 1e-5,
                 "off-audio cosine table matches reference window within profile tolerance");
    }
    need(smoothBoundary < 0.05f, "smooth window keeps audible unity-boundary dip");
    chimera::Reel discontinuous(20, 20);
    for (int i = 0; i < 4800; ++i)
        need(discontinuous.write(i, chimera::StereoFrame{i % 600 < 300 ? -1.f : 1.f,
                                                         i % 600 < 300 ? -1.f : 1.f}, i),
             "prepare discontinuous source");
    chimera::Grains declick;
    float before = 0.f, at = 0.f, after = 0.f;
    for (int frame = 0; frame <= 696; ++frame) {
        const chimera::Grains::Result out = declick.step(discontinuous, region, c);
        if (frame == 599) before = out.audio.l;
        if (frame == 600) at = out.audio.l;
        if (frame == 696) after = out.audio.l;
    }
    need(std::fabs(before - at) < 1e-5f && after < -0.99f,
         "causal unity residual begins at previous wet frame and reaches new source");
    chimera::Grains immediate;
    immediate.setImmediateTransitions(true);
    float immediateBoundary = 0.f;
    for (int frame = 0; frame <= 600; ++frame) {
        const chimera::Grains::Result out = immediate.step(discontinuous, region, c);
        if (frame == 600) immediateBoundary = out.audio.l;
    }
    need(immediateBoundary < -0.99f,
         "immediate mode bypasses the unity-boundary residual");
    chimera::Grains toggled;
    for (int frame = 0; frame <= 600; ++frame)
        toggled.step(discontinuous, region, c);
    toggled.setImmediateTransitions(true);
    need(toggled.step(discontinuous, region, c).audio.l < -0.99f,
         "enabling immediate mode clears an already-active residual");
    chimera::Grains chord;
    chord.setChordRatios(2.0, -3.0, 4.0);
    c.morph = 1.f;
    c.rate = -1.f;
    for (int frame = 0; frame <= 455; ++frame) chord.step(reel, region, c);
    need(chord.slotRatio(0) == 1.0 && chord.slotRatio(1) == 2.0 &&
         chord.slotRatio(2) == -3.0 && chord.slotRatio(3) == 4.0,
         "signed configured ratios latch in four musical slots");
    chord.setChordRatios(5.0, -6.0, 7.0);
    need(chord.slotRatio(1) == 2.0,
         "changing mcr does not retune an already sounding voice");
    for (int frame = 456; frame <= 755; ++frame) chord.step(reel, region, c);
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
    chimera::Grains stoppedFull;
    fc.morph = 1.f/6.f;
    fc.rate = 1.f;
    for (int frame = 0; frame < 137; ++frame) stoppedFull.step(reel, region, fc, false, true);
    const double heldAddress = stoppedFull.primaryPosition();
    fc.rate = 0.f;
    float heldPhase = -1.f;
    for (int frame = 0; frame < 10000; ++frame) {
        const chimera::Grains::Result out = stoppedFull.step(reel, region, fc, false, true);
        if (frame == 0) heldPhase = out.primaryPhase;
        need(!out.primaryBoundary && out.completions == 0 &&
             std::fabs(out.primaryPosition - heldAddress) < 1e-5 &&
             std::fabs(out.primaryPhase - heldPhase) < 1e-6f,
             "full-Splice Stop holds primary address/phase without new onsets");
    }
    need(stoppedFull.onsetCount() == 1,
         "full-Splice Stop cannot accumulate stationary onsets");
    chimera::Grains stoppedFinite;
    chimera::CoreOutput stopControl{};
    stopControl.gene = static_cast<float>(gene);
    stopControl.morph = 1.f/6.f;
    stopControl.rate = 0.f;
    std::uint32_t finiteStops = 0;
    for (int frame = 0; frame <= 1200; ++frame) {
        const chimera::Grains::Result out = stoppedFinite.step(reel, region, stopControl);
        finiteStops += out.primaryBoundary ? 1u : 0u;
        need(std::fabs(out.primaryPosition) < 1e-5,
             "finite Stop keeps source address while primary timer advances");
    }
    need(finiteStops == 2 && stoppedFinite.onsetCount() == 3,
         "finite Gene timer and onsets remain defined at Stop");
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
    chimera::Grains editedMetadata, unchangedMetadata;
    chimera::CoreOutput metadataControl{};
    metadataControl.gene = static_cast<float>(gene);
    metadataControl.morph = 0.7f; // Fractional onset hop; no onset is due at frame 600.
    metadataControl.rate = 1.f;
    for (int frame = 0; frame < 600; ++frame) {
        editedMetadata.step(twoRegions, region, metadataControl);
        unchangedMetadata.step(twoRegions, region, metadataControl);
    }
    const std::uint64_t onsetsBeforeEdit = editedMetadata.onsetCount();
    const chimera::Grains::Result metadataBoundary = editedMetadata.step(
        twoRegions, chimera::Region{4800, 7200}, metadataControl, false, false, true);
    const chimera::Grains::Result unchangedBoundary = unchangedMetadata.step(
        twoRegions, region, metadataControl);
    need(metadataBoundary.primaryBoundary && unchangedBoundary.primaryBoundary &&
         editedMetadata.onsetCount() == onsetsBeforeEdit &&
         editedMetadata.onsetCount() == unchangedMetadata.onsetCount() &&
         metadataBoundary.readers == unchangedBoundary.readers &&
         std::fabs(metadataBoundary.audio.l - unchangedBoundary.audio.l) < 1e-5f,
         "marker metadata handoff preserves old voices without an extra Morph onset");
    bool newRegionOnset = false;
    for (int frame = 601; frame < 750; ++frame) {
        const chimera::Grains::Result edited = editedMetadata.step(
            twoRegions, chimera::Region{4800, 7200}, metadataControl);
        const chimera::Grains::Result unchanged = unchangedMetadata.step(
            twoRegions, region, metadataControl);
        if (editedMetadata.onsetCount() == onsetsBeforeEdit) {
            need(edited.readers == unchanged.readers &&
                 std::fabs(edited.audio.l - unchanged.audio.l) < 1e-5f,
                 "old Morph voices retain captured source through marker edit");
        }
        else if (edited.audio.l < unchanged.audio.l - 0.01f) newRegionOnset = true;
    }
    need(newRegionOnset && editedMetadata.onsetCount() == unchangedMetadata.onsetCount(),
         "next scheduled Morph onset adopts refreshed region without changing cadence");
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
    const double maximumGene = 1.0;
    need(chimera::profile1::finiteGeneFrames(chimera::kMaxReelFrames, maximumGene) == 1383,
         "full-size Reel fixture maps to a 1383-frame finite Gene");
    chimera::Grains maximumFinite, maximumFull;
    chimera::CoreOutput mc{};
    mc.gene = static_cast<float>(maximumGene);
    mc.morph = 1.f/6.f;
    mc.rate = 2.f;
    int finiteAt = -1;
    double finiteBefore = 0, fullBefore = 0;
    for (int frame = 0; frame <= 1383; ++frame) {
        const chimera::Grains::Result finiteOut = maximumFinite.step(maximum, maximumRegion, mc);
        const chimera::Grains::Result fullOut = maximumFull.step(maximum, maximumRegion, mc, false, true);
        if (finiteOut.completions && finiteAt < 0) finiteAt = frame;
        if (frame == 1382) { finiteBefore = finiteOut.primaryPosition; fullBefore = fullOut.primaryPosition; }
        need(!fullOut.primaryBoundary && !fullOut.completions,
             "full-Splice traversal remains far from completion beside finite Gene");
    }
    need(finiteAt == 1383 && std::fabs(finiteBefore - 2766.0) < 1e-5 &&
         std::fabs(fullBefore - 2766.0) < 1e-5 &&
         std::fabs(maximumFinite.primaryPosition() - 2.0) < 1e-5 &&
         std::fabs(maximumFull.primaryPosition() - 2768.0) < 1e-5,
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
        const double expected = 1.0 + 479999.0 * densities[a] / 600.0;
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
    for (int frame = 240; frame < 600; ++frame)
        need(std::fabs(latchedWindow.step(reel, region, lc).audio.l - 1.f) < 1e-5f,
             "existing Gene retains its unity/window policy after gnsm change");
    need(latchedWindow.step(reel, region, lc).audio.l < 0.05f,
         "next Gene adopts newly selected smooth window");
    for (std::uint32_t length = 1; length <= 3; ++length) {
        chimera::Reel tiny(1, 1);
        for (std::uint32_t frame = 0; frame < length; ++frame)
            need(tiny.write(frame, chimera::StereoFrame{float(frame + 1), -float(frame + 1)}, frame),
                 "prepare tiny-region PM stress fixture");
        chimera::Grains pmStress;
        pmStress.setChordRatios(16.0, -16.0, 16.0);
        chimera::CoreOutput pc{};
        pc.rate = 32.f;
        pc.morph = 1.f;
        for (int frame = 0; frame < 100; ++frame) {
            const chimera::Grains::Result out = pmStress.step(
                tiny, chimera::Region{0, length}, pc, false, true, false,
                frame & 1 ? 960.0 : -960.0);
            need(out.readers <= 8 && std::isfinite(out.audio.l) && std::isfinite(out.audio.r),
                 "tiny full-Splice high-rate PM stays bounded and finite");
        }
        pc.rate = 0.f;
        const std::uint64_t beforeStop = pmStress.onsetCount();
        for (int frame = 0; frame < 100; ++frame) {
            const chimera::Grains::Result out = pmStress.step(
                tiny, chimera::Region{0, length}, pc, false, true, false,
                frame & 1 ? 960.0 : -960.0);
            need(out.readers <= 8 && std::isfinite(out.audio.l) && std::isfinite(out.audio.r) &&
                 (frame == 0 || !out.primaryBoundary),
                 "tiny full-Splice PM at Stop has no extra primary wrap or invalid read");
        }
        need(pmStress.onsetCount() == beforeStop,
             "PM at Stop does not accumulate new full-Splice onsets");
    }
    chimera::Reel pmGradient(4, 4);
    for (std::uint32_t frame = 0; frame < 1000; ++frame)
        need(pmGradient.write(frame, chimera::StereoFrame{frame / 1000.f, -frame / 1000.f}, frame),
             "prepare exact PM displacement fixture");
    chimera::Grains displaced;
    chimera::CoreOutput stopped{};
    stopped.morph = 1.f/6.f;
    stopped.rate = 0.f;
    const chimera::Region gradientRegion{0, 1000};
    const chimera::Grains::Result positive = displaced.step(
        pmGradient, gradientRegion, stopped, false, true, false, 96.0);
    const chimera::Grains::Result negative = displaced.step(
        pmGradient, gradientRegion, stopped, false, true, false, -96.0);
    need(std::fabs(positive.audio.l - 0.096f) < 1e-5f &&
         std::fabs(negative.audio.l - 0.904f) < 1e-5f &&
         positive.markerPosition == 0.0 && negative.markerPosition == 0.0 &&
         displaced.primaryPosition() == 0.0,
         "PM shifts read taps in both directions without moving primary or marker cursor");
    chimera::Reel pmOriginal(20, 20), pmShifted(20, 20);
    for (std::uint32_t frame = 0; frame < 4800; ++frame) {
        const std::uint32_t shifted = (frame + 96) % 4800;
        const auto sample = [](std::uint32_t address) {
            return chimera::StereoFrame{
                float((address * 37) % 997) / 997.f - 0.5f,
                float((address * 71) % 991) / 991.f - 0.5f};
        };
        need(pmOriginal.write(frame, sample(frame), frame) &&
             pmShifted.write(frame, sample(shifted), frame),
             "prepare shifted stereo source for multi-reader PM");
    }
    chimera::Grains modulatedReaders, shiftedSourceReaders;
    chimera::CoreOutput multiPm{};
    multiPm.gene = static_cast<float>(gene);
    multiPm.morph = 1.f;
    multiPm.rate = 1.25f;
    std::uint8_t pmPeakReaders = 0;
    for (int frame = 0; frame < 2400; ++frame) {
        const chimera::Grains::Result modulated = modulatedReaders.step(
            pmOriginal, region, multiPm, false, false, false, 96.0);
        const chimera::Grains::Result shiftedSource = shiftedSourceReaders.step(
            pmShifted, region, multiPm);
        if (modulated.readers > pmPeakReaders) pmPeakReaders = modulated.readers;
        need(std::fabs(modulated.audio.l - shiftedSource.audio.l) < 1e-5f &&
             std::fabs(modulated.audio.r - shiftedSource.audio.r) < 1e-5f &&
             modulated.primaryPosition == shiftedSource.primaryPosition,
             "PM applies the same 96-frame displacement to every moving reader");
    }
    need(pmPeakReaders >= 4, "multi-reader PM fixture exercises high Morph overlap");
    // Clock advances the source origin without changing finite Gene lifetime.
    chimera::CoreOutput clockControl{};
    clockControl.gene = static_cast<float>(gene);
    clockControl.morph = 1.f/6.f;
    clockControl.rate = 1.f;
    chimera::Grains shifted;
    shifted.step(reel, region, clockControl, false, false, false, 0.0,
                 chimera::Grains::ClockDrive(1));
    const std::uint64_t beforeShift = shifted.onsetCount();
    const chimera::Grains::Result shiftEdge = shifted.step(
        reel, region, clockControl, false, false, false, 0.0,
        chimera::Grains::ClockDrive(1, true));
    need(beforeShift == 1 && shifted.onsetCount() == 2 &&
         std::fabs(shiftEdge.markerPosition - 600.0) < 1e-6 &&
         !shiftEdge.primaryBoundary && !shiftEdge.completions,
         "Gene Shift forces one onset at the stepped origin without fabricated EOSG");
    const chimera::Grains::Result disconnected = shifted.step(
        reel, region, clockControl, false, false, false, 0.0,
        chimera::Grains::ClockDrive());
    need(std::fabs(disconnected.markerPosition) < 1e-6 &&
         std::fabs(shifted.trajectoryOffset()) < 1e-6,
         "Clock disconnect restores the free-running Slide origin");
    chimera::Reel clockTone(20, 20);
    for (std::uint32_t frame = 0; frame < 4800; ++frame) {
        const float sample = static_cast<float>(std::sin(2.0 * chimera::profile1::kPi *
                                                         261.626 * frame / 48000.0));
        need(clockTone.write(frame, chimera::StereoFrame{sample, sample}, frame),
             "prepare phase-misaligned Clock transition fixture");
    }
    chimera::Grains smoothClock;
    clockControl.rate = 1.f;
    double lastClockSample = 0.0, largestClockStep = 0.0;
    for (int frame = 0; frame < 350; ++frame) {
        const bool edge = frame == 100 || frame == 200 || frame == 300;
        const chimera::Grains::Result out = smoothClock.step(
            clockTone, region, clockControl, false, false, false, 0.0,
            chimera::Grains::ClockDrive(1, edge));
        if (frame >= 100) {
            const double change = std::fabs(out.audio.l - lastClockSample);
            if (frame >= 100 && change > largestClockStep) largestClockStep = change;
        }
        lastClockSample = out.audio.l;
    }
    need(largestClockStep < 0.15,
         "forced Gene Shift crossfades phase-misaligned tones across all voice slots");
    chimera::Grains coincidentShift;
    coincidentShift.step(reel, region, clockControl, false, false, false, 0.0,
                         chimera::Grains::ClockDrive(1));
    for (int frame = 1; frame < 600; ++frame)
        coincidentShift.step(reel, region, clockControl, false, false, false, 0.0,
                             chimera::Grains::ClockDrive(1));
    const std::uint64_t beforeCoincident = coincidentShift.onsetCount();
    const chimera::Grains::Result coincidentEdge = coincidentShift.step(
        reel, region, clockControl, false, false, false, 0.0,
        chimera::Grains::ClockDrive(1, true));
    need(beforeCoincident == 1 && coincidentShift.onsetCount() == 2 &&
         coincidentEdge.completions == 1 && !coincidentEdge.primaryBoundary,
         "coincident natural completion and Clock Shift keep one completion and one onset");
    chimera::Grains reverseShift;
    clockControl.rate = -1.f;
    reverseShift.step(reel, region, clockControl, false, false, false, 0.0,
                      chimera::Grains::ClockDrive(1));
    const chimera::Grains::Result reverseEdge = reverseShift.step(
        reel, region, clockControl, false, false, false, 0.0,
        chimera::Grains::ClockDrive(1, true));
    need(std::fabs(reverseEdge.markerPosition - 4199.0) < 1e-6,
         "Gene Shift follows reverse base-rate direction");
    chimera::Grains fullShift;
    clockControl.rate = 1.f;
    fullShift.step(reel, region, clockControl, false, true, false, 0.0,
                   chimera::Grains::ClockDrive(1));
    const chimera::Grains::Result fullEdge = fullShift.step(
        reel, region, clockControl, false, true, false, 0.0,
        chimera::Grains::ClockDrive(1, true));
    need(fullShift.onsetCount() == 2 && std::fabs(fullEdge.markerPosition) < 1e-6,
         "full-Splice Clock step wraps and retriggers the same region");
    for (int rateCase = 0; rateCase < 2; ++rateCase) {
        chimera::Grains stretched;
        clockControl.rate = rateCase ? 2.f : 0.5f;
        stretched.step(reel, region, clockControl, false, false, false, 0.0,
                       chimera::Grains::ClockDrive(2, true));
        for (int frame = 0; frame < 99; ++frame)
            stretched.step(reel, region, clockControl, false, false, false, 0.0,
                           chimera::Grains::ClockDrive(2));
        need(stretched.trajectoryOffset() == 0.0,
             "Stretch holds source trajectory before its second edge");
        const std::uint64_t beforePeriod = stretched.onsetCount();
        stretched.step(reel, region, clockControl, false, false, false, 0.0,
                       chimera::Grains::ClockDrive(2, true, 2400));
        need(stretched.onsetCount() == beforePeriod &&
             std::fabs(stretched.trajectoryOffset() - 600.25) < 1e-5,
             "Stretch edge reanchors source without forcing a musical onset");
        for (int frame = 0; frame < 99; ++frame)
            stretched.step(reel, region, clockControl, false, false, false, 0.0,
                           chimera::Grains::ClockDrive(2, false, 2400));
        need(std::fabs(stretched.trajectoryOffset() - 625.0) < 1e-4,
             "Stretch source speed is Gene length divided by Clock period, independent of pitch");
        clockControl.rate = rateCase ? -2.f : -0.5f;
        for (int frame = 0; frame < 10; ++frame)
            stretched.step(reel, region, clockControl, false, false, false, 0.0,
                           chimera::Grains::ClockDrive(2, false, 2400));
        need(std::fabs(stretched.trajectoryOffset() - 622.5) < 1e-4,
             "Stretch reverses source trajectory when base pitch direction reverses");
        for (int frame = 0; frame < 48000; ++frame)
            stretched.step(reel, region, clockControl, false, false, false, 0.0,
                           chimera::Grains::ClockDrive(2, false, 2400, true));
        need(std::fabs(stretched.trajectoryOffset() - 622.5) < 1e-4,
             "stopped Stretch Clock freezes source trajectory");
    }
    std::puts("PASS: Chimera finite-Gene timing, unity plateau, and four-slot density");
}

#include "ChimeraSlice.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>

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
    {
        chimera::Slice decay;
        decay.step(input(1.f, -1.f));
        chimera::Slice::Output quiet{};
        for (unsigned i = 0; i < 480000; ++i) quiet = decay.step(input(0.f, 0.f));
        need(quiet.audio.l == 0.f && quiet.audio.r == 0.f && quiet.cv == 0.f,
             "DC blockers and envelope settle to exact zero after silence");
    }
    {
        chimera::Reel markers(4, 4);
        for (unsigned i = 0; i < 1000; ++i) markers.write(i, {0.25f, 0.25f}, i);
        markers.addMarker(500);
        const auto markerId = markers.markerId(1);
        const auto audioRevision = markers.audioRevision();
        chimera::Slice liveEdit(&markers);
        liveEdit.setConditioning(false);
        for (unsigned i = 0; i < 100; ++i) liveEdit.step(input(0, 0, 1));
        const auto onsets = liveEdit.onsetCount();
        const double position = liveEdit.playbackPosition();
        markers.beginSnapshot(100);
        while (!markers.readyForWorker()) markers.maintenanceTick();
        need(liveEdit.editMarker(1, 600, false) && markers.region(1).begin == 600 &&
             markers.markerId(1) == markerId && markers.audioRevision() == audioRevision,
             "marker-only move preserves audio and marker identity");
        need(markers.snapshotMetadata().markers[1].frame == 500,
             "metadata edit never changes an already leased snapshot cut");
        liveEdit.step(input(0, 0, 1));
        need(liveEdit.onsetCount() == onsets && near(float(liveEdit.playbackPosition()), float(position + 1)),
             "marker edit preserves active voices and playback cursor");
        need(liveEdit.editMarker(0, 0, false, 1) && markers.region(1).begin == 500 &&
             liveEdit.editMarker(0, 0, false, 2) && markers.region(1).begin == 600,
             "marker table Undo/Redo swaps metadata without audio writes");
        need(!liveEdit.editMarker(0, 12, false) && liveEdit.markerHistoryState() == 1,
             "invalid marker edit retains previous history");
        need(liveEdit.selectRegion(1) && liveEdit.editMarker(1, 0, true) &&
             markers.markerCount() == 1 && liveEdit.currentRegion() == 0,
             "removing selected marker remaps to merged splice");
        need(liveEdit.editMarker(0, 0, false, 1) && markers.markerCount() == 2 &&
             markers.markerId(1) == markerId && markers.audioRevision() == audioRevision,
             "Undo restores removed stable marker without altering audio");
        markers.addMarker(200);
        need(!liveEdit.editMarker(0, 0, false, 2), "intervening marker change fences stale history");
    }
    for (float extreme : {1e30f, -1e30f, std::numeric_limits<float>::max()}) {
        chimera::Slice bounded;
        bounded.setPmEnabled(true);
        auto impulse = input(extreme, -extreme);
        impulse.pmRightConnected = true;
        auto out = bounded.step(impulse);
        auto silence = input(0.f, 0.f);
        silence.pmRightConnected = true;
        for (unsigned i = 0; i < 480000; ++i) {
            out = bounded.step(silence);
            need(std::isfinite(out.audio.l) && std::isfinite(out.audio.r) && std::isfinite(out.cv),
                 "extreme finite input cannot poison audio or energy state");
        }
        need(out.cv < 0.001f && bounded.pmActive() && bounded.overloaded(),
             "energy and PM detectors recover after extreme finite input");
    }
    chimera::Slice gainSlice(nullptr);
    gainSlice.setConditioning(false);
    gainSlice.setInputGain(2);
    const float firstGain = gainSlice.step(input(5.f, -5.f)).audio.l;
    for (int i = 1; i < 240; ++i) gainSlice.step(input(5.f, -5.f));
    need(near(firstGain, 1.f + (1.99526231f - 1.f) / 240.f) &&
         near(gainSlice.inputGainMultiplier(), 1.99526231f),
         "+6 dB input gain reaches its target on frame 240");
    gainSlice.setInputGain(3);
    for (int i = 0; i < 120; ++i) gainSlice.step(input(5.f, -5.f));
    const float interrupted = gainSlice.inputGainMultiplier();
    gainSlice.setInputGain(0);
    const float resumed = gainSlice.step(input(5.f, -5.f)).audio.l;
    need(near(resumed, interrupted + (0.70794578f - interrupted) / 240.f) &&
         gainSlice.inputGain() == 0,
         "interrupted gain fade restarts from its current multiplier");
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
    chimera::Slice latched(&reel);
    latched.setConditioning(false);
    latched.setInop(true);
    need(latched.selectRegion(0) && latched.startCurrent() &&
         latched.selectRegion(1), "change audible selection after Current begins");
    latched.step(input(4.f, -4.f));
    need(latched.writerPosition() == 1 && near(reel.readActive(0).l, 0.8f) &&
         !latched.selectRegion(2),
         "Current writer keeps its latched destination when playback selection changes");
    latched.stopRecord();
    chimera::Reel transitionReel(40, 40);
    for (std::uint32_t i = 0; i < 9600; ++i)
        need(transitionReel.write(i, chimera::StereoFrame{i < 4800 ? 1.f : -1.f,
                                                           i < 4800 ? 1.f : -1.f}, i),
             "prepare slice selection transition fixture");
    need(transitionReel.addMarker(4800), "split slice transition fixture");
    chimera::Slice selectedSlice(&transitionReel);
    selectedSlice.setConditioning(false);
    for (int frame = 0; frame < 200; ++frame)
        selectedSlice.step(input(0.f, 0.f, 1.f));
    need(selectedSlice.selectRegion(1), "select second audible region");
    const chimera::Slice::Output selectedFirst = selectedSlice.step(input(0.f, 0.f, 1.f));
    need(selectedFirst.audio.l > 0.9f && !selectedFirst.eosg,
         "Slice selection preserves old-region audio on the first transition frame");
    for (int frame = 1; frame < 48; ++frame)
        selectedSlice.step(input(0.f, 0.f, 1.f));
    need(selectedSlice.step(input(0.f, 0.f, 1.f)).audio.l < -0.9f,
         "Slice selection reaches the new region after the bounded reader tail");
    const std::uint16_t markersBeforeEmptyAppend = reel.markerCount();
    need(slice.startAppend(), "empty Append may be armed");
    slice.stopRecord();
    need(reel.markerCount() == markersBeforeEmptyAppend,
         "zero-frame Append leaves no empty region");

    chimera::Reel lastFrame(1, 1);
    for (std::uint32_t i = 0; i < lastFrame.capacityFrames() - 1; ++i)
        need(lastFrame.write(i, chimera::StereoFrame{0.1f, -0.1f}, i),
             "prepare nearly full Reel");
    chimera::Slice finalAppend(&lastFrame);
    need(finalAppend.startAppend(), "Append admits final available frame");
    const chimera::Slice::Output capacity = finalAppend.step(input(5.f, -5.f));
    need(capacity.full && !capacity.recording &&
         lastFrame.validFrames() == lastFrame.capacityFrames() &&
         lastFrame.markerCount() == 2 &&
         lastFrame.region(1).begin == lastFrame.capacityFrames() - 1 &&
         !finalAppend.startAppend() && finalAppend.startCurrent(),
         "last Append frame finalizes once; full Reel still allows Current");
    chimera::Reel markerFull(2, 2);
    for (std::uint32_t i = 0; i < 301; ++i)
        need(markerFull.write(i, chimera::StereoFrame{0.f, 0.f}, i),
             "prepare marker-capacity fixture");
    for (std::uint32_t i = 1; i < chimera::kMaxSplices; ++i)
        need(markerFull.addMarker(i), "fill marker table without consuming start twice");
    chimera::Slice capped(&markerFull);
    need(markerFull.markerCount() == chimera::kMaxSplices &&
         !capped.startAppend() && capped.startCurrent(),
         "marker cap rejects Append while Current recording remains available");
    chimera::Reel nearMarkerCap(2, 2);
    for (std::uint32_t i = 0; i < 301; ++i)
        need(nearMarkerCap.write(i, chimera::StereoFrame{0.f, 0.f}, i),
             "prepare near-cap marker fixture");
    for (std::uint32_t i = 1; i < 298; ++i)
        need(nearMarkerCap.addMarker(i), "fill to 298 regions");
    chimera::Slice lastMarkers(&nearMarkerCap);
    need(nearMarkerCap.markerCount() == 298 && lastMarkers.startAppend(),
         "Append allowed at 298 regions");
    lastMarkers.step(input(0.f, 0.f));
    lastMarkers.stopRecord();
    need(nearMarkerCap.markerCount() == 299 && lastMarkers.startAppend(),
         "first near-cap Append creates region 299");
    lastMarkers.step(input(0.f, 0.f));
    lastMarkers.stopRecord();
    need(nearMarkerCap.markerCount() == 300 && !lastMarkers.startAppend(),
         "second near-cap Append uses final region and blocks another");

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
    chimera::Reel oneFrame(1, 1);
    need(oneFrame.write(0, chimera::StereoFrame{0.25f, 0.25f}, 0),
         "prepare one-frame feedback fixture");
    chimera::Slice oneFrameTla(&oneFrame);
    oneFrameTla.setConditioning(false);
    for (int i = 0; i < 800; ++i) oneFrameTla.step(input(5.f, 5.f, 0.5f));
    need(oneFrameTla.startCurrent(), "record one-frame region");
    need(near(oneFrameTla.step(input(5.f, 5.f, 0.5f)).audio.l, 0.625f, 0.002f) &&
         near(oneFrame.readActive(0).l, 0.625f, 0.002f),
         "one-frame TLA reads old sample before its write");
    need(near(oneFrameTla.step(input(5.f, 5.f, 0.5f)).audio.l, 0.8125f, 0.002f),
         "next one-frame recurrence reads prior write, not same-frame write");

    chimera::Slice conditioned;
    const float firstDc = conditioned.step(input(5.f, 5.f)).audio.l;
    float settledDc = firstDc;
    for (int i = 0; i < 48000; ++i)
        settledDc = conditioned.step(input(5.f, 5.f)).audio.l;
    need(firstDc > 0.99f && std::fabs(settledDc) < 0.0001f,
         "default input and output 5 Hz DC conditioning");

    chimera::Reel tagged(1, 1);
    for (int i = 0; i < 16; ++i)
        need(tagged.write(i, chimera::StereoFrame{float(i), float(i)}, i), "tagged playback fill");
    chimera::Slice playback(&tagged);
    playback.setConditioning(false);
    for (int i = 0; i < 1000; ++i) playback.step(input(0.f, 0.f, 1.f));
    playback.setPlay(false);
    for (int i = 0; i < 48; ++i) playback.step(input(0.f, 0.f, 1.f));
    playback.setPlay(true);
    need(near(playback.step(input(0.f, 0.f, 1.f)).audio.l, 0.f),
         "Play rise restarts forward full-Splice at origin");
    playback.setPlay(false);
    for (int i = 0; i < 1000; ++i) playback.step(input(0.f, 0.f, 1.f, 1.f/6.f));
    playback.setPlay(true);
    need(near(playback.step(input(0.f, 0.f, 1.f, 1.f/6.f)).audio.l, 15.f/48.f, 0.002f),
         "Play rise restarts reverse full-Splice at wrapped origin minus one");

    const float rateKnobs[] = {5.f/6.f, 1.f};
    const int expectedTravelFrames[] = {16, 8};
    for (int caseIndex = 0; caseIndex < 2; ++caseIndex) {
        chimera::Slice cycle(&tagged);
        cycle.setConditioning(false);
        cycle.setPlay(false);
        const float knob = rateKnobs[caseIndex];
        for (int i = 0; i < 2000; ++i) cycle.step(input(0.f, 0.f, 1.f, knob));
        cycle.setPlay(true);
        for (int i = 0; i < expectedTravelFrames[caseIndex]; ++i)
            need(!cycle.step(input(0.f, 0.f, 1.f, knob)).naturalBoundary,
                 "full-Splice completion not early");
        need(cycle.step(input(0.f, 0.f, 1.f, knob)).naturalBoundary,
             "full-Splice completion reported at next frame entry");
        need(!cycle.step(input(0.f, 0.f, 1.f, knob)).naturalBoundary,
             "one completion per traversal");
    }
    chimera::Reel longReel(19, 19);
    for (std::uint32_t i = 0; i < 4800; ++i)
        need(longReel.write(i, chimera::StereoFrame{0.f, 0.f}, i),
             "prepare 4800-frame full-Splice timing fixture");
    double lowKnob = 0.5, highKnob = 5.0/6.0;
    for (int i = 0; i < 40; ++i) {
        const double mid = 0.5 * (lowKnob + highKnob);
        if (chimera::profile1::classicRate(mid) < 0.5) lowKnob = mid;
        else highKnob = mid;
    }
    float halfKnob = static_cast<float>(highKnob);
    while (chimera::profile1::classicRate(halfKnob) < 0.5)
        halfKnob = std::nextafter(halfKnob, 1.f);
    const float timingKnobs[] = {halfKnob, 5.f/6.f, 1.f};
    const int traversalFrames[] = {9600, 4800, 2400};
    for (int variant = 0; variant < 3; ++variant) {
        chimera::Slice timing(&longReel);
        timing.setConditioning(false);
        timing.setPlay(false);
        timing.step(input(0.f, 0.f, 1.f, timingKnobs[variant]));
        timing.setPlay(true);
        for (int i = 0; i < traversalFrames[variant]; ++i)
            need(!timing.step(input(0.f, 0.f, 1.f, timingKnobs[variant])).naturalBoundary,
                 "4800-frame full-Splice traversal cannot complete early");
        need(timing.step(input(0.f, 0.f, 1.f, timingKnobs[variant])).naturalBoundary,
             "full-Splice traversal follows signed-rate source travel");
    }
    chimera::Slice pulse(&tagged);
    pulse.setConditioning(false);
    pulse.setPlay(false);
    pulse.step(input(0.f, 0.f, 1.f));
    pulse.setPlay(true);
    for (int i = 0; i < 16; ++i)
        need(!pulse.step(input(0.f, 0.f, 1.f)).eosg,
             "EOSG stays low until natural full-Splice completion");
    for (int i = 0; i < 7; ++i)
        need(pulse.step(input(0.f, 0.f, 1.f)).eosg,
             "sixteen-frame traversal makes seven-frame adaptive pulse");
    need(!pulse.step(input(0.f, 0.f, 1.f)).eosg,
         "EOSG returns low between completions");
    bool activePulse = false;
    for (int i = 0; i < 16; ++i) {
        if (pulse.step(input(0.f, 0.f, 1.f)).eosg) { activePulse = true; break; }
    }
    need(activePulse, "next completion retriggers EOSG");
    pulse.setPlay(false);
    need(!pulse.step(input(0.f, 0.f, 1.f)).eosg,
         "Play stop clears a pulse immediately");

    chimera::Slice follower;
    follower.setConditioning(false);
    float expectedEnergy = 0.f;
    for (int i = 0; i < 12000; ++i) {
        const float value = i < 6000 ? 5.f :
            (i < 9000 ? 0.f : 1.75f * std::sin(float(i) * 0.017f));
        const chimera::Slice::Output out = follower.step(input(value, -value));
        const float energy = 0.5f *
            (out.audio.l * out.audio.l + out.audio.r * out.audio.r);
        const float alpha = energy > expectedEnergy ? 0.00415799815f : 0.00026038276f;
        expectedEnergy += alpha * (energy - expectedEnergy);
        const float expectedCv = 8.f * std::sqrt(expectedEnergy);
        need(std::fabs(out.cv - expectedCv) < 0.001f,
             "fast CV envelope tracks attack/release reference within 1 mV");
        if (i == 5999) need(out.cv > 7.99f, "anti-phase constant input reaches 8 V");
    }
    chimera::Slice leftEnergy, rightEnergy;
    leftEnergy.setConditioning(false);
    rightEnergy.setConditioning(false);
    float leftCv = 0.f, rightCv = 0.f;
    for (int i = 0; i < 12000; ++i) {
        leftCv = leftEnergy.step(input(5.f, 0.f)).cv;
        rightCv = rightEnergy.step(input(0.f, 5.f)).cv;
    }
    need(near(leftCv, 8.f / std::sqrt(2.f), 0.001f) &&
         near(rightCv, leftCv, 0.001f),
         "CV follower uses both post-mix stereo channels equally");
    chimera::Reel rightWetReel(1, 1);
    for (int i = 0; i < 16; ++i)
        need(rightWetReel.write(i, chimera::StereoFrame{0.f, 1.f}, i),
             "prepare right-only wet CV fixture");
    chimera::Slice rightWet(&rightWetReel);
    rightWet.setConditioning(false);
    float wetCv = 0.f;
    for (int i = 0; i < 12000; ++i)
        wetCv = rightWet.step(input(0.f, 0.f, 1.f)).cv;
    need(near(wetCv, rightCv, 0.001f),
         "CV follower includes wet right channel after S.O.S. mixing");
    chimera::Slice sineFollower;
    sineFollower.setConditioning(false);
    double sineSum = 0.0;
    float sineMin = 8.f, sineMax = 0.f;
    for (int frame = 0; frame < 96000; ++frame) {
        const float voltage = 5.f * static_cast<float>(
            std::sin(2.0 * chimera::profile1::kPi * 1000.0 * frame / 48000.0));
        const float cv = sineFollower.step(input(voltage, voltage)).cv;
        if (frame >= 48000) {
            sineSum += cv;
            if (cv < sineMin) sineMin = cv;
            if (cv > sineMax) sineMax = cv;
        }
    }
    need(std::fabs(sineSum / 48000.0 - 7.390832249895503) < 0.001 &&
         near(sineMin, 7.385726544253625f, 0.001f) &&
         near(sineMax, 7.395930369732668f, 0.001f),
         "unit-peak stereo sine follower matches profile-1 mean/min/max vectors");
    chimera::Slice stopped(&tagged);
    stopped.setPlay(false);
    need(stopped.startCurrent(), "stopped writer starts");
    for (int i = 0; i < 32; ++i)
        need(!stopped.step(input(0.f, 0.f, 0.f, 0.5f)).naturalBoundary,
             "record wraps and Vari-Speed Stop do not emit natural completions");
    chimera::Slice zeroRate(&tagged);
    zeroRate.setConditioning(false);
    need(zeroRate.startCurrent(), "recording continues through Vari-Speed Stop");
    for (int i = 0; i < 1000; ++i)
        zeroRate.step(input(0.f, 0.f, 1.f));
    const std::uint32_t writerBeforeStop = zeroRate.writerPosition();
    chimera::Slice::Output atStop{};
    for (int i = 0; i < 1000; ++i) {
        atStop = zeroRate.step(input(0.f, 0.f, 1.f, 0.5f));
        if (i > 600) need(!atStop.naturalBoundary,
                          "full-Splice boundaries cease at Vari-Speed Stop");
    }
    need(atStop.recording && near(atStop.audio.l, 0.f) &&
         zeroRate.writerPosition() == (writerBeforeStop + 1000) % 16,
         "wet fades silent at Stop while writer keeps advancing one frame per tick");
    const float writerRates[] = {0.5f, 1.f/6.f, 1.f, 5.f/6.f};
    for (int variant = 0; variant < 4; ++variant) {
        chimera::Reel destination(1, 1);
        for (int i = 0; i < 16; ++i)
            need(destination.write(i, chimera::StereoFrame{0.f, 0.f}, i),
                 "prepare writer-rate fixture");
        chimera::Slice writer(&destination);
        writer.setConditioning(false);
        writer.setPlay(false);
        need(writer.startCurrent(), "writer starts independent of Play/rate");
        for (int i = 0; i < 37; ++i)
            writer.step(input(5.f, -5.f, 0.f, writerRates[variant]));
        need(writer.writerPosition() == 5 && destination.readActive(4).l == 1.f,
             "writer advances at fixed 48 kHz for Stop/reverse/double/default rates");
    }
    chimera::Reel constant(1, 1);
    for (int i = 0; i < 16; ++i)
        need(constant.write(i, chimera::StereoFrame{1.f, 1.f}, i),
             "prepare constant wet fade fixture");
    chimera::Slice fade(&constant);
    fade.setConditioning(false);
    for (int i = 0; i < 1000; ++i) fade.step(input(0.f, 0.f, 1.f));
    int fadedTicks = 0;
    for (int i = 0; i < 2000; ++i) {
        const float wet = fade.step(input(0.f, 0.f, 1.f, 0.5f)).audio.l;
        if (wet < 0.999f && fadedTicks == 0)
            need(near(wet, 47.f/48.f, 0.001f),
                 "Stop fade begins with one 48-frame gain decrement");
        if (wet < 0.999f && fadedTicks < 48) ++fadedTicks;
        if (fadedTicks == 48) {
            need(near(wet, 0.f), "Stop fade reaches zero on frame 48");
            break;
        }
    }
    need(fadedTicks == 48, "Stop fade began within bounded rate-control settling");
    chimera::Slice transportFade(&constant);
    transportFade.setConditioning(false);
    for (int i = 0; i < 1000; ++i)
        transportFade.step(input(0.f, 0.f, 1.f));
    transportFade.setPlay(false);
    for (int frame = 0; frame < 48; ++frame) {
        const chimera::Slice::Output out = transportFade.step(input(0.f, 0.f, 1.f));
        need(near(out.audio.l, float(47 - frame) / 48.f, 1e-5f) &&
             near(out.audio.r, float(47 - frame) / 48.f, 1e-5f) && !out.eosg,
             "PLAY stop fades the last wet sample over exactly 48 core frames");
    }
    chimera::Slice quickReturn(&constant);
    quickReturn.setConditioning(false);
    for (int i = 0; i < 1000; ++i)
        quickReturn.step(input(0.f, 0.f, 1.f));
    quickReturn.setPlay(false);
    const float lowFrame = quickReturn.step(input(0.f, 0.f, 1.f)).audio.l;
    quickReturn.setPlay(true);
    const float returnFrame = quickReturn.step(input(0.f, 0.f, 1.f)).audio.l;
    need(std::fabs(returnFrame - lowFrame) < 0.05f && returnFrame > 0.9f,
         "brief PLAY-low interval crossfades its scalar tail into new playback");
    chimera::Reel corrupt(1, 1);
    need(corrupt.write(0, chimera::StereoFrame{
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::infinity()}, 0), "inject malformed source sample");
    chimera::Slice guarded(&corrupt);
    guarded.setConditioning(false);
    const chimera::Slice::Output safe = guarded.step(input(0.f, 0.f, 1.f));
    need(guarded.overloaded() && std::isfinite(safe.audio.l) &&
         std::isfinite(safe.audio.r) && std::isfinite(safe.cv),
         "nonfinite stored source cannot poison audio or CV state");
    guarded.setReel(nullptr);
    need(!guarded.overloaded(), "replacing a reel clears its sticky overload indicator");
    chimera::Reel finiteReel(20, 20);
    for (int i = 0; i < 4800; ++i)
        need(finiteReel.write(i, chimera::StereoFrame{1.f, 1.f}, i),
             "prepare finite Slice fixture");
    chimera::Slice finiteSlice(&finiteReel);
    finiteSlice.setConditioning(false);
    finiteSlice.setRampCv(true);
    finiteSlice.setPlay(false);
    chimera::CoreInput finiteInput = input(0.f, 0.f, 1.f);
    finiteInput.controls.gene = static_cast<float>(
        std::log(480.0 / 4800.0) / std::log(16.0 / 4800.0));
    finiteSlice.step(finiteInput); // Initialize the control smoother while stopped.
    finiteSlice.setPlay(true);
    int firstFiniteBoundary = -1, firstFinitePulse = -1;
    for (int frame = 0; frame <= 500; ++frame) {
        const chimera::Slice::Output out = finiteSlice.step(finiteInput);
        if (out.naturalBoundary && firstFiniteBoundary < 0) firstFiniteBoundary = frame;
        if (out.eosg && firstFinitePulse < 0) firstFinitePulse = frame;
        if (frame == 0 || frame == 480)
            need(out.cv < 0.05f, "primary ramp resets on finite onset/boundary");
        if (frame > 100 && frame < 479)
            need(near(out.audio.l, 1.f, 1e-5f), "finite wet plateau reaches Rack slice");
    }
    need(firstFiniteBoundary == 480 && firstFinitePulse == 480 &&
         finiteSlice.onsetCount() >= 2,
         "finite completion drives primary boundary and EOSG in integrated slice");
    chimera::Slice densePulse(&finiteReel);
    densePulse.setConditioning(false);
    densePulse.setPlay(false);
    chimera::CoreInput denseInput = finiteInput;
    denseInput.controls.morph = 1.f;
    densePulse.step(denseInput);
    densePulse.setPlay(true);
    for (int frame = 0; frame < 480; ++frame)
        need(!densePulse.step(denseInput).eosg,
             "dense finite voices cannot pulse EOSG before natural completion");
    for (int frame = 0; frame < 54; ++frame)
        need(densePulse.step(denseInput).eosg,
             "maximum Morph shortens natural EOSG pulse to 54 core frames");
    need(!densePulse.step(denseInput).eosg,
         "dense EOSG pulse returns low before the next completion");
    chimera::Slice overlappingRamp(&finiteReel);
    overlappingRamp.setConditioning(false);
    overlappingRamp.setRampCv(true);
    overlappingRamp.setPlay(false);
    chimera::CoreInput overlapInput = finiteInput;
    overlapInput.controls.morph = 1.f;
    overlappingRamp.step(overlapInput);
    overlappingRamp.setPlay(true);
    for (int frame = 0; frame <= 480; ++frame) {
        const chimera::Slice::Output out = overlappingRamp.step(overlapInput);
        if (frame == 120 || frame == 240 || frame == 360)
            need(near(out.cv, 8.f * frame / 480.f, 0.001f),
                 "secondary overlap leaves finite primary CV ramp untouched");
        if (frame == 480)
            need(near(out.cv, 0.f) && out.naturalBoundary,
                 "overlapping finite primary ramp resets only on primary boundary");
    }
    chimera::Slice reverseRamp(&finiteReel);
    reverseRamp.setConditioning(false);
    reverseRamp.setRampCv(true);
    reverseRamp.setPlay(false);
    chimera::CoreInput reverseInput = finiteInput;
    reverseInput.controls.rate = 1.f/6.f;
    reverseRamp.step(reverseInput);
    reverseRamp.setPlay(true);
    for (int frame = 0; frame <= 240; ++frame) {
        const chimera::Slice::Output out = reverseRamp.step(reverseInput);
        if (frame == 240)
            need(near(out.cv, 4.f, 0.001f),
                 "reverse finite playback keeps a rising primary CV ramp");
    }
    chimera::Slice fullRamp(&constant);
    fullRamp.setConditioning(false);
    fullRamp.setRampCv(true);
    for (int frame = 0; frame <= 16; ++frame) {
        const chimera::Slice::Output out = fullRamp.step(input(0.f, 0.f, 1.f));
        if (frame == 0 || frame == 16)
            need(near(out.cv, 0.f), "full-Splice primary ramp resets on source traversal");
        if (frame == 8)
            need(near(out.cv, 4.f), "full-Splice primary ramp follows traveled distance");
    }
    chimera::Slice heldRamp(&finiteReel);
    heldRamp.setConditioning(false);
    heldRamp.setRampCv(true);
    for (int frame = 0; frame < 600; ++frame)
        heldRamp.step(input(0.f, 0.f, 1.f));
    float stoppedPhase = 0.f;
    for (int frame = 0; frame < 600; ++frame)
        stoppedPhase = heldRamp.step(input(0.f, 0.f, 1.f, 0.5f)).cv;
    for (int frame = 0; frame < 200; ++frame)
        need(near(heldRamp.step(input(0.f, 0.f, 1.f, 0.5f)).cv, stoppedPhase, 0.001f),
             "full-Splice CV ramp holds after Vari-Speed settles at Stop");
    heldRamp.setPlay(false);
    need(near(heldRamp.step(input(0.f, 0.f, 1.f, 0.5f)).cv, 0.f),
         "stopped transport reports zero ramp CV");
    chimera::Slice fullMorph(&finiteReel);
    fullMorph.setConditioning(false);
    chimera::CoreInput chordInput = input(0.f, 0.f, 1.f);
    chordInput.controls.morph = 1.f;
    bool secondaryEosg = false;
    for (int frame = 0; frame < 4000; ++frame) {
        const chimera::Slice::Output out = fullMorph.step(chordInput);
        if (out.eosg && !out.naturalBoundary && frame > 3000) secondaryEosg = true;
        need(!out.naturalBoundary,
             "high-Morph secondary completion does not reset full-Splice primary");
    }
    need(secondaryEosg,
         "high-Morph full-Splice secondary completion contributes EOSG");
    chimera::Selection arbitration;
    arbitration.observe(0.f, 3, false);
    arbitration.observe(0.34f, 3, false);
    need(arbitration.organizeBin() == 0, "Organize ignores near-boundary upward dither");
    arbitration.observe(0.37f, 3, false);
    arbitration.observe(0.31f, 3, false);
    need(arbitration.organizeBin() == 1, "Organize enters bin one and holds its lower hysteresis band");
    arbitration.observe(0.29f, 3, false);
    need(arbitration.organizeBin() == 0, "Organize exits bin one after downward hysteresis");
    arbitration.observe(1.f, 3, false);
    need(arbitration.organizeBin() == 2, "large Organize jump reaches final equal-width bin");
    arbitration.observe(1.f, 3, true);
    arbitration.observe(1.f, 3, false);
    need(arbitration.requested() == 0 && arbitration.organizeBin() == 2,
         "stationary Organize does not undo wrapped Shift request");
    arbitration.observe(0.1f, 3, true);
    need(arbitration.organizeBin() == 0 && arbitration.requested() == 1,
         "same-frame Organize change resolves before Shift increment");
    arbitration.observe(1.f, chimera::kMaxSplices, false);
    arbitration.setRequested(chimera::kMaxSplices - 1, chimera::kMaxSplices);
    arbitration.observe(1.f, chimera::kMaxSplices, true);
    need(arbitration.organizeBin() == chimera::kMaxSplices - 1 &&
         arbitration.requested() == 0,
         "maximum marker count maps endpoint and wrapped Shift without overflow");
    chimera::Reel selectedReel(4, 4);
    for (std::uint32_t frame = 0; frame < 960; ++frame)
        need(selectedReel.write(frame, chimera::StereoFrame{
            frame < 480 ? 1.f : -1.f, frame < 480 ? 1.f : -1.f}, frame),
            "prepare two Splices for queued selection");
    need(selectedReel.addMarker(480), "prepare second Splice marker");
    chimera::CoreInput selectedInput = input(0.f, 0.f, 1.f);
    selectedInput.controls.morph = 1.f;
    chimera::Slice queued(&selectedReel);
    queued.setConditioning(false);
    for (int frame = 0; frame < 100; ++frame) queued.step(selectedInput);
    selectedInput.controls.organize = 1.f;
    bool secondaryBeforePrimary = false;
    for (int frame = 100; frame < 480; ++frame) {
        const chimera::Slice::Output out = queued.step(selectedInput);
        if (out.eosg && !out.naturalBoundary) secondaryBeforePrimary = true;
        need(queued.currentRegion() == 0 && queued.requestedRegion() == 1,
             "pending Splice waits for primary full-Splice boundary");
    }
    const chimera::Slice::Output committed = queued.step(selectedInput);
    need(secondaryBeforePrimary && committed.naturalBoundary &&
         queued.currentRegion() == 1,
         "secondary completions leave pending selection for the primary cycle");
    chimera::Reel appendedReel(2, 2);
    for (std::uint32_t frame = 0; frame < 480; ++frame)
        need(appendedReel.write(frame, chimera::StereoFrame{1.f, 1.f}, frame),
             "prepare running Append selection fixture");
    chimera::Slice appendedSlice(&appendedReel);
    appendedSlice.setConditioning(false);
    need(appendedSlice.startAppend(), "Append begins beside running old Splice");
    chimera::CoreInput appendInput = input(-5.f, -5.f, 0.f);
    for (int frame = 0; frame < 4; ++frame) appendedSlice.step(appendInput);
    appendedSlice.stopRecord();
    need(appendedSlice.currentRegion() == 0 && appendedSlice.requestedRegion() == 1,
         "Append finalization requests new Splice without interrupting active playback");
    appendInput.controls.sos = 1.f;
    for (int frame = 4; frame < 480; ++frame) {
        const chimera::Slice::Output out = appendedSlice.step(appendInput);
        need(appendedSlice.currentRegion() == 0 &&
             (frame < 200 || out.audio.l > 0.9f),
             "running old Splice remains audible until its primary boundary");
    }
    appendedSlice.step(appendInput);
    need(appendedSlice.currentRegion() == 1,
         "Append auto-request commits at the next primary boundary");
    chimera::Slice retriggerSelection(&selectedReel);
    retriggerSelection.setConditioning(false);
    selectedInput.controls.organize = 0.f;
    selectedInput.controls.slide = 0.f;
    for (int frame = 0; frame < 100; ++frame) retriggerSelection.step(selectedInput);
    selectedInput.controls.organize = 1.f;
    selectedInput.controls.slide = 0.25f;
    retriggerSelection.step(selectedInput);
    need(retriggerSelection.currentRegion() == 0 &&
         retriggerSelection.requestedRegion() == 1,
         "selection remains pending before Play retrigger");
    retriggerSelection.setPlay(false);
    retriggerSelection.setPlay(true);
    retriggerSelection.step(selectedInput);
    need(retriggerSelection.currentRegion() == 1 &&
         std::fabs(retriggerSelection.primaryPosition() - 600.75) < 1e-4,
         "Play retrigger commits selection before using new-region Slide origin");
    chimera::Slice immediate(&selectedReel);
    immediate.setConditioning(false);
    immediate.setImmediateTransitions(true);
    selectedInput.controls.organize = 0.f;
    selectedInput.controls.morph = 1.f/6.f;
    for (int frame = 0; frame < 100; ++frame) immediate.step(selectedInput);
    selectedInput.controls.organize = 1.f;
    const chimera::Slice::Output immediateOutput = immediate.step(selectedInput);
    need(immediate.currentRegion() == 1 && immediateOutput.audio.l < -0.9f,
         "immediate Organize commits on its event frame without old-region fade");
    chimera::Reel spliceReel(2, 2);
    for (std::uint32_t frame = 0; frame < 480; ++frame)
        need(spliceReel.write(frame, chimera::StereoFrame{
            frame < 100 ? 1.f : -1.f, frame < 100 ? 1.f : -1.f}, frame),
            "prepare live Splice capture fixture");
    chimera::Slice spliceSlice(&spliceReel);
    spliceSlice.setConditioning(false);
    for (int frame = 0; frame < 100; ++frame) spliceSlice.step(input(0.f, 0.f, 1.f));
    spliceSlice.requestSplice();
    spliceSlice.step(input(0.f, 0.f, 1.f));
    need(spliceReel.markerCount() == 2 && spliceReel.region(1).begin == 100 &&
         spliceSlice.currentRegion() == 0,
         "playing Splice captures primary address before this frame advances");
    for (int frame = 101; frame < 200; ++frame) {
        const chimera::Slice::Output out = spliceSlice.step(input(0.f, 0.f, 1.f));
        if (frame == 150)
            need(out.audio.l < -0.9f && spliceSlice.currentRegion() == 0,
                 "old voice keeps its captured bounds until natural primary boundary");
    }
    spliceSlice.requestSplice();
    spliceSlice.step(input(0.f, 0.f, 1.f));
    need(spliceReel.markerCount() == 3 && spliceReel.region(2).begin == 200,
         "second live Splice uses the continuing primary coordinate");
    for (int frame = 201; frame < 480; ++frame)
        spliceSlice.step(input(0.f, 0.f, 1.f));
    const chimera::Slice::Output spliceBoundary = spliceSlice.step(input(0.f, 0.f, 1.f));
    need(spliceBoundary.naturalBoundary && spliceSlice.currentRegion() == 0,
         "queued marker table becomes active at the old primary boundary");
    spliceSlice.setPlay(false);
    spliceSlice.requestSplice();
    spliceSlice.step(input(0.f, 0.f, 1.f));
    need(spliceReel.markerCount() == 4 && spliceReel.region(1).begin == 1,
         "stopped Splice captures the retained primary cursor");
    spliceSlice.requestSplice();
    spliceSlice.step(input(0.f, 0.f, 1.f));
    need(spliceReel.markerCount() == 4,
         "duplicate stopped cursor marker is harmless");
    chimera::Reel writerReel(2, 2);
    for (std::uint32_t frame = 0; frame < 480; ++frame)
        need(writerReel.write(frame, chimera::StereoFrame{0.f, 0.f}, frame),
             "prepare writer marker fixture");
    need(writerReel.addMarker(240), "prepare selected marker identity");
    const std::uint32_t selectedId = writerReel.markerId(1);
    chimera::Slice writerSlice(&writerReel);
    writerSlice.setConditioning(false);
    writerSlice.setPlay(false);
    need(writerSlice.startCurrent() && writerSlice.selectRegion(1),
         "latch Current writer then change selection");
    for (int frame = 0; frame < 40; ++frame) writerSlice.step(input(0.f, 0.f));
    writerSlice.requestSplice();
    writerSlice.step(input(0.f, 0.f));
    need(writerReel.region(1).begin == 40 && writerReel.findMarkerId(selectedId) == 2 &&
         writerSlice.currentRegion() == 2 && writerSlice.requestedRegion() == 2 &&
         writerSlice.writerPosition() == 41,
         "Current Splice takes pre-write address and remaps selected stable marker ID");
    writerSlice.stopRecord();
    chimera::Slice appendSplice(&writerReel);
    appendSplice.setPlay(false);
    need(appendSplice.startAppend(), "start Append for deferred endpoint Splice");
    appendSplice.requestSplice();
    appendSplice.step(input(0.f, 0.f));
    need(writerReel.validFrames() == 481 && writerReel.markerCount() == 3,
         "Append Splice at first writer frame defers and coalesces endpoint marker");
    appendSplice.step(input(0.f, 0.f));
    appendSplice.requestSplice();
    appendSplice.step(input(0.f, 0.f));
    need(writerReel.markerCount() == 4 && writerReel.region(3).begin == 482,
         "Append Splice inserts only after its addressed frame is written");
    appendSplice.stopRecord();
    need(writerReel.markerCount() == 5 && writerReel.region(3).begin == 480 &&
         appendSplice.requestedRegion() == 3,
         "Append finalization selects first appended marker before interior markers");
    chimera::Reel overlapReelA(2, 2), overlapReelB(2, 2);
    for (std::uint32_t frame = 0; frame < 480; ++frame) {
        need(overlapReelA.write(frame, chimera::StereoFrame{1.f, 1.f}, frame) &&
             overlapReelB.write(frame, chimera::StereoFrame{1.f, 1.f}, frame),
             "prepare paired finite marker fixtures");
    }
    chimera::CoreInput overlapping = input(0.f, 0.f, 1.f);
    overlapping.controls.gene = 1.f;
    overlapping.controls.morph = 1.f;
    overlapping.controls.slide = 0.25f;
    chimera::Slice overlapA(&overlapReelA), overlapB(&overlapReelB);
    overlapA.setConditioning(false);
    overlapB.setConditioning(false);
    overlapB.setChordRatios(-4.0, 7.0, -8.0);
    bool foundFiniteBoundary = false;
    for (int frame = 0; frame < 2000; ++frame) {
        const chimera::Slice::Output a = overlapA.step(overlapping);
        overlapB.step(overlapping);
        if (frame > 200 && a.naturalBoundary && overlapA.onsetCount() > 10) {
            foundFiniteBoundary = true;
            break;
        }
    }
    need(foundFiniteBoundary &&
         std::fabs(overlapA.primaryPosition() - overlapB.primaryPosition()) < 1e-7,
         "finite primary cursor ignores divergent secondary chord ratios");
    const std::uint32_t overlapAddress =
        static_cast<std::uint32_t>(std::floor(overlapA.primaryPosition()));
    overlapA.requestSplice();
    overlapB.requestSplice();
    const chimera::Slice::Output overlapOutput = overlapA.step(overlapping);
    overlapB.step(overlapping);
    need(overlapOutput.audio.l > 0.f && overlapAddress > 100 &&
         overlapReelA.region(1).begin == overlapAddress &&
         overlapReelB.region(1).begin == overlapAddress,
         "overlapping/chord finite Genes capture the Slide-adjusted primary address once");
    chimera::Reel gapReel(2, 2);
    for (std::uint32_t frame = 0; frame < 480; ++frame)
        need(gapReel.write(frame, chimera::StereoFrame{1.f, 1.f}, frame),
             "prepare density-gap marker fixture");
    chimera::CoreInput sparse = input(0.f, 0.f, 1.f);
    sparse.controls.gene = 1.f;
    sparse.controls.morph = 0.f;
    sparse.controls.slide = 0.37f;
    chimera::Slice gapReference(&gapReel);
    gapReference.setConditioning(false);
    int gapFrame = -1;
    std::uint32_t gapAddress = 0;
    bool previousQuiet = false;
    for (int frame = 0; frame < 4000; ++frame) {
        const double before = gapReference.primaryPosition();
        const chimera::Slice::Output out = gapReference.step(sparse);
        const bool quiet = std::fabs(out.audio.l) < 1e-6f;
        if (frame > 200 && previousQuiet && quiet && !out.naturalBoundary && before > 100.0) {
            gapFrame = frame;
            gapAddress = static_cast<std::uint32_t>(std::floor(before));
            break;
        }
        previousQuiet = quiet;
    }
    need(gapFrame >= 0, "find deterministic finite-Gene no-voice gap");
    chimera::Slice gapCapture(&gapReel);
    gapCapture.setConditioning(false);
    for (int frame = 0; frame < gapFrame; ++frame) gapCapture.step(sparse);
    gapCapture.requestSplice();
    const chimera::Slice::Output gapOutput = gapCapture.step(sparse);
    need(std::fabs(gapOutput.audio.l) < 1e-6f &&
         gapReel.region(1).begin == gapAddress,
         "density gap retains independent primary marker address without a musical reader");
    chimera::Reel pmReel(4, 4);
    for (std::uint32_t frame = 0; frame < 1000; ++frame)
        need(pmReel.write(frame, chimera::StereoFrame{
            frame < 500 ? 1.f : -1.f, frame < 500 ? 1.f : -1.f}, frame),
            "prepare stationary-cursor PM fixture");
    chimera::Slice pmSlice(&pmReel);
    pmSlice.setConditioning(false);
    pmSlice.setPmEnabled(true);
    chimera::CoreInput pmInput = input(0.f, 5.f, 1.f, 0.5f);
    pmInput.controls.slide = 0.25f;
    pmInput.pmRightVolts = 5.f;
    pmInput.pmRightConnected = true;
    for (int frame = 0; frame < 143999; ++frame) pmSlice.step(pmInput);
    need(!pmSlice.pmActive() && pmSlice.pmBlend() == 0.f,
         "quiet-right presence waits for three full seconds before PM entry");
    pmSlice.step(pmInput);
    need(pmSlice.pmActive() && near(pmSlice.pmBlend(), 1.f/240.f, 1e-6f),
         "PM entry begins a 240-frame routing/depth fade");
    chimera::Slice::Output pmSound{};
    for (int frame = 0; frame < 239; ++frame) pmSound = pmSlice.step(pmInput);
    need(pmSlice.pmBlend() > 0.999f && pmSound.audio.l < -0.9f,
         "audio-rate PM sounds a displaced stationary cursor at Vari-Speed Stop");
    const std::uint32_t unmodulatedAddress =
        static_cast<std::uint32_t>(std::floor(pmSlice.primaryPosition()));
    pmSlice.requestSplice();
    pmSlice.step(pmInput);
    need(pmReel.markerCount() == 2 && pmReel.region(1).begin == unmodulatedAddress &&
         unmodulatedAddress < 500,
         "SPLICE marker excludes 480-frame PM displacement");
    pmInput.controls.sos = 0.f;
    need(pmSlice.startCurrent(), "Current recording starts while PM is active");
    std::uint32_t pmWrittenAddress = 0;
    for (int frame = 0; frame < 400; ++frame) {
        pmWrittenAddress = pmSlice.writerPosition();
        pmSlice.step(pmInput);
    }
    need(std::fabs(pmReel.readActive(pmWrittenAddress).l) < 0.01f &&
         std::fabs(pmReel.readActive(pmWrittenAddress).r) < 0.01f,
         "active PM removes right CV from the live Current recording source");
    pmSlice.stopRecord();
    pmInput.controls.sos = 1.f;
    for (int frame = 0; frame < 400; ++frame) pmSlice.step(pmInput);
    pmInput.live.l = 5.f;
    for (int frame = 0; frame < 16; ++frame) pmSlice.step(pmInput);
    need(!pmSlice.pmActive(), "left signal exits PM after 16 detected frames");
    for (int frame = 0; frame < 300; ++frame) pmSound = pmSlice.step(pmInput);
    need(pmSlice.pmBlend() == 0.f && std::fabs(pmSound.audio.l) < 0.05f,
         "PM exit fades displacement and restores wet Stop silence");
    chimera::Slice routeSlice(nullptr);
    routeSlice.setConditioning(false);
    routeSlice.setPmEnabled(true);
    chimera::CoreInput routeInput = input(0.f, 5.f, 0.f);
    routeInput.pmRightConnected = true;
    routeInput.pmRightVolts = 5.f;
    for (int frame = 0; frame < 143999; ++frame) {
        const chimera::Slice::Output out = routeSlice.step(routeInput);
        if (frame == 143998)
            need(!routeSlice.pmActive() && near(out.audio.r, 1.f),
                 "quiet connected left retains right live audio until PM entry");
    }
    chimera::Slice::Output routeOut = routeSlice.step(routeInput);
    need(routeSlice.pmActive() && near(routeOut.audio.l, 0.f) &&
         near(routeOut.audio.r, 239.f/240.f),
         "PM entry begins a 240-frame right live-audio fade without left copying");
    for (int frame = 0; frame < 239; ++frame) routeOut = routeSlice.step(routeInput);
    need(routeSlice.pmBlend() > 0.999f && near(routeOut.audio.r, 0.f),
         "active PM removes right CV from live monitoring");
    routeInput.live.l = 5.f;
    for (int frame = 0; frame < 16; ++frame) routeOut = routeSlice.step(routeInput);
    need(!routeSlice.pmActive() && near(routeOut.audio.l, 1.f) &&
         near(routeOut.audio.r, 1.f/240.f),
         "left presence starts right-audio restoration without copying left");
    for (int frame = 0; frame < 239; ++frame) routeOut = routeSlice.step(routeInput);
    need(routeSlice.pmBlend() < 0.001f && near(routeOut.audio.r, 1.f),
         "PM exit restores the full right live-audio path in 5 ms");
    chimera::Slice rawDetector(nullptr);
    rawDetector.setConditioning(false);
    rawDetector.setPmEnabled(true);
    rawDetector.setInputGain(3);
    chimera::CoreInput rawInput = input(0.004f, 1.f, 0.f);
    rawInput.pmRightConnected = true;
    rawInput.pmRightVolts = 1.f;
    for (int frame = 0; frame < 144000; ++frame) rawDetector.step(rawInput);
    need(rawDetector.pmActive(),
         "PM presence detector uses raw left level before input gain");
    chimera::Reel offsetReel(4, 4);
    for (std::uint32_t frame = 0; frame < 1000; ++frame) {
        const float sample = (float(frame) - 500.f) / 500.f;
        need(offsetReel.write(frame, chimera::StereoFrame{sample, sample}, frame),
             "prepare linear PM address fixture");
    }
    chimera::Slice offsetSlice(&offsetReel);
    offsetSlice.setConditioning(false);
    offsetSlice.setPmEnabled(true);
    chimera::CoreInput offsetInput = input(0.f, 0.f, 1.f, 0.5f);
    offsetInput.controls.slide = 0.25f;
    offsetInput.pmRightConnected = true;
    for (int frame = 0; frame < 144240; ++frame) offsetSlice.step(offsetInput);
    need(offsetSlice.pmActive() && offsetSlice.pmBlend() > 0.999f,
         "linear PM fixture reaches full modulation depth");
    const double stationary = offsetSlice.primaryPosition();
    const float pmVoltages[4] = {1.f, -1.f, 20.f, -20.f};
    for (float voltage : pmVoltages) {
        offsetInput.pmRightVolts = voltage;
        const chimera::Slice::Output out = offsetSlice.step(offsetInput);
        const double bounded = chimera::profile1::clamp(double(voltage), -10.0, 10.0);
        const double address = chimera::profile1::wrapPosition(
            stationary + bounded * 96.0, chimera::Region{0, 1000});
        const float expected = float((address - 500.0) / 500.0);
        need(near(out.audio.l, expected, 0.01f) &&
             near(out.audio.r, expected, 0.01f) &&
             near(float(offsetSlice.primaryPosition()), float(stationary)),
             "PM uses signed 96 frames/V with a raw 10 V clamp and fixed cursor");
    }
    chimera::Reel combinedReel(20, 20);
    for (std::uint32_t frame = 0; frame < 4800; ++frame)
        need(combinedReel.write(frame, chimera::StereoFrame{1.f, -1.f}, frame),
             "prepare combined PM/snapshot stress Reel");
    chimera::Slice combined(&combinedReel);
    combined.setConditioning(false);
    combined.setPmEnabled(true);
    combined.setInop(true);
    chimera::CoreInput combinedInput = input(0.f, 5.f, 1.f);
    combinedInput.controls.gene = 1.f;
    combinedInput.controls.morph = 1.f;
    combinedInput.pmRightVolts = 5.f;
    combinedInput.pmRightConnected = true;
    for (int frame = 0; frame < 144240; ++frame) combined.step(combinedInput);
    need(combined.pmActive() && combined.startCurrent(),
         "combined stress begins Current recording with PM active");
    for (int frame = 0; frame < 7000; ++frame) {
        if (frame == 1000)
            need(combinedReel.beginSnapshot(combined.frame()),
                 "snapshot starts during dense PM/Current processing");
        combinedInput.pmRightVolts = (frame / 16) & 1 ? 5.f : -5.f;
        combinedInput.controls.rate = (frame / 250) & 1 ? 1.f/6.f : 5.f/6.f;
        const chimera::Slice::Output out = combined.step(combinedInput);
        need(std::isfinite(out.audio.l) && std::isfinite(out.audio.r) &&
             !combined.overloaded(),
             "dense PM/reversal/Current/snapshot pass stays finite");
    }
    need(combinedReel.readyForWorker() && combinedReel.cowCopies() > 0 &&
         near(combinedReel.readSnapshot(2000).l, 1.f) &&
         near(combinedReel.readActive(2000).l, 0.f, 0.02f),
         "frozen snapshot survives PM recording and active-page replacement");
    combined.stopRecord();
    need(combinedReel.beginRelease(), "combined snapshot release accepted");
    for (int frame = 0; frame < 10; ++frame) combinedReel.maintenanceTick();
    need(combinedReel.state() == chimera::Reel::Idle &&
         combinedReel.freePages() == 20,
         "combined stress returns all snapshot reserve pages");
    chimera::Slice clocked(&transitionReel);
    clocked.setConditioning(false);
    chimera::CoreInput clockInput = input(0.f, 0.f, 1.f);
    clockInput.controls.gene = static_cast<float>(
        std::log(480.0 / 4800.0) / std::log(16.0 / 4800.0));
    for (int frame = 0; frame < 100; ++frame) {
        clocked.setClockPlayback(true, false, 0, false, 1);
        clocked.step(clockInput);
    }
    const std::uint64_t beforeClockShift = clocked.onsetCount();
    clocked.requestShift();
    clocked.setClockPlayback(true, true, 0, false, 1);
    const chimera::Slice::Output clockSelection = clocked.step(clockInput);
    need(clocked.currentRegion() == 1 && clocked.onsetCount() == beforeClockShift + 1 &&
         std::fabs(clocked.trajectoryOffset() - 480.0) < 1e-5 &&
         !clockSelection.naturalBoundary && !clockSelection.eosg,
         "Clock Shift commits queued region before one forced onset without false EOSG");
    clocked.setPlay(false);
    const std::uint64_t stoppedClockOnsets = clocked.onsetCount();
    clocked.setClockPlayback(true, true, 480, false, 1);
    clocked.step(clockInput);
    need(clocked.onsetCount() == stoppedClockOnsets,
         "Clock cannot restart a PLAY-stopped transport");
    chimera::Slice collided(&transitionReel);
    collided.setConditioning(false);
    collided.setInop(true);
    collided.setPlay(false);
    collided.setClockPlayback(true, false, 480, false, 1);
    collided.step(clockInput); // Settle the finite-Gene control before playback.
    collided.setPlay(true);
    for (int frame = 0; frame < 480; ++frame) {
        collided.setClockPlayback(true, false, 480, false, 1);
        collided.step(clockInput);
    }
    need(collided.primaryBoundaryDue(),
         "prepare natural completion at the same logical frame as controls");
    const std::uint64_t beforeCollisionOnsets = collided.onsetCount();
    collided.requestShift();
    collided.requestPlayRetrigger();
    collided.setClockPlayback(true, true, 480, false, 1);
    collided.prepareFrameSelection(clockInput);
    need(collided.currentRegion() == 1 && collided.startCurrent(),
         "same-frame Shift selects writer destination before REC start");
    const chimera::Slice::Output collision = collided.step(clockInput);
    need(collision.naturalBoundary && collision.eosg &&
         collided.onsetCount() == beforeCollisionOnsets + 1 &&
         collided.writerPosition() == 4801,
         "natural completion survives Shift/PLAY/REC/Clock collision with one new onset");
    collided.stopRecord();
    chimera::Slice stoppedAtDue(&transitionReel);
    stoppedAtDue.setConditioning(false);
    stoppedAtDue.setPlay(false);
    stoppedAtDue.step(clockInput);
    stoppedAtDue.setPlay(true);
    for (int frame = 0; frame < 480; ++frame) stoppedAtDue.step(clockInput);
    need(stoppedAtDue.primaryBoundaryDue(), "prepare natural completion on PLAY stop");
    stoppedAtDue.setPlay(false);
    const chimera::Slice::Output stopDue = stoppedAtDue.step(clockInput);
    const chimera::Slice::Output laterStopped = stoppedAtDue.step(clockInput);
    need(stopDue.naturalBoundary && stopDue.eosg && !laterStopped.eosg,
         "PLAY stop retains a due natural completion once, then clears EOSG");
    chimera::Slice hybrid(&transitionReel);
    hybrid.setConditioning(false);
    chimera::CoreInput hybridInput = input(0.f, 0.f, 1.f);
    hybridInput.controls.morph = 0.55f;
    for (int frame = 0; frame < 1000; ++frame) {
        hybrid.setClockPlayback(true, false, 480, false, 0);
        hybrid.step(hybridInput);
    }
    need(hybrid.hybridStretch() && !hybrid.clockShiftMode(),
         "high Morph chooses Stretch in hybrid Clock mode");
    hybridInput.controls.morph = 0.5f;
    for (int frame = 0; frame < 1000; ++frame) {
        hybrid.setClockPlayback(true, false, 480, false, 0);
        hybrid.step(hybridInput);
    }
    need(hybrid.hybridStretch(), "hybrid density hysteresis holds at the threshold");
    hybridInput.controls.morph = 0.45f;
    for (int frame = 0; frame < 1000; ++frame) {
        hybrid.setClockPlayback(true, false, 480, false, 0);
        hybrid.step(hybridInput);
    }
    need(!hybrid.hybridStretch() && hybrid.clockShiftMode(),
         "low Morph returns hybrid Clock mode to Gene Shift");
    hybrid.setClockPlayback(true, false, 480, false, 2);
    hybrid.step(hybridInput);
    need(!hybrid.clockShiftMode(), "explicit Stretch ignores Morph-selected mode");
    hybrid.setClockPlayback(true, false, 480, false, 1);
    hybrid.step(hybridInput);
    need(hybrid.clockShiftMode(), "explicit Gene Shift ignores Morph-selected mode");
    std::puts("PASS: Chimera 48 kHz slice initial capture, Current, Append, and TLA");
}

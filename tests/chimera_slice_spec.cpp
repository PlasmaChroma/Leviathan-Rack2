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
    std::puts("PASS: Chimera 48 kHz slice initial capture, Current, Append, and TLA");
}

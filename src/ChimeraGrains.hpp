#pragma once

#include "ChimeraReel.hpp"
#include "ChimeraProfile.hpp"
#include "ChimeraPlaybackReader.hpp"
#include <cmath>
#include <cstdint>

namespace chimera {

// Fixed four-slot scheduler. Constructed off the audio thread with fixed window
// window table; step() never allocates. Gene LUT mapping is cached until its
// inputs change; stereo crossmix and pitch choices are latched at launch.
class Grains {
public:
    enum { kTransitionFrames = 250 };
    struct ClockDrive {
        int mode; // 0 free, 1 Gene Shift, 2 Stretch
        bool edge, waiting;
        std::uint32_t period;
        ClockDrive(int m = 0, bool e = false, std::uint32_t p = 0, bool w = false)
            : mode(m), edge(e), waiting(w), period(p) {}
    };
    struct Result {
        StereoFrame audio;
        bool primaryBoundary;
        std::uint8_t completions;
        double markerPosition;
        double primaryPosition;
        float primaryPhase;
        std::uint8_t readers;
        bool invalidSource;
    };

    Grains() : active_(false), phase_(0), primaryAge_(0), primaryLength_(1),
        primaryPosition_(0), slide_(0), nextSlot_(0), onsetCount_(0),
        random_(), smooth_(false), lastWet_{0.f, 0.f}, residual_{0.f, 0.f},
        residualAge_(0), residualLength_(0), residualBlend_(0), pendingResidual_(false),
        pendingCompletions_(0), estimateLength_(1), estimateCountdown_(0),
        full_(false), primaryTravel_(0), startAtCurrent_(false), modeStartPosition_(0),
        activeRegion_{0, 0}, transitionRemaining_(0), clockMode_(0), trajectoryOffset_(0),
        stretchAnchor_(0), stretchStep_(0), stretchVelocity_(0), lastDirection_(1) {
        (void) geneSize::lut(); // Initialize the firmware LUT off the audio thread.
        for (std::uint32_t i = 0; i <= 2048; ++i)
            edge_[i] = static_cast<float>(0.5 - 0.5 * std::cos(profile1::kPi * i / 2048.0));
        ratios_[0] = 2.0; ratios_[1] = 1.5; ratios_[2] = firmware::defaultChordThird;
        reset();
    }

    void setBandlimited(bool enabled) { bandlimited_ = enabled; }
    void setBalancedBandlimiting(bool enabled) { balancedBandlimiting_ = enabled; }
    void setSmooth(bool enabled) { smooth_ = enabled; }
    void setFirmwareEnvelopes(bool enabled) { firmwareEnvelopes_ = enabled; }
    void setImmediateTransitions(bool enabled) {
        if (enabled && !immediate_ && !firmwareEnvelopes_) {
            pendingResidual_ = false;
            residualLength_ = 0;
            for (int i = 0; i < 4; ++i) { tails_[i].active = false; scalarAge_[i] = 48; }
        }
        immediate_ = enabled;
    }
    void setChordRatios(double a, double b, double c) {
        const double incoming[3] = {a, b, c};
        for (int i = 0; i < 3; ++i)
            ratios_[i] = profile1::finite(incoming[i]) && std::fabs(incoming[i]) >= 0.0625 &&
                std::fabs(incoming[i]) <= 16.0 ? incoming[i] : 1.0;
    }
    void setSeed(std::uint32_t seed) { random_ = profile1::FirmwareRandom(seed); }
    void reset(double coordinate = 0.0) {
        continuousMorph_.value = 0.f;
        scheduler_.reset();
        havePmOffset_ = false;
        active_ = false; phase_ = 0; primaryAge_ = 0; primaryLength_ = 1;
        primaryTravel_ = 0;
        primaryPosition_ = coordinate; slide_ = 0; nextSlot_ = 0;
        lastWet_ = residual_ = StereoFrame{0.f, 0.f};
        residualAge_ = residualLength_ = 0; pendingResidual_ = false;
        pendingCompletions_ = 0;
        estimateCountdown_ = 0;
        transitionRemaining_ = 0;
        firmwareGain_ = 1.f;
        firmwareBlockPhase_ = 0;
        firmwareDense_ = false;
        startAtCurrent_ = false;
        activeRegion_ = Region{0, 0};
        clockMode_ = 0;
        resetTrajectory();
        lastDirection_ = 1;
        for (int i = 0; i < 4; ++i) {
            slots_[i] = Voice(); tails_[i] = Voice(); tailAge_[i] = 0;
            tailGainScale_[i] = 1.f; scalarAge_[i] = 48;
            tailLast_[i] = scalar_[i] = StereoFrame{0.f, 0.f};
            tailWeightLast_[i] = scalarWeight_[i] = 0;
        }
    }
    std::uint64_t onsetCount() const { return onsetCount_; }
    std::uint8_t takePendingCompletions() {
        const std::uint8_t count = pendingCompletions_;
        pendingCompletions_ = 0;
        return count;
    }
    std::uint32_t randomState() const { return random_.state; }
    double primaryPosition() const { return primaryPosition_; }
    bool primaryBoundaryDue() const {
        return active_ && (full_ ? primaryTravel_ >= activeRegion_.end - activeRegion_.begin :
                                  primaryAge_ >= primaryLength_);
    }
    std::uint32_t slotAge(std::uint8_t slot) const { return slot < 4 ? slots_[slot].age : 0; }
    double slotPosition(std::uint8_t slot) const { return slot < 4 ? slots_[slot].position : 0; }
    double slotCrossmix(std::uint8_t slot) const { return slot < 4 ? slots_[slot].crossmix : 0; }
    double slotRatio(std::uint8_t slot) const { return slot < 4 ? slots_[slot].ratio : 0.0; }
    bool slotActive(std::uint8_t slot) const { return slot < 4 && slots_[slot].active; }
    double slotEnvelopeEdge(std::uint8_t slot) const { return slot < 4 ? slots_[slot].envelopeEdge : 0; }
    float slotEnvelope(std::uint8_t slot) const { return slot < 4 ? slots_[slot].envelope : 0; }
    double trajectoryOffset() const { return trajectoryOffset_; }
    void resetTrajectory() {
        trajectoryOffset_ = stretchAnchor_ = stretchStep_ = stretchVelocity_ = 0;
    }

    Result step(const Reel& reel, Region region, const CoreOutput& c,
                bool retrigger = false, bool fullMode = false, bool metadataRefresh = false,
                double pmOffset = 0.0, ClockDrive clock = ClockDrive()) {
        continuousMorph_.step(c.morph);
        const double pmDelta = havePmOffset_ ? pmOffset - lastPmOffset_ : 0.0;
        lastPmOffset_ = pmOffset; havePmOffset_ = true;
        qualityBlend_ += profile1::clamp((bandlimited_ ? 1.0 : 0.0) - qualityBlend_, -1.0/240, 1.0/240);
        balancedBlend_ += profile1::clamp((balancedBandlimiting_ ? 1.0 : 0.0) - balancedBlend_, -1.0/240, 1.0/240);
        Result result = {StereoFrame{0.f, 0.f}, false, pendingCompletions_, primaryPosition_, primaryPosition_, 0.f, 0, false};
        pendingCompletions_ = 0;
        const std::uint32_t length = region.end - region.begin;
        if (!length) { reset(); return result; }
        geneClockValid_ = clock.mode != 0 && clock.period != 0 && !clock.waiting;
        if (!fullMode) geneFrames(length, c.gene);
        const bool regionChanged = active_ &&
            (region.begin != activeRegion_.begin || region.end != activeRegion_.end);
        bool metadataRestart = active_ && regionChanged && metadataRefresh &&
            full_ == fullMode && !retrigger;
        if (active_ && (full_ != fullMode || regionChanged) && !metadataRestart) {
            const double priorPosition = primaryPosition_;
            forceTransition();
            startAtCurrent_ = !regionChanged;
            modeStartPosition_ = priorPosition;
            if (regionChanged) slide_ = c.slide * (length - 1.0);
        }
        full_ = fullMode;
        const double density = profile1::morphDensity(c.morph);
        if (!full_) scheduler_.configure(cachedGeneFrames_, morph::stage(c.morph));
        const double targetSlide = c.slide * (length - 1.0);
        const double delta = profile1::clamp(targetSlide - slide_, -64.0, 64.0);
        slide_ += delta;
        if (active_ && !metadataRestart) {
            primaryPosition_ = profile1::wrapPosition(primaryPosition_ + delta, region);
            for (int i = 0; i < 4; ++i)
                if (slots_[i].active) slots_[i].position =
                    profile1::wrapPosition(slots_[i].position + delta, slots_[i].region);
            for (int i = 0; i < 4; ++i)
                if (tails_[i].active) tails_[i].position =
                    profile1::wrapPosition(tails_[i].position + delta, tails_[i].region);
        }
        if (c.rate > 0.f) lastDirection_ = 1;
        else if (c.rate < 0.f) lastDirection_ = -1;
        if (clockMode_ == 2 && ((stretchVelocity_ > 0 && lastDirection_ < 0) ||
                                (stretchVelocity_ < 0 && lastDirection_ > 0)))
            stretchVelocity_ = -stretchVelocity_;
        if (clock.mode != clockMode_) {
            const bool hadActive = active_;
            const double rebaseOffset = hadActive && clock.mode != 0 ? wrapOffset(
                primaryPosition_ - region.begin - slide_, length) : 0.0;
            if (active_) forceTransition();
            metadataRestart = false;
            clockMode_ = clock.mode;
            resetTrajectory();
            // Keep the first onset of the new mode near the current source
            // address; subsequent clock motion remains relative to this anchor.
            trajectoryOffset_ = rebaseOffset;
            stretchAnchor_ = trajectoryOffset_;
        }
        const double stepFrames = clock.edge && clockMode_ != 0 ?
            (full_ ? double(length) : double(geneFrames(length, c.gene))) : 0.0;
        if (clock.edge && clockMode_ == 1) {
            trajectoryOffset_ = wrapOffset(trajectoryOffset_ + lastDirection_ * stepFrames, length);
            if (active_) forceTransition();
        }
        else if (clock.edge && clockMode_ == 2) {
            if (clock.period) {
                stretchAnchor_ = wrapOffset(stretchAnchor_ + stretchStep_, length);
                trajectoryOffset_ = stretchAnchor_;
                stretchVelocity_ = lastDirection_ * stepFrames / clock.period;
                primaryPosition_ = origin(region, c.rate);
            }
            else {
                stretchAnchor_ = trajectoryOffset_;
                stretchVelocity_ = 0;
            }
            stretchStep_ = lastDirection_ * stepFrames;
        }
        if (metadataRestart) {
            // A marker edit changes the next primary region at this natural
            // boundary, but it does not itself schedule a musical onset.
            // Existing slots retain their captured regions and normal ages.
            result.primaryBoundary = true;
            primaryAge_ = 0;
            primaryLength_ = full_ ? length : geneFrames(length, c.gene);
            if (full_) {
                const std::uint32_t oldLength = activeRegion_.end - activeRegion_.begin;
                primaryTravel_ -= std::floor(primaryTravel_ / oldLength) * oldLength;
            }
            else primaryTravel_ = 0;
            primaryPosition_ = profile1::wrapPosition(origin(region, c.rate) +
                (c.rate < 0.f ? -primaryTravel_ : primaryTravel_), region);
            activeRegion_ = region;
            startAtCurrent_ = false;
            if (!full_) {
                estimateLength_ = primaryLength_;
                estimateCountdown_ = 32;
            }
        }
        if (!active_ || retrigger) {
            if (retrigger && active_) forceTransition();
            primaryAge_ = 0;
            primaryLength_ = full_ ? length : geneFrames(length, c.gene);
            primaryTravel_ = 0;
            primaryPosition_ = origin(region, c.rate);
            phase_ = 0;
            scheduler_.reset();
            if (!firmwareEnvelopes_) nextSlot_ = 0;
            active_ = true;
            activeRegion_ = region;
            launch(region, c, density, false);
            startAtCurrent_ = false;
        }
        else {
            bool boundaryThisFrame = metadataRestart;
            if (!metadataRestart && ((full_ && primaryTravel_ >= length) ||
                                     (!full_ && primaryAge_ >= primaryLength_))) {
                result.primaryBoundary = true;
                boundaryThisFrame = true;
                primaryAge_ = 0;
                if (full_) primaryTravel_ -=
                    std::floor(primaryTravel_ / length) * length;
                else {
                    primaryLength_ = geneFrames(length, c.gene);
                    primaryPosition_ = origin(region, c.rate);
                }
            }
            // Recovered rational timing for finite, unclocked Genes. Keep the
            // existing clock override and whole-splice traversal bypass below.
            if (!full_ && !estimateCountdown_) {
                estimateLength_ = geneFrames(length, c.gene);
                estimateCountdown_ = 32;
            }
            if (!full_) --estimateCountdown_;
            if (!full_ && clockMode_ == 0) {
                if (scheduler_.step()) launch(region, c, density, true);
            }
            else if (full_ && boundaryThisFrame && std::fabs(density - 1.0) < 1e-6 && c.rate != 0.f) {
                phase_ = 0;
                launch(region, c, density, true);
            }
            else if (!full_ || c.rate != 0.f) {
                const double estimate = full_ ? length / profile1::clamp(std::fabs(c.rate), 1e-6, 512.0) : estimateLength_;
                const double hop = estimate / density > 1.0 ? estimate / density : 1.0;
                phase_ += 1.0 / hop;
                if (phase_ >= 1.0) {
                    phase_ -= 1.0;
                    launch(region, c, density, true);
                }
            }
        }

        double sumL = 0, sumR = 0, weightSum = 0, envelopeSum = 0;
        for (int i = 0; i < 4; ++i)
            if (slots_[i].active) envelopeSum += slots_[i].envelope;
        if (firmwareBlockPhase_ == 0) firmwareDense_ = envelopeSum > 2.0;
        firmwareBlockPhase_ = (firmwareBlockPhase_ + 1) & 7;
        const bool forcedTransition = transitionRemaining_ != 0;
        const double transitionFraction = double(transitionRemaining_) / kTransitionFrames;
        for (int i = 0; i < 4; ++i) {
            Voice& v = slots_[i];
            Voice& tail = tails_[i];
            const double tailFraction = tail.active ? tailGainScale_[i] *
                (forcedTransition ? transitionFraction : (48.0 - tailAge_[i]) / 48.0) : 0.0;
            if (v.active) {
                const double w = voiceWeight(v) * (transitionRemaining_ ?
                    (1.0 - transitionFraction) : (1.0 - tailFraction));
                const StereoFrame source = firmwareEnvelopes_ && !v.full ?
                    readFirmware(reel, v.region, v.position + pmOffset,
                        c.rate * v.ratio + delta + pmDelta, firmwareDense_, result.invalidSource) :
                    read(reel, v.region, v.position + pmOffset, c.rate * v.ratio + delta + pmDelta, result.invalidSource);
                const StereoFrame mixed = profile1::stereoCrossmix(source, v.crossmix);
                sumL += mixed.l * w;
                sumR += mixed.r * w;
                weightSum += w;
                ++result.readers;
                if (firmwareEnvelopes_ && !v.full) {
                    if (static_cast<std::int64_t>(v.age) > v.releaseStart)
                        v.envelope = std::max(0.f, v.envelope - v.envelopeIncrement);
                    else
                        v.envelope = std::min(1.f, v.envelope + v.envelopeIncrement);
                }
                const double increment = profile1::clamp(c.rate * v.ratio, -512.0, 512.0);
                // Finite v.length is output time. Advancing by increment for
                // that many ages traverses abs(increment) * duration source
                // samples (wrapped within the complete splice), with no second
                // speed division. Whole-splice voices instead expire on travel.
                v.position = profile1::wrapPosition(v.position + increment, v.region);
                v.travel += std::fabs(increment);
                if (v.full) {
                    if (v.travel >= v.length) { v.active = false; ++pendingCompletions_; }
                }
                else if ((++v.age >= v.length && !firmwareEnvelopes_) ||
                         (firmwareEnvelopes_ && v.envelope <= 0.f &&
                          static_cast<std::int64_t>(v.age) > v.releaseStart)) {
                    v.active = false; ++pendingCompletions_;
                }
            }
            if (tail.active) {
                const double w = voiceWeight(tail) * tailFraction;
                const StereoFrame source = read(reel, tail.region, tail.position + pmOffset, c.rate * tail.ratio + delta + pmDelta, result.invalidSource);
                const StereoFrame mixed = profile1::stereoCrossmix(source, tail.crossmix);
                tailLast_[i] = StereoFrame{static_cast<float>(mixed.l * w),
                                           static_cast<float>(mixed.r * w)};
                tailWeightLast_[i] = w;
                sumL += tailLast_[i].l;
                sumR += tailLast_[i].r;
                weightSum += w;
                ++result.readers;
                const double increment = profile1::clamp(c.rate * tail.ratio, -512.0, 512.0);
                tail.position = profile1::wrapPosition(tail.position + increment, tail.region);
                tail.travel += std::fabs(increment);
                if (!tail.full) ++tail.age;
                if ((tail.full ? tail.travel >= tail.length : tail.age >= tail.length) ||
                    ++tailAge_[i] >= (forcedTransition ? kTransitionFrames : 48))
                    tail.active = false;
            }
            if (scalarAge_[i] < 48) {
                const double fade = (48.0 - scalarAge_[i]) / 48.0;
                sumL += scalar_[i].l * fade;
                sumR += scalar_[i].r * fade;
                weightSum += scalarWeight_[i] * fade;
                ++scalarAge_[i];
            }
        }
        const bool recovered = firmwareEnvelopes_ && !full_;
        if (recovered) {
            int index = static_cast<int>((envelopeSum - 1.0) * 6.0) + 3;
            index = index < 0 ? 0 : (index > 33 ? 33 : index);
            firmwareGain_ = .01f * firmware::morph_gain[index] + .99f * firmwareGain_;
        }
        const double divisor = recovered ? 1.0 / firmwareGain_ : (weightSum > 1.0 ? weightSum : 1.0);
        result.audio = StereoFrame{static_cast<float>(sumL / divisor),
                                   static_cast<float>(sumR / divisor)};
        if (pendingResidual_) {
            residual_.l = lastWet_.l - result.audio.l;
            residual_.r = lastWet_.r - result.audio.r;
            pendingResidual_ = false;
        }
        if (weightSum == 0) {
            residualLength_ = 0;
            lastWet_ = StereoFrame{0.f, 0.f};
        }
        else {
            if (residualLength_ && residualAge_ <= residualLength_) {
                const float decay = 1.f - edgeGain(static_cast<double>(residualAge_) / residualLength_);
                result.audio.l += residual_.l * residualBlend_ * decay;
                result.audio.r += residual_.r * residualBlend_ * decay;
                if (++residualAge_ > residualLength_) residualLength_ = 0;
            }
            lastWet_ = result.audio;
        }
        result.markerPosition = primaryPosition_;
        if (c.rate != 0.f)
            primaryPosition_ = profile1::wrapPosition(primaryPosition_ + c.rate, region);
        result.primaryPosition = primaryPosition_;
        result.primaryPhase = full_ ? static_cast<float>(profile1::clamp01(primaryTravel_ / length)) : primaryLength_ ?
            static_cast<float>(profile1::clamp01(double(primaryAge_) / primaryLength_)) : 0.f;
        if (full_) primaryTravel_ += std::fabs(c.rate);
        if (!full_) ++primaryAge_;
        if (transitionRemaining_) --transitionRemaining_;
        if (clockMode_ == 2 && !clock.waiting && stretchVelocity_ != 0)
            trajectoryOffset_ = wrapOffset(trajectoryOffset_ + stretchVelocity_, length);
        return result;
    }

private:
    std::uint32_t geneFrames(std::uint32_t length, double gene) {
        const int code = geneSize::adc(static_cast<float>(gene));
        if (cachedGeneRegion_ != length || cachedGeneCode_ != code ||
            cachedGeneClock_ != geneClockValid_) {
            cachedGeneRegion_ = length;
            cachedGeneCode_ = code;
            cachedGeneClock_ = geneClockValid_;
            cachedGeneFrames_ = profile1::roundNonnegative(
                geneSize::mapAdc(length, code, geneClockValid_).durationSamples);
        }
        return cachedGeneFrames_;
    }
    struct Voice {
        bool active = false;
        bool smooth = false;
        bool full = false;
        float envelope = 0.f, envelopeIncrement = .004f;
        double envelopeEdge = 250;
        std::int64_t releaseStart = 0;
        std::uint32_t age = 0, length = 0;
        double position = 0, ratio = 1, unity = 0;
        float crossmix = 0.f;
        double travel = 0, wallEstimate = 1;
        Region region{0, 0};
    };
    void forceTransition() {
        if (firmwareEnvelopes_ && !full_) {
            // Organize rotates a launch slot; the other voices keep their
            // captured splice bounds. No extra output crossfade or DC residual.
            active_ = false;
            phase_ = 0;
            primaryTravel_ = 0;
            pendingResidual_ = false;
            residualLength_ = 0;
            transitionRemaining_ = 0;
            return;
        }
        bool hadOutgoing = false;
        const float outgoingScale = transitionRemaining_ ?
            float(1.0 - double(transitionRemaining_) / kTransitionFrames) : 1.f;
        for (int i = 0; i < 4; ++i) {
            hadOutgoing = hadOutgoing || slots_[i].active || tails_[i].active;
            if (immediate_) {
                tails_[i].active = false;
                scalarAge_[i] = 48;
            }
            else {
                if (tails_[i].active) {
                    scalar_[i] = tailLast_[i];
                    scalarWeight_[i] = tailWeightLast_[i];
                    scalarAge_[i] = 0;
                }
                tails_[i] = slots_[i];
                tailAge_[i] = 0;
                tailGainScale_[i] = outgoingScale;
            }
            slots_[i] = Voice();
        }
        active_ = false;
        phase_ = 0;
        primaryTravel_ = 0;
        pendingResidual_ = false;
        residualLength_ = 0;
        transitionRemaining_ = !immediate_ && hadOutgoing ? kTransitionFrames : 0;
    }
    float edgeGain(double phase) const {
        const double x = profile1::clamp01(phase) * 2048.0;
        const int i = static_cast<int>(x);
        if (i >= 2048) return edge_[2048];
        return static_cast<float>(edge_[i] + (edge_[i+1] - edge_[i]) * (x - i));
    }
    double weight(std::uint32_t n, std::uint32_t age, bool smooth, double unity) const {
        const std::uint32_t e = profile1::windowEdge(n, smooth);
        if (!e) return 1.0;
        const std::uint32_t distance = age < n - 1 - age ? age : n - 1 - age;
        const double base = edgeGain(static_cast<double>(distance) / e);
        return (1.0 - unity) * base + unity;
    }
    double voiceWeight(const Voice& v) const {
        if (firmwareEnvelopes_ && !v.full) {
            const double address = std::floor(v.position);
            const double boundary = profile1::clamp01(std::min(
                address - v.region.begin, double(v.region.end) - address) / 64.0);
            return v.envelope * boundary;
        }
        if (!v.full) return weight(v.length, v.age, v.smooth, v.unity);
        if (v.length <= 2) return 1.0;
        const double phase = profile1::clamp01(v.travel / v.length);
        const double shortEdge = profile1::clamp(96.0 / v.wallEstimate, 0.0, 0.5);
        const double edge = v.smooth ? (shortEdge > 0.2 ? shortEdge : 0.2) : shortEdge;
        const double distance = phase < 1.0 - phase ? phase : 1.0 - phase;
        const double base = edge > 0 ? edgeGain(distance / edge) : 1.0;
        return (1.0 - v.unity) * base + v.unity;
    }
    double origin(Region region, double rate) const {
        if (startAtCurrent_) return profile1::wrapPosition(modeStartPosition_, region);
        return profile1::wrapPosition(region.begin + slide_ + trajectoryOffset_ +
            (rate < 0 ? -1.0 : 0.0), region);
    }
    static double wrapOffset(double offset, std::uint32_t length) {
        return profile1::wrapPosition(offset, Region{0, length});
    }
    void launch(Region region, const CoreOutput& c, double density, bool natural) {
        if (firmwareEnvelopes_ && !full_) {
            // MG204 0x08029030..0x08029036: end the previous launch's hold
            // at its current output age before rotating to the new slot.
            Voice& outgoing = slots_[(nextSlot_ + 3) % 4];
            if (outgoing.active && !outgoing.full)
                outgoing.releaseStart = std::min(outgoing.releaseStart,
                    static_cast<std::int64_t>(outgoing.age));
        }
        const std::uint8_t slot = nextSlot_;
        nextSlot_ = (nextSlot_ + 1) % 4;
        Voice& v = slots_[slot];
        const bool replacing = v.active;
        if (replacing && !immediate_ && !(firmwareEnvelopes_ && !full_)) {
            if (tails_[slot].active) {
                scalar_[slot] = tailLast_[slot];
                scalarWeight_[slot] = tailWeightLast_[slot];
                scalarAge_[slot] = 0;
            }
            tails_[slot] = v;
            tailAge_[slot] = 0;
            tailGainScale_[slot] = 1.f;
        }
        else if (immediate_) { tails_[slot].active = false; scalarAge_[slot] = 48; }
        const profile1::OnsetChoice choice = profile1::chooseOnset(random_, slot, continuousMorph_.value, ratios_);
        v = Voice();
        v.active = true;
        v.full = full_;
        v.length = full_ ? region.end - region.begin :
            geneFrames(region.end - region.begin, c.gene);
        v.wallEstimate = full_ ? v.length / profile1::clamp(std::fabs(c.rate), 1e-6, 512.0) : v.length;
        v.position = origin(region, c.rate * choice.ratio);
        v.region = region;
        v.ratio = choice.ratio;
        v.smooth = smooth_;
        v.unity = profile1::unityBlend(density, smooth_);
        if (firmwareEnvelopes_ && !full_) {
            v.envelopeEdge = std::min(v.length * .5, 24000.0);
            v.envelopeIncrement = v.length * .5 > 24000.0 ?
                1.f / 24000.f : 2.f * (1.f / v.length);
            const float launchFactor = firmware::morph_launch[morph::stageIndex(c.morph)];
            const float occupiedDensity = firmware::morph_density[morph::stageIndex(c.morph)];
            if ((v.envelopeEdge > 250 && (occupiedDensity < 1.f || !smooth_)) || launchFactor == 1.f) {
                v.envelopeEdge = 250;
                v.envelopeIncrement = .004f;
            }
            v.releaseStart = static_cast<std::int64_t>(double(v.length) - v.envelopeEdge);
            tails_[slot].active = false;
            scalarAge_[slot] = 48;
        }
        v.crossmix = choice.crossmix;
        ++onsetCount_;
        if (natural && !replacing && v.unity > 0 && !immediate_ && !(firmwareEnvelopes_ && !full_)) {
            pendingResidual_ = true;
            residualAge_ = 0;
            residualLength_ = full_ ? static_cast<std::uint32_t>(profile1::clamp(v.wallEstimate / 2.0, 0.0, 96.0)) :
                profile1::windowEdge(v.length, false);
            residualBlend_ = static_cast<float>(v.unity);
        }
    }
    StereoFrame read(const Reel& reel, Region r, double coordinate, double speed, bool& invalid) {
        if (qualityBlend_ <= 0 || std::fabs(speed) <= 1.0)
            return PlaybackReader::cubic(reel, r, coordinate, invalid);
        StereoFrame filtered = readFiltered(reel, r, coordinate, speed, invalid);
        if (qualityBlend_ < 1) {
            const auto original = PlaybackReader::cubic(reel, r, coordinate, invalid);
            filtered.l = float(original.l + (filtered.l-original.l)*qualityBlend_);
            filtered.r = float(original.r + (filtered.r-original.r)*qualityBlend_);
        }
        return filtered;
    }
    PlaybackReader reader_;
    static float firmwareSparse(float a, float b, float c, float d, float t) {
        const float curvature = ((a + d) - b) - c;
        return (((c - a) + curvature * (t * .5f)) * t) + ((a + c) * .5f + b);
    }
    StereoFrame readFirmware(const Reel& reel, Region r, double coordinate, double speed,
                             bool dense, bool& invalid) {
        // Independent quality extension. Keep the recovered reader at <=1x
        // and when disabled; match its raw DC gain of TWO when filtering.
        const double width = std::fabs(speed);
        if (qualityBlend_ <= 0 || width <= 1.0)
            return firmwareRead(reel, r, coordinate, dense, invalid);
        const float blend = static_cast<float>(qualityBlend_ *
            profile1::clamp01((width - 1.0) * 8.0));
        const StereoFrame filtered = readFiltered(reel, r, coordinate, speed, invalid);
        if (blend >= 1.f) return {2.f * filtered.l, 2.f * filtered.r};
        const StereoFrame original = firmwareRead(reel, r, coordinate, dense, invalid);
        return {original.l + (2.f * filtered.l - original.l) * blend,
                original.r + (2.f * filtered.r - original.r) * blend};
    }
    static StereoFrame firmwareRead(const Reel& reel, Region r, double coordinate, bool dense, bool& invalid) {
        const double p = profile1::wrapPosition(coordinate, r);
        const std::int64_t base = static_cast<std::int64_t>(std::floor(p));
        const float t = static_cast<float>(p - base);
        const StereoFrame b = reel.readActive(profile1::wrapTap(base, 0, r));
        const StereoFrame c = reel.readActive(profile1::wrapTap(base, 1, r));
        StereoFrame out;
        if (dense) out = {2.f * (b.l + (c.l - b.l) * t), 2.f * (b.r + (c.r - b.r) * t)};
        else {
            const StereoFrame a = reel.readActive(profile1::wrapTap(base, -1, r));
            const StereoFrame d = reel.readActive(profile1::wrapTap(base, 2, r));
            out = {firmwareSparse(a.l,b.l,c.l,d.l,t), firmwareSparse(a.r,b.r,c.r,d.r,t)};
        }
        if (!profile1::finite(out.l) || !profile1::finite(out.r)) invalid = true;
        return {profile1::audio(out.l), profile1::audio(out.r)};
    }
    StereoFrame readFiltered(const Reel& reel, Region r, double coordinate, double speed, bool& invalid) {
        if (std::fabs(speed) <= 1.0) return PlaybackReader::cubic(reel, r, coordinate, invalid);
        if (balancedBlend_ <= 0) return reader_.read(reel, r, coordinate, speed, invalid);
        if (balancedBlend_ >= 1) return reader_.read(reel, r, coordinate, speed, invalid, true, true);
        const auto full = reader_.read(reel, r, coordinate, speed, invalid);
        const auto balanced = reader_.read(reel, r, coordinate, speed, invalid, true, true);
        return {float(full.l + (balanced.l-full.l)*balancedBlend_),
                float(full.r + (balanced.r-full.r)*balancedBlend_)};
    }
    bool bandlimited_ = false, havePmOffset_ = false;
    bool balancedBandlimiting_ = false;
    double balancedBlend_ = 0;
    double qualityBlend_ = 0, lastPmOffset_ = 0;
    Voice slots_[4], tails_[4];
    std::uint16_t tailAge_[4];
    float tailGainScale_[4];
    StereoFrame tailLast_[4], scalar_[4];
    double tailWeightLast_[4], scalarWeight_[4];
    std::uint8_t scalarAge_[4];
    bool active_;
    double phase_;
    morph::Scheduler scheduler_;
    morph::Continuous continuousMorph_;
    std::uint32_t primaryAge_, primaryLength_;
    double primaryPosition_, slide_;
    std::uint8_t nextSlot_;
    std::uint64_t onsetCount_;
    profile1::FirmwareRandom random_;
    bool smooth_;
    bool immediate_ = false;
    bool firmwareEnvelopes_ = false;
    float firmwareGain_ = 1.f;
    std::uint8_t firmwareBlockPhase_ = 0;
    bool firmwareDense_ = false;
    double ratios_[3];
    float edge_[2049];
    std::uint32_t cachedGeneRegion_ = 0, cachedGeneFrames_ = 0;
    int cachedGeneCode_ = -1;
    bool geneClockValid_ = false, cachedGeneClock_ = false;
    StereoFrame lastWet_, residual_;
    std::uint32_t residualAge_, residualLength_;
    float residualBlend_;
    bool pendingResidual_;
    std::uint8_t pendingCompletions_;
    std::uint32_t estimateLength_;
    std::uint8_t estimateCountdown_;
    bool full_;
    double primaryTravel_;
    bool startAtCurrent_;
    double modeStartPosition_;
    Region activeRegion_;
    std::uint16_t transitionRemaining_;
    int clockMode_;
    double trajectoryOffset_, stretchAnchor_, stretchStep_, stretchVelocity_;
    int lastDirection_;
};

} // namespace chimera

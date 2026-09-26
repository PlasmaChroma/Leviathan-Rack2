#pragma once

#include "ChimeraCore.hpp"
#include "ChimeraGrains.hpp"
#include "ChimeraReel.hpp"
#include "ChimeraSelection.hpp"
#include <cmath>
#include <cstdint>

namespace chimera {

// 48 kHz audio slice. Owns no audio-thread allocation; its Reel is prepared
// off audio and outlives this engine. Playback uses the bounded musical slots;
// the Current writer follows committed Splice selections at its own fixed rate.
class Slice {
public:
    enum RecordState { Idle, Current, Append };
    struct Output {
        StereoFrame audio;
        float cv;
        bool recording, full, naturalBoundary, eosg;
    };

    explicit Slice(Reel* reel = 0) : reel_(reel), state_(Idle), play_(true), retrigger_(false),
        inop_(false), currentRegion_(0), writer_(0), appendStart_(0),
        position_(0.0),
        wetGain_(1.f), sourceBlend_(0.f),
        lastWet_{0.f, 0.f}, stopTail_{0.f, 0.f}, stopTailRemaining_(0),
        frame_(0), lastBoundaryFrame_(0), eosgRemaining_(0), hadBoundary_(false),
        energyState_(0.f),
        overloaded_(false), conditioning_(true), wasReading_(false),
        rampCv_(false), immediateTransitions_(false), shiftRequested_(false), spliceRequested_(false),
        metadataRegionPending_(false), frozenRegion_{0, 0},
        pmEnabled_(false), pmActive_(false), pmBlend_(0.f), leftEnergy_(0.f),
        quietFrames_(0), loudFrames_(0),
        primaryPhase_(0.f), ratioA_(2), ratioB_(1.5), ratioC_(4.0/3.0),
        clockConnected_(false), clockEdge_(false), clockWaiting_(false),
        clockPeriod_(0), clockOption_(0), hybridStretch_(false) {}

    void setBandlimitedPlayback(bool enabled) { grains_.setBandlimited(enabled); }
    void setReel(Reel* reel) {
        markerHistoryState_ = 0;
        overloaded_ = false;
        reel_ = reel;
        state_ = Idle;
        writer_ = appendStart_ = 0;
        recordSegmentStart_ = 0;
        recordSeekPending_ = false;
        currentRegion_ = 0;
        position_ = 0.0;
        eosgRemaining_ = 0;
        hadBoundary_ = false;
        grains_.reset();
        selection_.reset();
        shiftRequested_ = spliceRequested_ = metadataRegionPending_ = metadataRefreshDue_ = false;
        pmActive_ = false;
        pmBlend_ = leftEnergy_ = 0.f;
        quietFrames_ = loudFrames_ = 0;
        wasReading_ = false;
        lastWet_ = stopTail_ = StereoFrame{0.f, 0.f};
        stopTailRemaining_ = 0;
        clockEdge_ = false;
        hybridStretch_ = false;
        framePrepared_ = false;
    }
    void setPlay(bool play) {
        if (play && !play_) retrigger_ = true;
        if (!play) {
            retrigger_ = false;
            if (play_) {
                stopTail_ = lastWet_;
                stopTailRemaining_ = 48;
            }
        }
        play_ = play;
    }
    void requestPlayRetrigger() {
        if (play_) retrigger_ = true;
    }
    bool playing() const { return play_; }
    bool primaryBoundaryDue() const { return wasReading_ && grains_.primaryBoundaryDue(); }
    void setConditioning(bool enabled) {
        if (conditioning_ != enabled) {
            inputDc_[0] = inputDc_[1] = DcBlocker();
            outputDc_[0] = outputDc_[1] = DcBlocker();
        }
        conditioning_ = enabled;
    }
    void setSmoothGenes(bool enabled) { grains_.setSmooth(enabled); }
    void setImmediateTransitions(bool enabled) {
        immediateTransitions_ = enabled;
        grains_.setImmediateTransitions(enabled);
    }
    void requestShift() { shiftRequested_ = true; }
    void requestSplice() { spliceRequested_ = true; }
    void setRampCv(bool enabled) { rampCv_ = enabled; }
    void setPrimaryEosg(bool enabled) {
        if (primaryEosg_ == enabled) return;
        primaryEosg_ = enabled;
        eosgRemaining_ = 0;
        hadBoundary_ = false;
    }

    void setPmEnabled(bool enabled) { pmEnabled_ = enabled; }
    void setRateMode(int mode) { controls_.setRateMode(mode); }
    void setInputGain(int index) {
        if (index < 0 || index > 3 || index == gainIndex_) return;
        static const float gains[4] = {0.70794578f, 1.f, 1.99526231f, 3.98107171f};
        gainIndex_ = index;
        gainStart_ = gainCurrent_;
        gainTarget_ = gains[index];
        gainRemaining_ = 240;
    }
    int inputGain() const { return gainIndex_; }
    float inputGainMultiplier() const { return gainCurrent_; }
    void setClockPlayback(bool connected, bool acceptedEdge, std::uint32_t period,
                          bool waiting, int option) {
        clockConnected_ = connected;
        clockEdge_ = acceptedEdge;
        clockPeriod_ = period;
        clockWaiting_ = waiting;
        clockOption_ = option;
    }
    bool clockShiftMode() const { return clockOption_ == 1 || (clockOption_ == 0 && !hybridStretch_); }
    bool hybridStretch() const { return hybridStretch_; }
    // Resolve this frame's controls and selection before a Clock-armed writer
    // latches its Current region. step() consumes the prepared result once.
    void prepareFrameSelection(const CoreInput& input) {
        preparedControls_ = stepControls(input);
        preparedNaturalBoundary_ = resolveSelection(preparedControls_);
        framePrepared_ = true;
    }
    double trajectoryOffset() const { return grains_.trajectoryOffset(); }
    bool pmActive() const { return pmActive_; }
    float pmBlend() const { return pmBlend_; }
    void setChordRatios(double a, double b, double c) {
        if (a == ratioA_ && b == ratioB_ && c == ratioC_) return;
        ratioA_ = a; ratioB_ = b; ratioC_ = c;
        grains_.setChordRatios(a, b, c);
    }
    std::uint64_t onsetCount() const { return grains_.onsetCount(); }
    double primaryPosition() const { return position_; }
    std::uint16_t currentRegion() const { return currentRegion_; }
    std::uint32_t writerFrame() const { return writer_; }
    std::uint32_t recordSegmentStartFrame() const { return recordSegmentStart_; }
    double playbackPosition() const { return position_; }
    std::uint16_t requestedRegion() const { return selection_.requested(); }
    std::uint16_t organizeBin() const { return selection_.organizeBin(); }
    void setInop(bool inop) {
        inop_ = inop;
        if (state_ == Idle) sourceBlend_ = inop ? 1.f : 0.f;
    }
    RecordState recordState() const { return state_; }
    std::uint32_t writerPosition() const { return writer_; }
    std::uint64_t frame() const { return frame_; }
    bool overloaded() const { return overloaded_; }
    bool selectRegion(std::uint16_t index) {
        if (!reel_ || index >= reel_->markerCount()) return false;
        const bool retargetWriter = state_ == Current && index != currentRegion_;
        currentRegion_ = index;
        metadataRegionPending_ = false;
        metadataRefreshDue_ = false;
        selection_.setRequested(index, reel_->markerCount());
        const Region selected = reel_->region(index);
        if (retargetWriter) {
            recordRegion_ = selected;
            writer_ = selected.begin;
            recordSegmentStart_ = writer_;
            recordSeekPending_ = true;
        }
        position_ = selected.begin;
        grains_.resetTrajectory();
        return true;
    }

    unsigned markerHistoryState() const {
        return reel_ && reel_->documentRevision() == markerHistoryRevision_ ? markerHistoryState_ : 0;
    }
    bool editMarker(unsigned index, std::uint32_t frame, bool remove, unsigned history = 0) {
        if (!reel_ || state_ != Idle || (history && markerHistoryState() != history)) return false;
        const Region previousRegion = metadataRegionPending_ ? frozenRegion_ : reel_->region(currentRegion_);
        const auto currentId = reel_->markerId(currentRegion_);
        const auto requestedId = reel_->markerId(selection_.requested());
        if (history) reel_->swapMarkerHistory(markerHistory_, markerHistoryCount_);
        else if (index >= kMaxSplices || !reel_->editMarker(std::uint16_t(index), frame, remove,
                     markerHistory_, markerHistoryCount_)) return false;
        markerHistoryState_ = history == 1 ? 2 : 1;
        markerHistoryRevision_ = reel_->documentRevision();
        remapMarkers(currentId, requestedId, previousRegion);
        return true;
    }

    bool startCurrent() {
        if (!reel_ || state_ != Idle) return false;
        if (!reel_->validFrames()) return startAppend();
        const Region r = reel_->region(currentRegion_);
        if (r.end <= r.begin) return false;
        recordRegion_ = r;
        writer_ = static_cast<std::uint32_t>(std::floor(profile1::wrapPosition(position_, r)));
        recordSegmentStart_ = writer_;
        recordSeekPending_ = true;
        state_ = Current;
        return true;
    }
    bool startAppend() {
        if (!reel_ || state_ != Idle || reel_->validFrames() >= reel_->capacityFrames()) return false;
        if (reel_->validFrames() && reel_->markerCount() >= kMaxSplices) return false;
        appendStart_ = reel_->validFrames();
        writer_ = appendStart_;
        recordSeekPending_ = false;
        state_ = Append;
        return true;
    }
    void stopRecord() {
        if (state_ == Append && reel_ && writer_ > appendStart_) {
            if (appendStart_) {
                reel_->addMarker(appendStart_);
                std::uint16_t appended = 0;
                while (appended + 1 < reel_->markerCount() &&
                       reel_->region(appended).begin != appendStart_) ++appended;
                selection_.setRequested(appended, reel_->markerCount());
                if (!play_) selectRegion(appended);
            }
            else {
                currentRegion_ = 0;
                position_ = 0.0;
                selection_.setRequested(0, reel_->markerCount());
                grains_.reset(position_);
                wasReading_ = false;
            }
        }
        state_ = Idle;
        recordSeekPending_ = false;
    }

    Output step(const CoreInput& input) {
        bool naturalBoundary = framePrepared_ ? preparedNaturalBoundary_ : false;
        bool naturalCompletion = false;
        const CoreOutput c = framePrepared_ ? preparedControls_ : stepControls(input);
        if (!framePrepared_) naturalBoundary = resolveSelection(c);
        framePrepared_ = false;
        const int clockMode = clockConnected_ ?
            (clockOption_ == 1 ? 1 : (clockOption_ == 2 || hybridStretch_ ? 2 : 1)) : 0;
        const bool clockShift = clockEdge_ && clockMode == 1;
        const Grains::ClockDrive clockDrive(clockMode, clockEdge_, clockPeriod_, clockWaiting_);
        clockEdge_ = false;
        const float normalizedLeft = guard(profile1::audio(input.live.l) * 0.2f);
        const float leftPower = normalizedLeft * normalizedLeft;
        if (!profile1::finite(leftEnergy_)) leftEnergy_ = 0.f;
        leftEnergy_ += 0.0020811647f * (leftPower - leftEnergy_);
        if (leftEnergy_ < 1e-12f) leftEnergy_ = 0.f;
        if (!pmEnabled_ || !input.pmRightConnected) {
            pmActive_ = false;
            quietFrames_ = loudFrames_ = 0;
        }
        else if (!pmActive_) {
            if (leftEnergy_ < 1e-6f) {
                if (++quietFrames_ >= 144000) {
                    pmActive_ = true;
                    loudFrames_ = 0;
                }
            }
            else quietFrames_ = 0;
        }
        else if (leftEnergy_ > 4e-6f) {
            if (++loudFrames_ >= 16) {
                pmActive_ = false;
                quietFrames_ = 0;
            }
        }
        else loudFrames_ = 0;
        pmBlend_ += profile1::clamp((pmActive_ ? 1.f : 0.f) - pmBlend_, -1.f/240.f, 1.f/240.f);
        const double pmOffset = pmBlend_ *
            profile1::clamp(profile1::audio(input.pmRightVolts), -10.f, 10.f) * 96.0;
        const StereoFrame boundedLive{guard(c.live.l), guard(c.live.r)};
        StereoFrame live = conditioning_ ?
            StereoFrame{inputDc_[0].step(boundedLive.l), inputDc_[1].step(boundedLive.r)} : boundedLive;
        live.r *= 1.f - pmBlend_;
        StereoFrame wet{0.f, 0.f};
        double markerPosition = position_;
        Region region{0, 0};
        if (reel_ && reel_->markerCount()) {
            const bool metadataDue = metadataRegionPending_ &&
                (!wasReading_ || !play_ || retrigger_ ||
                 grains_.primaryBoundaryDue() || clockShift);
            if (metadataDue) metadataRegionPending_ = false;
            region = metadataRegionPending_ ? frozenRegion_ : reel_->region(currentRegion_);
            // An initial Append is never audible until finalized.
            if (state_ == Append && appendStart_ == 0) region.end = region.begin;
            // Existing playback bounds remain frozen during Append.
            if (state_ == Append && appendStart_ && region.end > appendStart_)
                region.end = appendStart_;
        }
        const bool canRead = play_ && region.end > region.begin;
        const bool finiteGene = !controls_.fullGene();
        const double length = region.end - region.begin;
        const bool metadataRefresh = metadataRefreshDue_ && !metadataRegionPending_ && canRead;
        if (metadataRefresh) metadataRefreshDue_ = false;
        if (!canRead && wasReading_) {
            naturalBoundary = naturalBoundary || grains_.primaryBoundaryDue();
            naturalCompletion = grains_.takePendingCompletions() != 0;
            grains_.reset(position_);
            wasReading_ = false;
        }
        if (canRead) {
            if (!wasReading_) { grains_.reset(position_); wasReading_ = true; }
            const Grains::Result g = grains_.step(*reel_, region, c, retrigger_, !finiteGene,
                                                   metadataRefresh, pmOffset, clockDrive);
            wet = g.audio;
            position_ = g.primaryPosition;
            markerPosition = g.markerPosition;
            primaryPhase_ = g.primaryPhase;
            naturalBoundary = naturalBoundary || g.primaryBoundary;
            naturalCompletion = g.completions != 0;
            overloaded_ = overloaded_ || g.invalidSource;
            retrigger_ = false;
        }
        if (state_ == Current && recordSeekPending_) {
            // The reader has now resolved Slide, rate direction, and any
            // same-frame retrigger. Seek only once; subsequent writes stay
            // fixed-rate and independent of playback speed.
            writer_ = static_cast<std::uint32_t>(std::floor(
                profile1::wrapPosition(markerPosition, recordRegion_)));
            recordSegmentStart_ = writer_;
            recordSeekPending_ = false;
        }
        const bool splice = spliceRequested_;
        spliceRequested_ = false;
        const bool deferredSplice = splice && state_ == Append;
        if (splice && reel_ && state_ != Append && reel_->validFrames()) {
            const double address = state_ == Current ? double(writer_) :
                canRead ? markerPosition : position_;
            if (state_ == Current || region.end > region.begin)
                insertMarker(static_cast<std::uint32_t>(std::floor(state_ == Current ?
                    address : profile1::wrapPosition(address, region))), region);
        }
        wet.l = guard(wet.l);
        wet.r = guard(wet.r);
        const float gainTarget = canRead && (c.rate != 0.f || pmBlend_ > 0.f) ? 1.f : 0.f;
        wetGain_ += profile1::clamp(gainTarget - wetGain_, -1.f/48.f, 1.f/48.f);
        wet.l *= wetGain_;
        wet.r *= wetGain_;
        // A stopped transport has no fresh reader sample. Fade the last wet
        // sample explicitly; multiplying the already-zero wet signal by the
        // gain ramp would otherwise cut it in one tick. A quick PLAY return
        // crossfades this bounded tail with the new reader.
        if (stopTailRemaining_) {
            const float oldFraction = float(stopTailRemaining_ - 1) / 48.f;
            wet.l = stopTail_.l * oldFraction + wet.l * (1.f - oldFraction);
            wet.r = stopTail_.r * oldFraction + wet.r * (1.f - oldFraction);
            --stopTailRemaining_;
        }
        lastWet_ = wet;
        // One recovered complementary crossfade, shared by monitor and inop=0 writer.
        // Output conditioning follows this bus; input gain/conditioning precedes it.
        const StereoFrame bus = soundOnSound::mix(live, wet, c.sos);
        bool full = false;
        if (reel_ && state_ != Idle) {
            // Recovered inop=1 selects conditioned live only after forming the
            // monitor bus. Preserve the existing 48-frame option-change fade.
            const float target = inop_ ? 1.f : 0.f;
            sourceBlend_ += profile1::clamp(target - sourceBlend_, -1.f/48.f, 1.f/48.f);
            StereoFrame source{(1.f-sourceBlend_)*bus.l + sourceBlend_*live.l,
                               (1.f-sourceBlend_)*bus.r + sourceBlend_*live.r};
            source.l = guard(source.l);
            source.r = guard(source.r);
            const std::uint32_t writtenFrame = writer_;
            if (!reel_->write(writer_, source, frame_)) {
                full = true;
                stopRecord();
            }
            else if (state_ == Current) {
                ++writer_;
                if (writer_ == recordRegion_.end) writer_ = recordRegion_.begin;
            }
            else if (state_ == Append) {
                if (deferredSplice) insertMarker(writtenFrame, region);
                ++writer_;
                if (writer_ == reel_->capacityFrames()) {
                    full = true;
                    stopRecord();
                }
            }
        }
        // A natural completion starts a core-timed pulse. Keep it shorter than
        // half the expected interval so rapid traversals remain distinguishable.
        const bool completion = primaryEosg_ ? naturalBoundary : naturalCompletion;
        if ((completion || (naturalBoundary && canRead)) &&
            (finiteGene || c.rate != 0.f || completion)) {
            double interval = finiteGene ?
                profile1::finiteGeneFrames(static_cast<std::uint32_t>(length), c.gene,
                    clockConnected_ && clockPeriod_ && !clockWaiting_) /
                    (primaryEosg_ ? 1.0 : profile1::morphDensity(c.morph)) :
                length / (c.rate != 0.f ? std::fabs(c.rate) : 1.0);
            if (hadBoundary_) {
                const std::uint64_t spacing = frame_ - lastBoundaryFrame_;
                if (spacing && spacing < interval) interval = double(spacing);
            }
            const double boundedWidth = profile1::clamp(interval * 0.45, 1.0, 240.0);
            eosgRemaining_ = static_cast<std::uint16_t>(boundedWidth);
            lastBoundaryFrame_ = frame_;
            hadBoundary_ = true;
        }
        if ((!canRead && !completion) ||
            (!finiteGene && c.rate == 0.f && !completion)) eosgRemaining_ = 0;
        const bool eosg = eosgRemaining_ != 0;
        if (eosg) --eosgRemaining_;
        if (reel_) reel_->maintenanceTick();
        ++frame_;
        const StereoFrame heard = conditioning_ ?
            StereoFrame{outputDc_[0].step(bus.l), outputDc_[1].step(bus.r)} : bus;
        const float energyLeft = guard(heard.l), energyRight = guard(heard.r);
        const float energy = 0.5f * (energyLeft * energyLeft + energyRight * energyRight);
        if (!profile1::finite(energyState_)) energyState_ = 0.f;
        const float alpha = energy > energyState_ ? 0.00415799815f : 0.00026038276f;
        energyState_ += alpha * (energy - energyState_);
        if (energyState_ < 1e-12f) energyState_ = 0.f;
        const float cv = rampCv_ ? (canRead ? 8.f * primaryPhase_ : 0.f) :
            8.f * fastSqrt(energyState_ < 1.f ? energyState_ : 1.f);
        return Output{heard, cv, state_ != Idle, full, naturalBoundary, eosg};
    }

private:
    CoreOutput stepControls(const CoreInput& input) {
        if (gainRemaining_) {
            const std::uint16_t elapsed = static_cast<std::uint16_t>(241 - gainRemaining_);
            gainCurrent_ = gainStart_ + (gainTarget_ - gainStart_) * (float(elapsed) / 240.f);
            --gainRemaining_;
        }
        CoreInput gained = input;
        gained.live.l *= gainCurrent_;
        gained.live.r *= gainCurrent_;
        return controls_.step(gained);
    }
    bool resolveSelection(const CoreOutput& c) {
        // Recovered launch-factor boundary; existing clock policy remains in charge.
        hybridStretch_ = morph::stretchCandidate(c.morph);
        const bool clockShift = clockEdge_ && clockShiftMode();
        bool naturalBoundary = false;
        if (reel_) {
            selection_.observe(c.organize, reel_->markerCount(), shiftRequested_);
            const std::uint16_t requested = selection_.requested();
            if (requested != currentRegion_ &&
                (!play_ || immediateTransitions_ || retrigger_ || !wasReading_ ||
                 grains_.primaryBoundaryDue() || clockShift)) {
                naturalBoundary = wasReading_ && grains_.primaryBoundaryDue();
                selectRegion(requested);
            }
        }
        shiftRequested_ = false;
        return naturalBoundary;
    }
    void insertMarker(std::uint32_t frame, Region playbackRegion) {
        if (!reel_ || frame >= reel_->validFrames() ||
            reel_->markerCount() >= kMaxSplices -
                (state_ == Append && appendStart_ != 0 ? 1 : 0) ||
            (state_ == Append && frame == appendStart_)) return;
        const std::uint32_t currentId = reel_->markerId(currentRegion_);
        const std::uint32_t requestedId = reel_->markerId(selection_.requested());
        if (!reel_->addMarker(frame)) return;
        remapMarkers(currentId, requestedId, playbackRegion);
    }
    void remapMarkers(std::uint32_t currentId, std::uint32_t requestedId, Region playbackRegion) {
        const std::uint16_t count = reel_->markerCount();
        const std::uint16_t remappedCurrent = reel_->findMarkerId(currentId);
        const std::uint16_t remappedRequested = reel_->findMarkerId(requestedId);
        if (remappedCurrent < count) currentRegion_ = remappedCurrent;
        else {
            currentRegion_ = 0;
            while (currentRegion_ + 1 < count &&
                   reel_->region(currentRegion_ + 1).begin <= playbackRegion.begin) ++currentRegion_;
        }
        selection_.setRequested(remappedRequested < count ? remappedRequested : currentRegion_, count);
        const Region updated = reel_->region(currentRegion_);
        if (wasReading_ && (updated.begin != playbackRegion.begin || updated.end != playbackRegion.end)) {
            frozenRegion_ = playbackRegion;
            metadataRegionPending_ = true;
            metadataRefreshDue_ = true;
        }
    }
    static float fastSqrt(float x) {
        if (x <= 0.f) return 0.f;
        std::uint32_t bits;
        std::memcpy(&bits, &x, sizeof(bits));
        bits = 0x5f3759dfu - (bits >> 1);
        float inverse;
        std::memcpy(&inverse, &bits, sizeof(inverse));
        const float half = 0.5f * x;
        inverse *= 1.5f - half * inverse * inverse;
        inverse *= 1.5f - half * inverse * inverse;
        return x * inverse;
    }
    struct DcBlocker {
        float oldInput = 0.f;
        float oldOutput = 0.f;
        float step(float input) {
            const float output = input - oldInput + 0.9993457156679053f * oldOutput;
            oldInput = input;
            oldOutput = std::fabs(output) < 1e-12f ? 0.f : output;
            return oldOutput;
        }
    };
    float guard(float value) {
        if (!profile1::finite(value)) { overloaded_ = true; return 0.f; }
        if (value > 64.f) { overloaded_ = true; return 64.f; }
        if (value < -64.f) { overloaded_ = true; return -64.f; }
        return value;
    }

    Core controls_;
    Marker markerHistory_[kMaxSplices]{};
    std::uint16_t markerHistoryCount_ = 0;
    unsigned markerHistoryState_ = 0; // 1 Undo, 2 Redo.
    std::uint64_t markerHistoryRevision_ = 0;
    Grains grains_;
    Selection selection_;
    Reel* reel_;
    RecordState state_;
    bool play_, retrigger_, inop_;
    std::uint16_t currentRegion_;
    Region recordRegion_;
    std::uint32_t writer_, appendStart_;
    std::uint32_t recordSegmentStart_ = 0;
    bool recordSeekPending_ = false;
    double position_;
    float wetGain_, sourceBlend_;
    StereoFrame lastWet_, stopTail_;
    std::uint8_t stopTailRemaining_;
    std::uint64_t frame_;
    std::uint64_t lastBoundaryFrame_;
    bool primaryEosg_ = false; // Legacy core callers retain all-voice completions.
    std::uint16_t eosgRemaining_;
    bool hadBoundary_;
    float energyState_;
    bool overloaded_;
    bool conditioning_;
    bool wasReading_, rampCv_, immediateTransitions_, shiftRequested_, spliceRequested_;
    bool metadataRegionPending_, metadataRefreshDue_ = false;
    Region frozenRegion_;
    bool pmEnabled_, pmActive_;
    float pmBlend_, leftEnergy_;
    std::uint32_t quietFrames_;
    std::uint8_t loudFrames_;
    float primaryPhase_;
    double ratioA_, ratioB_, ratioC_;
    bool clockConnected_, clockEdge_, clockWaiting_;
    std::uint32_t clockPeriod_;
    int clockOption_;
    bool hybridStretch_;
    int gainIndex_ = 1;
    float gainCurrent_ = 1.f, gainStart_ = 1.f, gainTarget_ = 1.f;
    std::uint16_t gainRemaining_ = 0;
    CoreOutput preparedControls_{};
    bool framePrepared_ = false, preparedNaturalBoundary_ = false;
    DcBlocker inputDc_[2], outputDc_[2];
};

} // namespace chimera

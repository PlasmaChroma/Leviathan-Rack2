#pragma once

#include "ChimeraCore.hpp"
#include "ChimeraGrains.hpp"
#include "ChimeraReel.hpp"
#include <cmath>
#include <cstdint>

namespace chimera {

// 48 kHz audio slice. Owns no audio-thread allocation; its Reel is prepared
// off audio and outlives this engine. Playback uses the bounded musical slots
// while the Current/Append writer remains independent.
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
        frame_(0), lastBoundaryFrame_(0), eosgRemaining_(0), hadBoundary_(false),
        energyState_(0.f),
        overloaded_(false), conditioning_(true), wasReading_(false),
        rampCv_(false), primaryPhase_(0.f), ratioA_(2), ratioB_(3), ratioC_(4) {}

    void setReel(Reel* reel) {
        reel_ = reel;
        state_ = Idle;
        currentRegion_ = 0;
        position_ = 0.0;
        eosgRemaining_ = 0;
        hadBoundary_ = false;
        grains_.reset();
        wasReading_ = false;
    }
    void setPlay(bool play) {
        if (play && !play_) retrigger_ = true;
        if (!play) retrigger_ = false;
        play_ = play;
    }
    void setConditioning(bool enabled) {
        if (conditioning_ != enabled) {
            inputDc_[0] = inputDc_[1] = DcBlocker();
            outputDc_[0] = outputDc_[1] = DcBlocker();
        }
        conditioning_ = enabled;
    }
    void setSmoothGenes(bool enabled) { grains_.setSmooth(enabled); }
    void setImmediateTransitions(bool enabled) { grains_.setImmediateTransitions(enabled); }
    void setRampCv(bool enabled) { rampCv_ = enabled; }
    void setChordRatios(double a, double b, double c) {
        if (a == ratioA_ && b == ratioB_ && c == ratioC_) return;
        ratioA_ = a; ratioB_ = b; ratioC_ = c;
        grains_.setChordRatios(a, b, c);
    }
    std::uint64_t onsetCount() const { return grains_.onsetCount(); }
    double primaryPosition() const { return position_; }
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
        currentRegion_ = index;
        const Region selected = reel_->region(index);
        position_ = selected.begin;
        return true;
    }

    bool startCurrent() {
        if (!reel_ || state_ != Idle) return false;
        if (!reel_->validFrames()) return startAppend();
        const Region r = reel_->region(currentRegion_);
        if (r.end <= r.begin) return false;
        recordRegion_ = r;
        writer_ = r.begin;
        state_ = Current;
        return true;
    }
    bool startAppend() {
        if (!reel_ || state_ != Idle || reel_->validFrames() >= reel_->capacityFrames()) return false;
        if (reel_->validFrames() && reel_->markerCount() >= kMaxSplices) return false;
        appendStart_ = reel_->validFrames();
        writer_ = appendStart_;
        state_ = Append;
        return true;
    }
    void stopRecord() {
        if (state_ == Append && reel_ && writer_ > appendStart_) {
            if (appendStart_) {
                reel_->addMarker(appendStart_);
                currentRegion_ = reel_->markerCount() - 1;
            }
            else currentRegion_ = 0;
            position_ = static_cast<double>(reel_->region(currentRegion_).begin);
            grains_.reset(position_);
            wasReading_ = false;
        }
        state_ = Idle;
    }

    Output step(const CoreInput& input) {
        bool naturalBoundary = false;
        bool naturalCompletion = false;
        const CoreOutput c = controls_.step(input);
        const StereoFrame live = conditioning_ ?
            StereoFrame{inputDc_[0].step(c.live.l), inputDc_[1].step(c.live.r)} : c.live;
        StereoFrame wet{0.f, 0.f};
        Region region{0, 0};
        if (reel_ && reel_->markerCount()) {
            region = reel_->region(currentRegion_);
            // An initial Append is never audible until finalized.
            if (state_ == Append && appendStart_ == 0) region.end = region.begin;
            // Existing playback bounds remain frozen during Append.
            if (state_ == Append && appendStart_ && region.end > appendStart_)
                region.end = appendStart_;
        }
        const bool canRead = play_ && region.end > region.begin;
        const bool finiteGene = !controls_.fullGene();
        const double length = region.end - region.begin;
        if (!canRead && wasReading_) { grains_.reset(position_); wasReading_ = false; }
        if (canRead) {
            if (!wasReading_) { grains_.reset(position_); wasReading_ = true; }
            const Grains::Result g = grains_.step(*reel_, region, c, retrigger_, !finiteGene);
            wet = g.audio;
            position_ = g.primaryPosition;
            primaryPhase_ = g.primaryPhase;
            naturalBoundary = naturalBoundary || g.primaryBoundary;
            naturalCompletion = g.completions != 0;
            overloaded_ = overloaded_ || g.invalidSource;
            retrigger_ = false;
        }
        wet.l = guard(wet.l);
        wet.r = guard(wet.r);
        const float gainTarget = canRead && c.rate != 0.f ? 1.f : 0.f;
        wetGain_ += profile1::clamp(gainTarget - wetGain_, -1.f/48.f, 1.f/48.f);
        wet.l *= wetGain_;
        wet.r *= wetGain_;
        const float s = c.sos;
        StereoFrame bus{(1.f-s)*live.l + s*wet.l, (1.f-s)*live.r + s*wet.r};
        bool full = false;
        if (reel_ && state_ != Idle) {
            const float target = inop_ ? 1.f : 0.f;
            sourceBlend_ += profile1::clamp(target - sourceBlend_, -1.f/48.f, 1.f/48.f);
            StereoFrame source{(1.f-sourceBlend_)*bus.l + sourceBlend_*live.l,
                               (1.f-sourceBlend_)*bus.r + sourceBlend_*live.r};
            source.l = guard(source.l);
            source.r = guard(source.r);
            if (!reel_->write(writer_, source, frame_)) {
                full = true;
                stopRecord();
            }
            else if (state_ == Current) {
                ++writer_;
                if (writer_ == recordRegion_.end) writer_ = recordRegion_.begin;
            }
            else if (state_ == Append) {
                ++writer_;
                if (writer_ == reel_->capacityFrames()) {
                    full = true;
                    stopRecord();
                }
            }
        }
        // A natural completion starts a core-timed pulse. Keep it shorter than
        // half the expected interval so rapid traversals remain distinguishable.
        if ((naturalCompletion || naturalBoundary) && canRead && (finiteGene || c.rate != 0.f)) {
            double interval = finiteGene ? profile1::finiteGeneFrames(static_cast<std::uint32_t>(length), c.gene) / profile1::morphDensity(c.morph) : length / std::fabs(c.rate);
            if (hadBoundary_) {
                const std::uint64_t spacing = frame_ - lastBoundaryFrame_;
                if (spacing && spacing < interval) interval = double(spacing);
            }
            const double boundedWidth = profile1::clamp(interval * 0.45, 1.0, 240.0);
            eosgRemaining_ = static_cast<std::uint16_t>(boundedWidth);
            lastBoundaryFrame_ = frame_;
            hadBoundary_ = true;
        }
        if (!canRead || (!finiteGene && c.rate == 0.f)) eosgRemaining_ = 0;
        const bool eosg = eosgRemaining_ != 0;
        if (eosg) --eosgRemaining_;
        if (reel_) reel_->maintenanceTick();
        ++frame_;
        const StereoFrame heard = conditioning_ ?
            StereoFrame{outputDc_[0].step(bus.l), outputDc_[1].step(bus.r)} : bus;
        const float energy = 0.5f * (heard.l * heard.l + heard.r * heard.r);
        const float alpha = energy > energyState_ ? 0.00415799815f : 0.00026038276f;
        energyState_ += alpha * (energy - energyState_);
        const float cv = rampCv_ ? (canRead ? 8.f * primaryPhase_ : 0.f) :
            8.f * fastSqrt(energyState_ < 1.f ? energyState_ : 1.f);
        return Output{heard, cv, state_ != Idle, full, naturalBoundary, eosg};
    }

private:
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
            oldOutput = output;
            return output;
        }
    };
    float guard(float value) {
        if (!profile1::finite(value)) { overloaded_ = true; return 0.f; }
        if (value > 64.f) { overloaded_ = true; return 64.f; }
        if (value < -64.f) { overloaded_ = true; return -64.f; }
        return value;
    }

    Core controls_;
    Grains grains_;
    Reel* reel_;
    RecordState state_;
    bool play_, retrigger_, inop_;
    std::uint16_t currentRegion_;
    Region recordRegion_;
    std::uint32_t writer_, appendStart_;
    double position_;
    float wetGain_, sourceBlend_;
    std::uint64_t frame_;
    std::uint64_t lastBoundaryFrame_;
    std::uint16_t eosgRemaining_;
    bool hadBoundary_;
    float energyState_;
    bool overloaded_;
    bool conditioning_;
    bool wasReading_, rampCv_;
    float primaryPhase_;
    double ratioA_, ratioB_, ratioC_;
    DcBlocker inputDc_[2], outputDc_[2];
};

} // namespace chimera

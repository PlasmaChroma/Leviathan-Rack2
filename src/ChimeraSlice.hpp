#pragma once

#include "ChimeraCore.hpp"
#include "ChimeraReel.hpp"
#include <cmath>
#include <cstdint>

namespace chimera {

// The first audible 48 kHz slice. Owns no allocation; its Reel is prepared
// off audio and outlives this engine. Later phases replace its single reader
// with the bounded musical voice scheduler.
class Slice {
public:
    enum RecordState { Idle, Current, Append };
    struct Output { StereoFrame audio; bool recording; bool full; bool naturalBoundary; };

    explicit Slice(Reel* reel = 0) : reel_(reel), state_(Idle), play_(true), retrigger_(false),
        inop_(false), currentRegion_(0), writer_(0), appendStart_(0),
        position_(0.0), slide_(0.0), travel_(0.0), boundaryPending_(false),
        wetGain_(1.f), sourceBlend_(0.f),
        frame_(0), overloaded_(false), conditioning_(true) {}

    void setReel(Reel* reel) {
        reel_ = reel;
        state_ = Idle;
        currentRegion_ = 0;
        position_ = slide_ = travel_ = 0.0;
        boundaryPending_ = false;
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
    void setInop(bool inop) {
        inop_ = inop;
        if (state_ == Idle) sourceBlend_ = inop ? 1.f : 0.f;
    }
    RecordState recordState() const { return state_; }
    std::uint32_t writerPosition() const { return writer_; }
    std::uint64_t frame() const { return frame_; }
    bool overloaded() const { return overloaded_; }

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
            travel_ = slide_ = 0.0;
        }
        state_ = Idle;
    }

    Output step(const CoreInput& input) {
        const bool naturalBoundary = boundaryPending_;
        boundaryPending_ = false;
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
        const double length = region.end - region.begin;
        if (canRead) {
            const double target = c.slide * (length - 1.0);
            const double delta = profile1::clamp(target - slide_, -64.0, 64.0);
            slide_ += delta;
            if (retrigger_) {
                position_ = profile1::wrapPosition(region.begin + slide_ +
                    (c.rate < 0.f ? -1.0 : 0.0), region);
                travel_ = 0.0;
                retrigger_ = false;
            }
            else position_ = profile1::wrapPosition(position_ + delta, region);
            if (position_ < region.begin || position_ >= region.end)
                position_ = region.begin + slide_;
            wet = read(region, position_);
        }
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
        if (canRead && c.rate != 0.f) {
            position_ = profile1::wrapPosition(position_ + c.rate, region);
            travel_ += std::fabs(c.rate);
            if (travel_ >= length) {
                boundaryPending_ = true;
                travel_ = std::fmod(travel_, length); // Retain fractional excess.
            }
        }
        if (reel_) reel_->maintenanceTick();
        ++frame_;
        const StereoFrame heard = conditioning_ ?
            StereoFrame{outputDc_[0].step(bus.l), outputDc_[1].step(bus.r)} : bus;
        return Output{heard, state_ != Idle, full, naturalBoundary};
    }

private:
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
    StereoFrame read(Region r, double coordinate) const {
        const double p = profile1::wrapPosition(coordinate, r);
        const std::int64_t base = static_cast<std::int64_t>(std::floor(p));
        const double t = p - base;
        const StereoFrame a = reel_->readActive(profile1::wrapTap(base, -1, r));
        const StereoFrame b = reel_->readActive(profile1::wrapTap(base,  0, r));
        const StereoFrame c = reel_->readActive(profile1::wrapTap(base,  1, r));
        const StereoFrame d = reel_->readActive(profile1::wrapTap(base,  2, r));
        return StereoFrame{static_cast<float>(profile1::cubic(a.l,b.l,c.l,d.l,t)),
                           static_cast<float>(profile1::cubic(a.r,b.r,c.r,d.r,t))};
    }
    float guard(float value) {
        if (!profile1::finite(value)) { overloaded_ = true; return 0.f; }
        if (value > 64.f) { overloaded_ = true; return 64.f; }
        if (value < -64.f) { overloaded_ = true; return -64.f; }
        return value;
    }

    Core controls_;
    Reel* reel_;
    RecordState state_;
    bool play_, retrigger_, inop_;
    std::uint16_t currentRegion_;
    Region recordRegion_;
    std::uint32_t writer_, appendStart_;
    double position_, slide_, travel_;
    bool boundaryPending_;
    float wetGain_, sourceBlend_;
    std::uint64_t frame_;
    bool overloaded_;
    bool conditioning_;
    DcBlocker inputDc_[2], outputDc_[2];
};

} // namespace chimera

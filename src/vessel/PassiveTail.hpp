#pragma once
#include "HostRateAdapter.hpp"
#include <cmath>

namespace vessel {

// Cached homogeneous modal evolution and the equivalent FIR observer. Cache
// construction is incremental on the audio owner: no worker, allocation, or
// transcendental work, and at most preparationTaps polynomial terms per call.
class PassiveTail {
public:
    static constexpr unsigned preparationTaps = 16;
    void invalidate() noexcept { count_=0; mode_=stage_=tap_=0; ready_=false; }
    bool ready() const noexcept { return ready_; }
    void prepare(const VesselEngine& engine, const StereoDecimator& filter) noexcept;
    static bool eligible(const VesselEngine& engine) noexcept;
    // Controls have already been delivered by the adapter. Maintains the same
    // smoothing and fault accounting as the engine's ordinary unforced path.
    EngineFrame advance(VesselEngine& engine, unsigned factor) const noexcept;

    // A fictitious continuation supplies only the old FIR history contribution
    // during re-entry. Configuration changes must leave this snapshot intact.
    void capture(const PassiveTail& model, const VesselEngine& engine) noexcept;
    StereoSample stepBaseline() noexcept;
    StereoSample filteredBaseline() const noexcept;
private:
    struct Matrix {
        double a=1, b=0, c=0, d=1;
        Matrix times(const Matrix& r) const noexcept;
        Matrix power(unsigned n) const noexcept;
        Matrix inverse() const noexcept;
        void step(ModalState& state) const noexcept;
    };
    struct Mode {
        Matrix internal, host;
        double filteredX=0, filteredY=1;
    };
    std::array<Mode,maxModes> modes_ {};
    std::array<ModalState,maxModes> states_ {};
    ModalVector left_ {}, right_ {};
    std::size_t count_=0, mode_=0;
    unsigned stage_=0, tap_=0;
    Matrix inverse_, response_, power_, stageStep_, filterSum_;
    bool ready_=false;
};

inline void PassiveTail::Matrix::step(ModalState& s) const noexcept {
    const double x=s.x; s.x=a*x+b*s.y; s.y=c*x+d*s.y;
}

inline bool PassiveTail::eligible(const VesselEngine& e) noexcept {
    return e.fastTail_ && !e.audit_ && !e.active_ && !e.rotating_
        && e.engagement_==0 && e.bank_.additionalSigma_==0;
}

inline EngineFrame PassiveTail::advance(VesselEngine& e, unsigned factor) const noexcept {
    EngineFrame frame;
    for(unsigned k=0; k<factor; ++k) {
        e.speed_+=e.controlAlpha_*(e.targetSpeed_-e.speed_);
        e.pressure_+=e.controlAlpha_*(e.targetPressure_-e.pressure_);
    }
    e.previousFriction_=0;
    bool finite=true;
    for(std::size_t j=0; j<count_; ++j) {
        auto& state=e.bank_.states_[j]; const auto& m=modes_[j];
        m.host.step(state);
        const double v=m.filteredX*state.x+m.filteredY*state.y;
        frame.leftVelocity+=left_[j]*v; frame.rightVelocity+=right_[j]*v;
        finite=finite && std::isfinite(state.x) && std::isfinite(state.y);
    }
    if(!finite || !std::isfinite(e.compression_) || !std::isfinite(e.strikerVelocity_)
        || !std::isfinite(frame.leftVelocity) || !std::isfinite(frame.rightVelocity)) {
        e.bank_.clear(); e.active_=false; e.compression_=e.strikerVelocity_=0;
        ++e.ledger_.nonfiniteResets;
        frame.leftVelocity=frame.rightVelocity=0; frame.fault=true;
    }
    return frame;
}

inline StereoSample PassiveTail::stepBaseline() noexcept {
    StereoSample sample;
    for(std::size_t j=0; j<count_; ++j) {
        modes_[j].internal.step(states_[j]);
        sample.left+=left_[j]*states_[j].y; sample.right+=right_[j]*states_[j].y;
    }
    return sample;
}

inline StereoSample PassiveTail::filteredBaseline() const noexcept {
    StereoSample sample;
    for(std::size_t j=0; j<count_; ++j) {
        const auto& m=modes_[j]; const auto& s=states_[j];
        const double v=m.filteredX*s.x+m.filteredY*s.y;
        sample.left+=left_[j]*v; sample.right+=right_[j]*v;
    }
    return sample;
}
} // namespace vessel

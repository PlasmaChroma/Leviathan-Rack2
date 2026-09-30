#include "HostRateAdapter.hpp"

#include <algorithm>
#include <cmath>

namespace vessel {
StereoDecimator::StereoDecimator() noexcept {
    constexpr unsigned center = (taps-1)/2;
    double sum = 0.0;
    for (unsigned i = 0; i <= center; ++i) {
        const double n = double(i)-center;
        const double sinc = n == 0.0 ? 0.45 : std::sin(0.45*pi*n)/(pi*n);
        const double window = 0.42-0.5*std::cos(2*pi*i/(taps-1))+0.08*std::cos(4*pi*i/(taps-1));
        coefficients_[i] = sinc*window;
        sum += coefficients_[i]*(i == center ? 1.0 : 2.0);
    }
    for (auto& c : coefficients_) c /= sum;
}
bool StereoDecimator::configure(unsigned factor) noexcept {
    if (factor != 1 && factor != 2 && factor != 4 && factor != 8) return false;
    factor_ = factor;
    stageCount_ = factor == 8 ? 3 : factor == 4 ? 2 : factor == 2 ? 1 : 0;
    reset();
    return true;
}
void StereoDecimator::reset() noexcept { stages_ = {}; }
double StereoDecimator::latencyHostSamples() const noexcept {
    // Cascaded input-sample delay: center*(factor-1); output phase selects
    // input factor-1, so subtract that phase before converting to host tags.
    return double((taps-1)/2-1)*(factor_-1)/factor_;
}
double StereoDecimator::coefficient(unsigned tap) const noexcept {
    if (tap >= taps) return 0.0;
    return coefficients_[std::min(tap, taps-1-tap)];
}
bool StereoDecimator::pushStage(Stage& stage, const StereoSample& input, StereoSample& output) noexcept {
    stage.history[stage.position] = input;
    const unsigned newest = stage.position;
    if (++stage.position == taps) stage.position = 0;
    stage.phase ^= 1;
    if (stage.phase) return false;
    output = {};
    unsigned a = newest, b = stage.position;
    for (unsigned i = 0; i < (taps-1)/2; ++i) {
        output.left += coefficients_[i]*(stage.history[a].left+stage.history[b].left);
        output.right += coefficients_[i]*(stage.history[a].right+stage.history[b].right);
        a = a == 0 ? taps-1 : a-1;
        if (++b == taps) b = 0;
    }
    output.left += coefficients_.back()*stage.history[a].left;
    output.right += coefficients_.back()*stage.history[a].right;
    return true;
}
bool StereoDecimator::push(const StereoSample& input, StereoSample& output) noexcept {
    StereoSample current = input;
    for (unsigned i = 0; i < stageCount_; ++i) {
        StereoSample next;
        if (!pushStage(stages_[i], current, next)) return false;
        current = next;
    }
    output = current;
    return true;
}

unsigned HostRateAdapter::factorForRate(double rate) noexcept {
    if (!std::isfinite(rate) || rate < 32000.0 || rate > 192000.0) return 0;
    unsigned factor = 1;
    while (rate*factor < 176400.0 && factor < 8) factor *= 2;
    return factor;
}
bool HostRateAdapter::configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
                                const EngineSettings& settings, double rate) noexcept {
    const unsigned factor = factorForRate(rate);
    if (!factor || !engine_.configure(bowl, mallet, settings, rate*factor)) return false;
    if (rate != hostRate_) {
        decimator_.configure(factor);
        transitionGain_ = hostRate_ > 0.0 ? 0.0 : 1.0;
        transitionFrom_ = lastOutput_;
        transitionIncrement_ = 1.0/(0.005*rate);
        hostRate_ = rate;
    }
    return true;
}
void HostRateAdapter::reset() noexcept {
    engine_.reset();
    decimator_.reset();
    transitionGain_ = 1.0;
    lastOutput_ = {}; transitionFrom_ = {};
}
HostFrame HostRateAdapter::process(const HostControls& controls) noexcept {
    HostFrame output;
    if (hostRate_ == 0.0) { output.fault = true; return output; }
    const double speed = std::isfinite(controls.speed) ? std::max(-2.0, std::min(2.0, controls.speed)) : 0.0;
    const double pressure = std::isfinite(controls.pressure) ? std::max(0.0, std::min(15.0, controls.pressure)) : 0.0;
    engine_.setRotation(controls.rotate, speed, pressure);
    if (controls.strikeEvent) engine_.strike(controls.velocity);
    for (unsigned i = 0; i < decimator_.factor(); ++i) {
        const auto frame = engine_.step();
        output.fault = output.fault || frame.fault;
        StereoSample sample; sample.left = frame.leftVelocity; sample.right = frame.rightVelocity;
        decimator_.push(sample, output.audio);
    }
    output.bowlEnergy = engine_.bowl().energy();
    if (output.fault || !std::isfinite(output.audio.left) || !std::isfinite(output.audio.right)) {
        decimator_.reset(); output.audio = {}; output.fault = true;
        transitionGain_ = 0.0;
        transitionFrom_ = {};
    } else {
        transitionGain_ = std::min(1.0, transitionGain_+transitionIncrement_);
        // Old filter delays never get reinterpreted at the new rate. Hold
        // just the last finite output, blending it away over five milliseconds.
        output.audio.left = transitionFrom_.left*(1.0-transitionGain_)+output.audio.left*transitionGain_;
        output.audio.right = transitionFrom_.right*(1.0-transitionGain_)+output.audio.right*transitionGain_;
    }
    lastOutput_ = output.audio;
    return output;
}
} // namespace vessel

#include "HostRateAdapter.hpp"
#if defined(VESSEL_EXPERIMENTAL_FIR109)
#include "ExperimentalFir109.hpp"
#endif

#include <algorithm>
#include <cmath>
#if defined(__SSE2__) && !defined(VESSEL_SCALAR_FIR)
#include <emmintrin.h>
#endif

namespace vessel {
StereoDecimator::StereoDecimator() noexcept {
    #if defined(VESSEL_EXPERIMENTAL_FIR109)
    coefficients_ = experimentalFir109;
    #else
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
    #endif
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
    stage.history[stage.position+taps] = input;
    const unsigned newest = stage.position;
    if (++stage.position == taps) stage.position = 0;
    stage.phase ^= 1;
    if (stage.phase) return false;
    const StereoSample* newer = stage.history.data()+newest+taps;
    const StereoSample* older = stage.history.data()+newest+1;
#if defined(__SSE2__) && !defined(VESSEL_SCALAR_FIR)
    static_assert(sizeof(StereoSample) == 2*sizeof(double)
        && offsetof(StereoSample, right) == sizeof(double), "packed stereo FIR lanes");
    __m128d sum = _mm_setzero_pd();
    unsigned i = 0;
#if !defined(VESSEL_SERIAL_FIR)
    // Independent chains hide add latency. Only the output observer is
    // reassociated; the mechanical/contact reductions retain their order.
    // VESSEL_SERIAL_FIR keeps the original SSE2 order for strict tests/benchmarks.
    __m128d sum1 = sum, sum2 = sum, sum3 = sum;
    for (; i+3 < (taps-1)/2; i += 4) {
        const auto term = [&](unsigned offset) {
            const __m128d pair = _mm_add_pd(_mm_loadu_pd(&(newer-offset)->left),
                                           _mm_loadu_pd(&(older+offset)->left));
            return _mm_mul_pd(_mm_set1_pd(coefficients_[i+offset]), pair);
        };
        sum = _mm_add_pd(sum, term(0));
        sum1 = _mm_add_pd(sum1, term(1));
        sum2 = _mm_add_pd(sum2, term(2));
        sum3 = _mm_add_pd(sum3, term(3));
        newer -= 4; older += 4;
    }
    sum = _mm_add_pd(_mm_add_pd(sum, sum1), _mm_add_pd(sum2, sum3));
#endif
    for (; i < (taps-1)/2; ++i) {
        const __m128d pair = _mm_add_pd(_mm_loadu_pd(&newer->left), _mm_loadu_pd(&older->left));
        sum = _mm_add_pd(sum, _mm_mul_pd(_mm_set1_pd(coefficients_[i]), pair));
        --newer; ++older;
    }
    sum = _mm_add_pd(sum, _mm_mul_pd(_mm_set1_pd(coefficients_.back()), _mm_loadu_pd(&newer->left)));
    _mm_storeu_pd(&output.left, sum);
#else
    output = {};
    for (unsigned i = 0; i < (taps-1)/2; ++i) {
        output.left += coefficients_[i]*(newer->left+older->left);
        output.right += coefficients_[i]*(newer->right+older->right);
        --newer; ++older;
    }
    output.left += coefficients_.back()*newer->left;
    output.right += coefficients_.back()*newer->right;
#endif
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

unsigned HostRateAdapter::factorForRate(double rate, ProcessingQuality quality) noexcept {
    if (!std::isfinite(rate) || rate < 32000.0 || rate > 192000.0) return 0;
    const double minimum = quality == ProcessingQuality::Economy ? 44100.0
        : quality == ProcessingQuality::Balanced ? 88200.0 : 176400.0;
    unsigned factor = 1;
    while (rate*factor < minimum && factor < 8) factor *= 2;
    return factor;
}
bool HostRateAdapter::configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
                                const EngineSettings& settings, double rate, ProcessingQuality quality) noexcept {
    PreparedConfiguration next;
    const unsigned maximum = factorForRate(rate);
    for (unsigned factor = factorForRate(rate, quality); factor && factor <= maximum; factor *= 2) {
        if (!prepareConfiguration(bowl, mallet, settings, rate, next, factor)) continue;
        applyConfiguration(bowl, mallet, settings, rate, next);
        return true;
    }
    return false;
}
bool HostRateAdapter::prepareConfiguration(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
    const EngineSettings& settings, double rate, PreparedConfiguration& next, unsigned factor) const noexcept {
    next.factor = factor;
    return next.factor && engine_.prepareConfiguration(bowl, mallet, settings, rate*next.factor, next.engine);
}
void HostRateAdapter::applyConfiguration(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
    const EngineSettings& settings, double rate, const PreparedConfiguration& next) noexcept {
    engine_.applyConfiguration(bowl, mallet, settings, next.engine);
    if (rate != hostRate_ || next.factor != decimator_.factor()) {
        decimator_.configure(next.factor);
        transitionGain_ = hostRate_ > 0.0 ? 0.0 : 1.0;
        transitionFrom_ = lastOutput_;
        transitionIncrement_ = 1.0/(0.005*rate);
        hostRate_ = rate;
    }
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
    if (controls.strikeEvent) engine_.strike(controls.velocity, controls.strikeVelocityScale);
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

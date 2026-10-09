#pragma once

#include "Types.hpp"
#include <cmath>
#include <limits>
#if defined(__SSE2__)
#include <emmintrin.h>
#endif

namespace vessel {

class ModalBank {
public:
    // Setup/control boundary only: expensive transforms are cached here.
    // Failure leaves both coefficients and the persistent state untouched.
    bool configure(const BowlDescriptor& bowl, double frequency, double decayMultiplier,
                   double imperfection, double internalRate) noexcept;
    void clear() noexcept;
    std::size_t size() const noexcept { return count_; }
    double timeStep() const noexcept { return h_; }
    const ModeCoefficients& coefficients(std::size_t i) const noexcept { return coefficients_[i]; }
    const ModalState& state(std::size_t i) const noexcept { return states_[i]; }
    bool setState(std::size_t i, double x, double y) noexcept;
    double energy() const noexcept;
    bool finite() const noexcept;
    // Control-rate extra viscous loss. Base coefficients remain the linear
    // specimen; only hot damping/admittance change, never state or stiffness.
    bool setAdditionalDamping(double sigma) noexcept;

    // Fixed unforced step, with unchanged scalar arithmetic and pickup order.
    // Filtering remains in the host adapter at the original internal cadence.
    bool advanceFree(const ModalVector& left, const ModalVector& right,
                     double& leftVelocity, double& rightVelocity) noexcept;
    ModalVector freeMidpoint() const noexcept;
    double velocity(const ModalVector& port) const noexcept;
    double midpointVelocity(const ModalVector& port, const ModalVector& free) const noexcept;
    double admittance(const ModalVector& a, const ModalVector& b) const noexcept;
    ModalStepAudit commit(const ModalVector& free, const ModalVector& force,
                         bool audit = false) noexcept;
    // Active-contact counterpart of advanceFree: retain each scalar operation
    // and reduction order while reading updated states only once.
    bool commitObserved(const ModalVector& free, const ModalVector& force,
                        const ModalVector& left, const ModalVector& right,
                        double& leftVelocity, double& rightVelocity) noexcept;

    // Shapes use outward radial and increasing-angle tangential coordinates.
    // The outside striker uses inward=true for both injection and observation.
    ModalVector radialPort(double angle, double patchWidth, bool inward = false) const noexcept;
    ModalVector tangentialPort(double angle, double patchWidth) const noexcept;
    ModalVector observer(double angle) const noexcept;

private:
    friend class PassiveTail;
    std::array<ModeCoefficients, maxModes> coefficients_ {};
    std::array<ModalState, maxModes> states_ {};
    std::array<ModePairDescriptor, maxPairs> pairs_ {};
    // Contiguous coefficients for the audio loops; preserve scalar operation order.
    ModalVector hotA_ {}, hotInverseD_ {}, hotWeight_ {}, hotOmega_ {}, hotSigma_ {};
    std::size_t count_ = 0;
    double h_ = 1.0 / 192000.0;
    double additionalSigma_ = 0.0;
};

// Small numerical loops are visible to the strict-math audio compiler;
// coefficient generation remains in the .cpp.
inline ModalVector ModalBank::freeMidpoint() const noexcept {
    ModalVector free {};
    for (std::size_t j = 0; j < count_; ++j)
        free[j] = (states_[j].y - hotA_[j]*states_[j].x)*hotInverseD_[j];
    return free;
}
inline double ModalBank::velocity(const ModalVector& port) const noexcept {
    double v = 0.0;
    for (std::size_t j = 0; j < count_; ++j) v += port[j]*states_[j].y;
    return v;
}
inline double ModalBank::midpointVelocity(const ModalVector& port, const ModalVector& free) const noexcept {
    double v = 0.0;
    for (std::size_t j = 0; j < count_; ++j) v += port[j]*free[j];
    return v;
}
inline double ModalBank::admittance(const ModalVector& a, const ModalVector& b) const noexcept {
    double y = 0.0;
    for (std::size_t j = 0; j < count_; ++j) y += a[j]*b[j]*hotWeight_[j];
    return y;
}
inline ModalStepAudit ModalBank::commit(const ModalVector& free, const ModalVector& force, bool audit) noexcept {
    ModalStepAudit result;
    if (audit) result.energyBefore = energy();
    for (std::size_t j = 0; j < count_; ++j) {
        auto& s = states_[j];
        const double mid = free[j] + hotWeight_[j]*force[j];
        s.x += h_*hotOmega_[j]*mid;
        s.y = 2.0*mid - s.y;
        if (audit) {
            result.work += h_*force[j]*mid;
            result.dampingLoss += 2.0*h_*hotSigma_[j]*mid*mid;
        }
    }
    if (audit) {
        result.energyAfter = energy();
        result.residual = result.energyAfter-result.energyBefore-result.work+result.dampingLoss;
    }
    return result;
}
inline bool ModalBank::commitObserved(const ModalVector& free, const ModalVector& force,
    const ModalVector& left, const ModalVector& right,
    double& leftVelocity, double& rightVelocity) noexcept {
    leftVelocity = rightVelocity = 0.0;
    bool valid = true;
    std::size_t j = 0;
#if defined(__SSE2__)
    const __m128d h = _mm_set1_pd(h_);
    const __m128d two = _mm_set1_pd(2.0);
    const __m128d sign = _mm_set1_pd(-0.0);
    const __m128d maximum = _mm_set1_pd(std::numeric_limits<double>::max());
    __m128d finiteMask = _mm_castsi128_pd(_mm_set1_epi32(-1));
    for (; j + 1 < count_; j += 2) {
        const __m128d old0 = _mm_loadu_pd(&states_[j].x);
        const __m128d old1 = _mm_loadu_pd(&states_[j+1].x);
        const __m128d x = _mm_unpacklo_pd(old0, old1);
        const __m128d y = _mm_unpackhi_pd(old0, old1);
        const __m128d mid = _mm_add_pd(_mm_loadu_pd(free.data()+j),
            _mm_mul_pd(_mm_loadu_pd(hotWeight_.data()+j), _mm_loadu_pd(force.data()+j)));
        const __m128d nextX = _mm_add_pd(x,
            _mm_mul_pd(_mm_mul_pd(h, _mm_loadu_pd(hotOmega_.data()+j)), mid));
        const __m128d nextY = _mm_sub_pd(_mm_mul_pd(two, mid), y);
        _mm_storeu_pd(&states_[j].x, _mm_unpacklo_pd(nextX, nextY));
        _mm_storeu_pd(&states_[j+1].x, _mm_unpackhi_pd(nextX, nextY));
        finiteMask = _mm_and_pd(finiteMask, _mm_and_pd(
            _mm_cmple_pd(_mm_andnot_pd(sign, nextX), maximum),
            _mm_cmple_pd(_mm_andnot_pd(sign, nextY), maximum)));
        const __m128d l = _mm_mul_pd(_mm_loadu_pd(left.data()+j), nextY);
        const __m128d r = _mm_mul_pd(_mm_loadu_pd(right.data()+j), nextY);
        // Accumulate in original mode order: no horizontal reassociation.
        leftVelocity += _mm_cvtsd_f64(l);
        leftVelocity += _mm_cvtsd_f64(_mm_unpackhi_pd(l, l));
        rightVelocity += _mm_cvtsd_f64(r);
        rightVelocity += _mm_cvtsd_f64(_mm_unpackhi_pd(r, r));
    }
    valid = _mm_movemask_pd(finiteMask) == 3;
#endif
    for (; j < count_; ++j) {
        auto& s = states_[j];
        const double mid = free[j] + hotWeight_[j]*force[j];
        s.x += h_*hotOmega_[j]*mid;
        s.y = 2.0*mid - s.y;
        valid = valid && std::isfinite(s.x) && std::isfinite(s.y);
        leftVelocity += left[j]*s.y;
        rightVelocity += right[j]*s.y;
    }
    return valid;
}
} // namespace vessel

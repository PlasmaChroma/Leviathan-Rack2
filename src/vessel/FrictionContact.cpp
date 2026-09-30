#include "FrictionContact.hpp"
#include "StrikeContact.hpp"

#include <algorithm>
#include <cmath>

namespace vessel {

FrictionValue frictionValue(double slip, double load, const MalletDescriptor& m) noexcept {
    FrictionValue value;
    if (load == 0.0) return value;
    const double z = slip/m.weakeningVelocity;
    const double weakening = std::abs(z) < 27.0 ? std::exp(-z*z) : 0.0;
    const double mu = m.muK+(m.muS-m.muK)*weakening;
    const double a = slip/m.regularizationVelocity;
    const double t = std::abs(a) < 20.0 ? std::tanh(a) : (a < 0.0 ? -1.0 : 1.0);
    const double slope = weakening == 0.0 ? 0.0 : -2.0*z/m.weakeningVelocity*(m.muS-m.muK)*weakening;
    value.force = load*mu*t;
    value.derivative = load*(slope*t+mu/m.regularizationVelocity*(1.0-t*t));
    return value;
}
double frictionNegativeSlopeBound(double load, const MalletDescriptor& m) noexcept {
    return load*0.8577638849607068*(m.muS-m.muK)/m.weakeningVelocity;
}
FrictionSolution solveFriction(double delta, double Y, double load,
                               const MalletDescriptor& m, double previous) noexcept {
    FrictionSolution result;
    if (!validMallet(m) || !std::isfinite(delta) || !std::isfinite(Y) || Y < 0.0
        || !std::isfinite(load) || load < 0.0 || !std::isfinite(previous)
        || Y*frictionNegativeSlopeBound(load, m) > 0.9) return result;
    if (load == 0.0) {
        result.slip = delta;
        result.converged = true;
        return result;
    }
    double lo = -load*m.muS, hi = load*m.muS;
    double force = std::max(lo, std::min(hi, previous));
    const double tolerance = 1e-12+1e-11*std::max(1.0, hi);
    for (unsigned i = 0; i < 80; ++i) {
        result.force = force;
        result.slip = delta-Y*force;
        const auto law = frictionValue(result.slip, load, m);
        result.derivative = law.derivative;
        result.residual = force-law.force;
        result.iterations = i+1;
        if (!std::isfinite(result.residual) || !std::isfinite(result.derivative)) return result;
        if (std::abs(result.residual) <= tolerance) {
            result.converged = true;
            return result;
        }
        if (result.residual > 0.0) hi = force;
        else lo = force;
        const double candidate = force-result.residual/(1.0+Y*law.derivative);
        // Bound Newton's work near sharp adhesion transitions. A merely
        // in-bracket step can crawl along a bracket edge indefinitely.
        force = i < 12 && std::isfinite(candidate) && candidate > lo && candidate < hi
            ? candidate : 0.5*(lo+hi);
    }
    return result;
}

void ContactOrbit::configure(const BowlDescriptor& bowl, const ModalBank& bank,
                             double width, double angle) noexcept {
    pairs_ = bowl.pairCount;
    ticks_ = 0;
    incrementCached_ = false;
    for (std::size_t n = 0; n < pairs_; ++n) {
        const auto& p = bowl.pairs[n];
        orders_[n] = p.order;
        const double beta = p.order*(angle-p.orientation);
        cosine_[n] = std::cos(beta);
        sine_[n] = std::sin(beta);
        const double half = 0.5*p.order*width;
        const double patch = std::abs(half) < 1e-8 ? 1.0-half*half/6.0 : std::sin(half)/half;
        gains_[2*n] = patch*bank.coefficients(2*n).inverseRootMass;
        gains_[2*n+1] = patch*bank.coefficients(2*n+1).inverseRootMass;
        tangentGains_[2*n] = gains_[2*n]/p.order;
        tangentGains_[2*n+1] = gains_[2*n+1]/p.order;
    }
}
void ContactOrbit::rotate() noexcept {
    for (std::size_t n = 0; n < pairs_; ++n) {
        const double c = cosine_[n], s = sine_[n];
        cosine_[n] = c*dc_[n]-s*ds_[n];
        sine_[n] = s*dc_[n]+c*ds_[n];
    }
}
void ContactOrbit::midpoint(double increment, ModalVector& tangent, ModalVector& inward, bool includeNormal) noexcept {
    tangent = {}; inward = {};
    if (!incrementCached_ || increment != cachedIncrement_) {
        cachedIncrement_ = increment; incrementCached_ = true;
        for (std::size_t n = 0; n < pairs_; ++n) {
            const double d = 0.5*orders_[n]*increment, d2 = d*d;
            if (std::abs(d) < 0.01) {
                ds_[n] = d*(1.0-d2/6.0+d2*d2/120.0);
                dc_[n] = 1.0-d2/2.0+d2*d2/24.0-d2*d2*d2/720.0;
            } else {
                ds_[n] = std::sin(d); dc_[n] = std::cos(d);
            }
        }
    }
    rotate();
    for (std::size_t n = 0; n < pairs_; ++n) {
        tangent[2*n] = -tangentGains_[2*n]*sine_[n];
        tangent[2*n+1] = tangentGains_[2*n+1]*cosine_[n];
        if (includeNormal) {
            inward[2*n] = -gains_[2*n]*cosine_[n];
            inward[2*n+1] = -gains_[2*n+1]*sine_[n];
        }
    }
}
void ContactOrbit::finish() noexcept {
    rotate();
    if (++ticks_ == 4096) {
        ticks_ = 0;
        for (std::size_t n = 0; n < pairs_; ++n) {
            const double norm = 1.0/std::sqrt(cosine_[n]*cosine_[n]+sine_[n]*sine_[n]);
            cosine_[n] *= norm; sine_[n] *= norm;
        }
    }
}

} // namespace vessel

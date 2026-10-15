#pragma once
// Offline prototype only. Shared generated tables; no audio-thread construction.
#include "vessel/FrictionContact.hpp"
#include "FrictionTables.hpp"
#include <algorithm>
#include <cmath>
namespace vessel {
namespace friction_approximation {
struct Curve { double value, derivative; };
inline Curve lookup(const double (*coefficients)[4], double x, double invStep, unsigned cells) noexcept {
    const double index = x*invStep;
    // Rounding can map nextafter(20,0)*102.4 to the final boundary.
    const unsigned cell = std::min(unsigned(index),cells-1);
    const double u = index-cell;
    const auto* c = coefficients[cell];
    return {c[0]+u*(c[1]+u*(c[2]+u*c[3])),
        (c[1]+u*(2*c[2]+u*3*c[3]))*invStep};
}
}
inline FrictionValue frictionValueApproximate(double slip, double load, const MalletDescriptor& m) noexcept {
    FrictionValue value;
    if (load == 0.0) return value;
    const double z = slip/m.weakeningVelocity;
    const double a = slip/m.regularizationVelocity;
    const friction_approximation::Curve g = std::abs(z)<8 ? friction_approximation::lookup(friction_tables::gaussian,std::abs(z),128,1024) : friction_approximation::Curve{0,0};
    const friction_approximation::Curve t = std::abs(a)<20 ? friction_approximation::lookup(friction_tables::tanhCurve,std::abs(a),102.4,2048) : friction_approximation::Curve{1,0};
    const double direction = slip<0 ? -1.0 : 1.0;
    const double mu = m.muK+(m.muS-m.muK)*g.value;
    const double slope = direction*g.derivative/m.weakeningVelocity*(m.muS-m.muK);
    value.force = load*mu*(direction*t.value);
    value.derivative = load*(slope*(direction*t.value)+mu/m.regularizationVelocity*t.derivative);
    return value;
}
// Narrower experiment: preserve analytic Gaussian weakening, replace only tanh.
inline FrictionValue frictionValueTanhApproximate(double slip, double load, const MalletDescriptor& m) noexcept {
    FrictionValue value;
    if (load == 0.0) return value;
    const double z = slip/m.weakeningVelocity;
    const double weakening = std::abs(z)<27 ? std::exp(-z*z) : 0.0;
    const double mu = m.muK+(m.muS-m.muK)*weakening;
    const double a = slip/m.regularizationVelocity;
    const friction_approximation::Curve t = std::abs(a)<20
        ? friction_approximation::lookup(friction_tables::tanhCurve,std::abs(a),102.4,2048)
        : friction_approximation::Curve{1,0};
    const double signedT = slip<0 ? -t.value : t.value;
    const double slope = weakening==0 ? 0.0 : -2*z/m.weakeningVelocity*(m.muS-m.muK)*weakening;
    value.force = load*mu*signedT;
    value.derivative = load*(slope*signedT+mu/m.regularizationVelocity*t.derivative);
    return value;
}
inline double frictionApproximateNegativeSlopeBound(double load, const MalletDescriptor& m) noexcept {
    return load*friction_tables::gaussianNegativeSlopeBound*(m.muS-m.muK)/m.weakeningVelocity
        +load*m.muS/m.regularizationVelocity*friction_tables::tanhNegativeSlopeBound;
}
// Isolate Gaussian lookup cost while preserving analytic tanh.
inline FrictionValue frictionValueGaussianApproximate(double slip, double load, const MalletDescriptor& m) noexcept {
    FrictionValue value;
    if (load == 0.0) return value;
    const double z = slip/m.weakeningVelocity;
    const auto g = std::abs(z)<8
        ? friction_approximation::lookup(friction_tables::gaussian,std::abs(z),128,1024)
        : friction_approximation::Curve{0,0};
    const double a = slip/m.regularizationVelocity;
    const double t = std::abs(a)<20 ? std::tanh(a) : (a<0 ? -1. : 1.);
    const double mu = m.muK+(m.muS-m.muK)*g.value;
    const double slope = (slip<0 ? -1. : 1.)*g.derivative/m.weakeningVelocity*(m.muS-m.muK);
    value.force = load*mu*t;
    value.derivative = load*(slope*t+mu/m.regularizationVelocity*(1.-t*t));
    return value;
}

// Prepared once per solve, not globally cached: an isolated arithmetic trial.
// Multiplying by reciprocals changes rounding. Keep the analytic functions.
struct FrictionReciprocals {
    double weakening, regularization, deltaMu, muK;
    explicit FrictionReciprocals(const MalletDescriptor& m) noexcept
        : weakening(1./m.weakeningVelocity), regularization(1./m.regularizationVelocity),
          deltaMu(m.muS-m.muK), muK(m.muK) {}
    template<bool fastTanh = false>
    FrictionValue evaluate(double slip, double load) const noexcept {
        FrictionValue value;
        if (load == 0.) return value;
        const double z=slip*weakening, a=slip*regularization;
        const double g=std::abs(z)<27 ? std::exp(-z*z) : 0.;
        double t, dt;
        if (fastTanh) {
            const auto curve=std::abs(a)<20
                ? friction_approximation::lookup(friction_tables::tanhCurve,std::abs(a),102.4,2048)
                : friction_approximation::Curve{1,0};
            t=a<0 ? -curve.value : curve.value; dt=curve.derivative;
        } else {
            t=std::abs(a)<20 ? std::tanh(a) : (a<0 ? -1. : 1.);
            dt=1.-t*t;
        }
        const double mu=muK+deltaMu*g;
        const double slope=g==0 ? 0. : -2.*z*weakening*deltaMu*g;
        value.force=load*mu*t;
        value.derivative=load*(slope*t+mu*regularization*dt);
        return value;
    }
};
} // namespace vessel

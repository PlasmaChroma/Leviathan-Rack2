#pragma once
#include "FrictionContact.hpp"
#include "FrictionTanhTable.hpp"
#include "ContactWorkProfile.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace vessel {
// Construct after descriptor validation, once per solve. The analytic public
// frictionValue() remains an independent reference. Force and derivative below
// share one monotone cubic tanh; Gaussian weakening remains analytic.
class PreparedFrictionLaw {
    double weakening_, regularization_, deltaMu_, muK_;
    const MalletDescriptor& descriptor_;
    bool analyticFallback_;
public:
    // The descriptor must outlive this per-solve object. Subnormal scales use
    // the reference law to avoid overflowing cached reciprocals.
    explicit PreparedFrictionLaw(const MalletDescriptor& m) noexcept
        : weakening_(m.weakeningVelocity>=std::numeric_limits<double>::min() ? 1./m.weakeningVelocity : 0.),
          regularization_(m.regularizationVelocity>=std::numeric_limits<double>::min() ? 1./m.regularizationVelocity : 0.),
          deltaMu_(m.muS-m.muK), muK_(m.muK), descriptor_(m),
          analyticFallback_(m.weakeningVelocity<std::numeric_limits<double>::min()
              || m.regularizationVelocity<std::numeric_limits<double>::min()) {}
    FrictionValue evaluate(double slip, double load) const noexcept {
        if (analyticFallback_) return frictionValue(slip,load,descriptor_);
        VESSEL_WORK(++contactWork().friction().lawEvaluations);
        FrictionValue value;
        if (load == 0.) return value;
        const double z=slip*weakening_, a=slip*regularization_;
        VESSEL_WORK(contactWork().friction().gaussianCore += std::abs(z)<1.);
        VESSEL_WORK(contactWork().friction().gaussianTail += std::abs(z)>=8. && std::abs(z)<27.);
        VESSEL_WORK(contactWork().friction().expEvaluations += std::abs(z)<27.);
        const double g=std::abs(z)<27 ? std::exp(-z*z) : 0.;
        double t=1., dt=0.;
        if (std::abs(a)<20) {
            VESSEL_WORK(++contactWork().friction().tanhLookups);
            const double index=std::abs(a)*102.4;
            const unsigned cell=std::min(unsigned(index),2047u);
            const double u=index-cell;
            const auto* c=friction_tanh::tanhCurve[cell];
            t=c[0]+u*(c[1]+u*(c[2]+u*c[3]));
            dt=(c[1]+u*(2*c[2]+u*3*c[3]))*102.4;
        }
        if (a<0) t=-t;
        const double mu=muK_+deltaMu_*g;
        const double slope=g==0 ? 0. : -2.*z*weakening_*deltaMu_*g;
        value.force=load*mu*t;
        value.derivative=load*(slope*t+mu*regularization_*dt);
        return value;
    }
};
inline double preparedFrictionNegativeSlopeBound(double load, const MalletDescriptor& m) noexcept {
    if (m.weakeningVelocity<std::numeric_limits<double>::min()
        || m.regularizationVelocity<std::numeric_limits<double>::min())
        return load*0.8577638849607068*(m.muS-m.muK)/m.weakeningVelocity;
    return load*friction_tanh::gaussianNegativeSlopeBound*(m.muS-m.muK)/m.weakeningVelocity
        +load*m.muS/m.regularizationVelocity*friction_tanh::tanhNegativeSlopeBound;
}
} // namespace vessel

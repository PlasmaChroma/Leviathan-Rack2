#include "PassiveTail.hpp"
#include <cmath>

namespace vessel {
PassiveTail::Matrix PassiveTail::Matrix::times(const Matrix& r) const noexcept {
    Matrix m;
    m.a=a*r.a+b*r.c; m.b=a*r.b+b*r.d;
    m.c=c*r.a+d*r.c; m.d=c*r.b+d*r.d;
    return m;
}
PassiveTail::Matrix PassiveTail::Matrix::power(unsigned n) const noexcept {
    Matrix out, base=*this;
    while(n) { if(n&1) out=out.times(base); base=base.times(base); n>>=1; }
    return out;
}
PassiveTail::Matrix PassiveTail::Matrix::inverse() const noexcept {
    const double k=1/(a*d-b*c);
    Matrix m; m.a=d*k; m.b=-b*k; m.c=-c*k; m.d=a*k; return m;
}
void PassiveTail::prepare(const VesselEngine& engine, const StereoDecimator& filter) noexcept {
    if(ready_ || filter.factor()==1) return;
    const auto& bank=engine.bank_;
    if(count_==0) {
        count_=bank.size(); left_=engine.observerL_; right_=engine.observerR_;
    }
    const unsigned stages=filter.factor()==8?3:filter.factor()==4?2:1;
    for(unsigned work=0; work<preparationTaps && mode_<count_; ++work) {
        if(tap_==0) {
            if(stage_==0) {
                const auto& c=bank.coefficients(mode_); auto& m=modes_[mode_].internal;
                m.a=1-2*c.a*c.a*c.inverseD; m.b=2*c.a*c.inverseD;
                m.c=-2*c.a*c.inverseD; m.d=2*c.inverseD-1;
                modes_[mode_].host=m.power(filter.factor()); inverse_=m.inverse(); response_={};
            }
            stageStep_=inverse_.power(1u<<stage_); power_={};
            filterSum_.a=filterSum_.b=filterSum_.c=filterSum_.d=0;
        }
        const double k=filter.coefficient(tap_);
        filterSum_.a+=k*power_.a; filterSum_.b+=k*power_.b;
        filterSum_.c+=k*power_.c; filterSum_.d+=k*power_.d;
        power_=power_.times(stageStep_);
        if(++tap_==StereoDecimator::taps) {
            response_=response_.times(filterSum_); tap_=0;
            if(++stage_==stages) {
                modes_[mode_].filteredX=response_.c; modes_[mode_].filteredY=response_.d;
                stage_=0; ++mode_;
            }
        }
    }
    ready_=mode_==count_;
}
void PassiveTail::capture(const PassiveTail& model, const VesselEngine& engine) noexcept {
    // Copy only the completed model, not the incremental builder's scratch.
    count_=model.count_; modes_=model.modes_; left_=model.left_; right_=model.right_;
    states_=engine.bank_.states_;
}
} // namespace vessel

#pragma once
// Offline prototype only. The runner grants access in a temporary core copy.
#include "vessel/HostRateAdapter.hpp"
#include <algorithm>
#include <cmath>

namespace vessel {
class ComposedTailHost {
    struct Matrix {
        double a=1,b=0,c=0,d=1;
        Matrix times(const Matrix& r) const {
            Matrix m; m.a=a*r.a+b*r.c; m.b=a*r.b+b*r.d;
            m.c=c*r.a+d*r.c; m.d=c*r.b+d*r.d; return m;
        }
        Matrix power(unsigned n) const {
            Matrix out,base=*this;
            while(n) { if(n&1)out=out.times(base); base=base.times(base); n>>=1; }
            return out;
        }
        Matrix inverse() const {
            const double k=1/(a*d-b*c); Matrix m;
            m.a=d*k;m.b=-b*k;m.c=-c*k;m.d=a*k;return m;
        }
        void step(double& x,double& y) const {
            const double old=x;x=a*x+b*y;y=c*old+d*y;
        }
    };
    struct Row { double x=0,y=1; };
    struct Mode {
        Matrix host,internal;
        Row filtered;
    };
    std::array<Mode,maxModes> modes_ {};
    HostRateAdapter host_;
    ModalVector left_ {},right_ {};
    unsigned warm_=0,required_=0,stages_=0;
    bool running_=false;
    std::uint64_t composed_=0,restores_=0;
    StereoDecimator baselineFilter_;
    std::array<Mode,maxModes> baselineModes_ {};
    std::array<ModalState,maxModes> baselineStates_ {};
    ModalVector baselineLeft_ {},baselineRight_ {};
    std::size_t baselineSize_=0;
    unsigned handoffRemaining_=0;
    std::uint64_t handoffFrames_=0;

    bool eligible(const HostControls& c) const {
        const auto& e=host_.engine_;
        return host_.factor()>1 && !c.rotate && !c.strikeEvent && !e.audit_
            && !e.active_ && !e.rotating_ && e.engagement_==0
            && e.bank_.additionalSigma_==0;
    }
    void prepare() {
        const auto& bank=host_.engine_.bank_;
        left_=host_.engine_.observerL_;right_=host_.engine_.observerR_;
        stages_=host_.decimator_.stageCount_;
        std::array<double,StereoDecimator::taps> coefficients;
        for(unsigned t=0;t<StereoDecimator::taps;++t)coefficients[t]=host_.decimator_.coefficient(t);
        // Evaluate each FIR stage as a polynomial of the backward modal
        // transition. Products compose the dilated cascade without expanding
        // its 897-tap equivalent kernel or performing a convolution in setup.
        for(std::size_t j=0;j<bank.size();++j) {
            const auto& c=bank.coefficients(j);Matrix m;
            m.a=1-2*c.a*c.a*c.inverseD;m.b=2*c.a*c.inverseD;
            m.c=-2*c.a*c.inverseD;m.d=2*c.inverseD-1;
            const Matrix inverse=m.inverse();modes_[j].host=m.power(host_.factor());modes_[j].internal=m;
            Matrix response;
            for(unsigned stage=0;stage<stages_;++stage) {
                const Matrix step=inverse.power(1u<<stage);
                Matrix power,filter;filter.a=filter.b=filter.c=filter.d=0;
                for(unsigned t=0;t<StereoDecimator::taps;++t) {
                    const double k=coefficients[t];
                    filter.a+=k*power.a;filter.b+=k*power.b;
                    filter.c+=k*power.c;filter.d+=k*power.d;
                    power=power.times(step);
                }
                response=response.times(filter);
            }
            modes_[j].filtered.x=response.c;modes_[j].filtered.y=response.d;
        }
        const unsigned length=128*(host_.factor()-1)+1;
        required_=(length+host_.factor()-1)/host_.factor();warm_=0;
    }
    void beginHandoff() {
        if(!running_)return;
        // Both zero-history filters share the host boundary phase. Their
        // difference removes the fictitious continuation, leaving precisely
        // the old history contribution supplied by the analytic observer.
        const auto& bank=host_.engine_.bank_;
        baselineStates_=bank.states_;baselineSize_=bank.size();
        baselineModes_=modes_;baselineLeft_=left_;baselineRight_=right_;
        baselineFilter_.configure(host_.factor());host_.decimator_.reset();
        handoffRemaining_=required_;
        running_=false;warm_=0;++restores_;
    }
    HostFrame handoffProcess(const HostControls& controls) {
        // Mirror HostRateAdapter processing, inserting the linear history
        // correction before its existing output transition/fault handling.
        HostFrame out;auto& e=host_.engine_;
        const double speed=std::isfinite(controls.speed)?std::max(-2.0,std::min(2.0,controls.speed)):0;
        const double pressure=std::isfinite(controls.pressure)?std::max(0.0,std::min(15.0,controls.pressure)):0;
        e.setRotation(controls.rotate,speed,pressure);
        if(controls.strikeEvent)e.strike(controls.velocity,controls.strikeVelocityScale);
        StereoSample baselineFiltered;
        for(unsigned k=0;k<host_.factor();++k) {
            const auto frame=e.step();out.fault=out.fault||frame.fault;
            StereoSample actual;actual.left=frame.leftVelocity;actual.right=frame.rightVelocity;
            host_.decimator_.push(actual,out.audio);
            StereoSample baseline;
            for(std::size_t j=0;j<baselineSize_;++j) {
                auto& state=baselineStates_[j];baselineModes_[j].internal.step(state.x,state.y);
                baseline.left+=baselineLeft_[j]*state.y;baseline.right+=baselineRight_[j]*state.y;
            }
            baselineFilter_.push(baseline,baselineFiltered);
        }
        // After the finite impulse response has drained, the actual filter
        // contains only real post-boundary samples; no correction is needed.
        if(--handoffRemaining_) {
            StereoSample analytic;
            for(std::size_t j=0;j<baselineSize_;++j) {
                const auto& state=baselineStates_[j];const auto& row=baselineModes_[j].filtered;
                const double v=row.x*state.x+row.y*state.y;
                analytic.left+=baselineLeft_[j]*v;analytic.right+=baselineRight_[j]*v;
            }
            out.audio.left+=analytic.left-baselineFiltered.left;
            out.audio.right+=analytic.right-baselineFiltered.right;
        }
        ++handoffFrames_;out.bowlEnergy=e.bowl().energy();
        if(out.fault || !std::isfinite(out.audio.left) || !std::isfinite(out.audio.right)) {
            host_.decimator_.reset();handoffRemaining_=0;warm_=0;
            out.audio={};out.fault=true;host_.transitionGain_=0;host_.transitionFrom_={};
        } else {
            host_.transitionGain_=std::min(1.0,host_.transitionGain_+host_.transitionIncrement_);
            out.audio.left=host_.transitionFrom_.left*(1-host_.transitionGain_)+out.audio.left*host_.transitionGain_;
            out.audio.right=host_.transitionFrom_.right*(1-host_.transitionGain_)+out.audio.right*host_.transitionGain_;
        }
        host_.lastOutput_=out.audio;return out;
    }

public:
    bool configure(const BowlDescriptor& bowl,const MalletDescriptor& mallet,
        const EngineSettings& settings,double rate,ProcessingQuality q) {
        auto validated=host_;
        if(!validated.configure(bowl,mallet,settings,rate,q))return false;
        beginHandoff();
        const auto oldFactor=host_.factor();const double oldRate=host_.hostRate();
        if(!host_.configure(bowl,mallet,settings,rate,q))return false;
        if(oldFactor!=host_.factor() || oldRate!=host_.hostRate())handoffRemaining_=0;
        prepare();return true;
    }
    void reset() { running_=false;handoffRemaining_=0;host_.reset();warm_=0; }
    void updateHighEnergyDamping() {
        // A change in recurrence invalidates the homogeneous history model.
        const double excess=std::min(1e6,std::max(0.0,(host_.engine_.bank_.energy()-.04)/.04));
        const double square=excess*excess;
        if(.5*square/(1+square)!=host_.engine_.bank_.additionalSigma_)beginHandoff();
        host_.engine_.updateHighEnergyDamping();
    }
    HostFrame process(const HostControls& controls) {
        if(!eligible(controls)) {beginHandoff();warm_=0;}
        if(handoffRemaining_)return handoffProcess(controls);
        if(!eligible(controls))return host_.process(controls);
        if(warm_<required_) { ++warm_;return host_.process(controls); }
        auto& e=host_.engine_;auto& bank=e.bank_;
        running_=true;++composed_;
        const double speed=std::isfinite(controls.speed)?std::max(-2.0,std::min(2.0,controls.speed)):0;
        const double pressure=std::isfinite(controls.pressure)?std::max(0.0,std::min(15.0,controls.pressure)):0;
        e.setRotation(false,speed,pressure);
        for(unsigned k=0;k<host_.factor();++k) {
            e.speed_+=e.controlAlpha_*(e.targetSpeed_-e.speed_);
            e.pressure_+=e.controlAlpha_*(e.targetPressure_-e.pressure_);
        }
        e.previousFriction_=0;
        HostFrame out;bool valid=true;
        for(std::size_t j=0;j<bank.size();++j) {
            auto& state=bank.states_[j];const auto& m=modes_[j];
            m.host.step(state.x,state.y);
            const double v=m.filtered.x*state.x+m.filtered.y*state.y;
            out.audio.left+=left_[j]*v;out.audio.right+=right_[j]*v;
            out.bowlEnergy+=.5*(state.x*state.x+state.y*state.y);
            valid=valid && std::isfinite(state.x) && std::isfinite(state.y);
        }
        valid=valid && std::isfinite(e.compression_) && std::isfinite(e.strikerVelocity_)
            && std::isfinite(out.audio.left) && std::isfinite(out.audio.right);
        if(!valid) { reset();out={};out.fault=true;return out; }
        host_.transitionGain_=std::min(1.0,host_.transitionGain_+host_.transitionIncrement_);
        out.audio.left=host_.transitionFrom_.left*(1-host_.transitionGain_)+out.audio.left*host_.transitionGain_;
        out.audio.right=host_.transitionFrom_.right*(1-host_.transitionGain_)+out.audio.right*host_.transitionGain_;
        host_.lastOutput_=out.audio;return out;
    }
    const VesselEngine& engine() const {return host_.engine();}
    VesselEngine& probeEngine() {return host_.engine_;}
    unsigned factor() const {return host_.factor();}
    bool running() const {return running_;}
    void probeBeginHandoff() {beginHandoff();}
    bool handingOff() const {return handoffRemaining_!=0;}
    std::uint64_t handoffFrames() const {return handoffFrames_;}
    std::uint64_t composedFrames() const {return composed_;}
    std::uint64_t restorations() const {return restores_;}
};
} // namespace vessel

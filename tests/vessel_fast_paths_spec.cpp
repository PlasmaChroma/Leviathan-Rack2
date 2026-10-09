#include "vessel/HostRateAdapter.hpp"
#include "vessel/SeedProfiles.hpp"
#include "../tools/vessel/experiments/FrictionApproximation.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
using namespace vessel;
void require(bool okay,const char* message) {if(!okay)throw std::runtime_error(message);}
void curves() {
    double forceError=0,derivativeError=0;
    for(auto evaluate:{frictionValueApproximate,frictionValueTanhApproximate})
    for(const auto& m:seedMallets)for(double load:{.1,2.5,15.})for(bool narrow:{false,true})
    for(int i=0;i<=100000;++i) {
        const double slip=(narrow?m.regularizationVelocity*20:m.weakeningVelocity*8)*(2.*i/100000-1);
        const auto a=frictionValue(slip,load,m),b=evaluate(slip,load,m);
        forceError=std::max(forceError,std::abs(a.force-b.force));
        derivativeError=std::max(derivativeError,std::abs(a.derivative-b.derivative));
        require(std::abs(a.force-b.force)<1e-5*std::max(1.,load*m.muS),"force approximation error");
        require(std::abs(a.derivative-b.derivative)<5e-6*std::max(1.,load*m.muS/m.regularizationVelocity),"derivative error");
        require(b.force*slip>=0 && std::abs(b.force)<=load*m.muS+1e-12,"friction bound/passivity");
        require(b.derivative>=-frictionApproximateNegativeSlopeBound(load,m)-1e-10,"approximation certificate");
        if(i%1000==501) {
            const double h=m.regularizationVelocity*1e-4;
            const double finite=(evaluate(slip+h,load,m).force-evaluate(slip-h,load,m).force)/(2*h);
            require(std::abs(finite-b.derivative)<1e-5*std::max(1.,std::abs(b.derivative)),"derivative inconsistent with force curve");
        }
    }
    for(auto evaluate:{frictionValueApproximate,frictionValueTanhApproximate})
    for(const auto& m:seedMallets)for(double edge:{8*m.weakeningVelocity,20*m.regularizationVelocity})
    for(double slip:{std::nextafter(edge,0.),edge,std::nextafter(edge,std::numeric_limits<double>::infinity()),-std::nextafter(edge,0.)}) {
        const auto f=evaluate(slip,15,m);
        require(std::isfinite(f.force)&&std::isfinite(f.derivative),"table boundary");
    }
    std::cout<<"[PASS] Dense force/derivative, monotonic slope certificate, dissipation and rounded lookup boundaries; max errors "<<forceError<<" N / "<<derivativeError<<" N/(m/s)\n";
}
void observedCommit() {
    std::mt19937 random(713);
    std::uniform_real_distribution<double> value(-.1, .1);
    for (const auto& bowl : seedBowls) for (double rate : {48000., 96000., 192000.}) {
        ModalBank reference;
        require(reference.configure(bowl, 220., 1., 1., rate), "modal fixture configuration");
        reference.setAdditionalDamping(.125);
        for (std::size_t j=0; j<reference.size(); ++j)
            require(reference.setState(j,value(random),value(random)), "modal fixture state");
        ModalBank candidate = reference;
        const auto left = reference.observer(.2), right = reference.observer(.7);
        for (int step=0; step<2000; ++step) {
            const auto free = reference.freeMidpoint();
            ModalVector force {};
            for (std::size_t j=0; j<reference.size(); ++j) force[j] = value(random);
            reference.commit(free,force);
            double l=0, r=0;
            require(candidate.commitObserved(free,force,left,right,l,r) == reference.finite(),
                "fused commit validity mismatch");
            require(l == reference.velocity(left) && r == reference.velocity(right),
                "fused commit changes pickup arithmetic");
            for (std::size_t j=0; j<reference.size(); ++j)
                require(candidate.state(j).x == reference.state(j).x
                    && candidate.state(j).y == reference.state(j).y,
                    "fused commit changes modal state arithmetic");
        }
        for (double invalid : {std::numeric_limits<double>::infinity(),
                -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()}) {
            for (std::size_t j=0; j<reference.size(); ++j) {
                candidate = reference;
                ModalVector force {}; force[j] = invalid;
                double l=0,r=0;
                require(!candidate.commitObserved(reference.freeMidpoint(),force,left,right,l,r),
                    "fused commit misses nonfinite state");
            }
        }
    }
    std::cout << "[PASS] Fused active commit: exact states/pickups and nonfinite detection in every mode\n";
}
int main(){try{curves();observedCommit();std::cout<<"Vessel fast paths: 2 groups PASS\n";}catch(const std::exception& e){std::cerr<<"[FAIL] "<<e.what()<<'\n';return 1;}}

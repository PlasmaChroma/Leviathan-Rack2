#include "MatchedPoleCandidate.hpp"
#include "StrikeGradientCandidate.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <random>

static double oldGradient(double d0,double d1,double k) {
    auto potential=[&](double d){return d>0?.4*k*d*d*std::sqrt(d):0.;};
    if(d0<=0 && d1<=0) return 0.;
    if(d0<0 || d1<0) return (potential(d1)-potential(d0))/(d1-d0);
    double a=std::sqrt(d0),b=std::sqrt(d1);
    if(a+b==0.) return 0.;
    return .4*k*(d1*d1+d1*b*a+d1*d0+b*a*d0+d0*d0)/(a+b);
}
int main() {
    constexpr double pi=3.14159265358979323846, ln1000=6.9077552789821370521;
    double worstRelative=0.,worstInverseD=0.;std::size_t configs=0;
    for(double rate:{48000.,96000.,192000.,384000.,4000000.})
    for(double t60:{.1,1.,25.,120.})
    for(int i=0;i<1001;++i) {
        double f=20.*std::pow(.3999*rate/20.,i/1000.);
        vessel_audit::MatchedPoleCoefficients candidate;
        if(!vessel_audit::matchedPoleCandidate(f,t60,rate,candidate)) throw std::runtime_error("configuration rejected");
        const double h=1/rate,sigma0=ln1000/t60,r=std::exp(-sigma0*h),theta=2*pi*f*h;
        const double den=1+2*r*std::cos(theta)+r*r;
        const double sigma=(2/h)*(-std::expm1(-2*sigma0*h))/den;
        const double nu=(4/h)*r*std::sin(theta)/den;
        const double omega=std::hypot(sigma,nu),a=.5*h*omega,inv=1/(1+h*sigma+a*a);
        worstRelative=std::max(worstRelative,std::max(std::abs(candidate.omega-omega)/omega,std::abs(candidate.sigma-sigma)/sigma));
        worstInverseD=std::max(worstInverseD,std::abs(candidate.inverseD-inv));
        ++configs;
    }
    std::mt19937_64 rng(72163);std::uniform_real_distribution<double> e(-18.,-1.);
    std::size_t mismatch=0;double maxForceRelative=0.;
    for(int i=0;i<100000;++i) {
        double d0=std::pow(10.,e(rng)),d1=std::pow(10.,e(rng));if(i%3==0)d1=-d1;
        vessel_audit::StrikeGradientCache cache;
        if(!cache.prepare(d0,2e8)) throw std::runtime_error("gradient preparation failed");
        double reference=oldGradient(d0,d1,2e8),value=cache.evaluate(d1).force;
        if(value!=reference)++mismatch;
        maxForceRelative=std::max(maxForceRelative,std::abs(value-reference)/std::max(1e-300,std::abs(reference)));
    }
    std::cout<<std::setprecision(17)<<"{\"modal_configurations\":"<<configs<<",\"max_relative_omega_sigma_difference\":"<<worstRelative
      <<",\"max_absolute_inverseD_difference\":"<<worstInverseD<<",\"strike_cases\":100000,\"strike_force_bit_mismatches\":"<<mismatch
      <<",\"strike_max_relative_force_difference\":"<<maxForceRelative<<"}\n";
    return worstRelative<1e-13 && worstInverseD<1e-14 && maxForceRelative<1e-13?0:1;
}

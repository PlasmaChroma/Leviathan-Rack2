#include "vessel/ModalBank.hpp"
#include "vessel/SeedProfiles.hpp"
#include "vessel/FrictionContact.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace vessel;
void require(bool okay, const char* message) { if (!okay) throw std::runtime_error(message); }
void same(const ModalBank& a, const ModalBank& b) {
    require(a.size() == b.size() && a.timeStep() == b.timeStep(), "bank shape changed");
    for (std::size_t j=0; j<a.size(); ++j) {
        const auto& x=a.coefficients(j); const auto& y=b.coefficients(j);
        require(x.frequency==y.frequency && x.t60==y.t60 && x.mass==y.mass
            && x.inverseRootMass==y.inverseRootMass && x.omega==y.omega && x.sigma==y.sigma
            && x.a==y.a && x.inverseD==y.inverseD && x.admittanceWeight==y.admittanceWeight
            && x.staticComplianceRatio==y.staticComplianceRatio, "cached/fresh coefficients differ");
        require(a.state(j).x==b.state(j).x && a.state(j).y==b.state(j).y, "state changed");
    }
    require(a.freeMidpoint()==b.freeMidpoint(), "hot coefficients differ");
}
void orbitGeometry() {
    ContactOrbit cached;
    for (unsigned i=0; i<1000; ++i) {
        auto bowl=seedBowls[(i/20)%2];
        if (i%7==0) --bowl.pairCount;
        for (std::size_t n=0; n<bowl.pairCount; ++n) {
            bowl.pairs[n].order += (i/30)%2;
            bowl.pairs[n].massA *= 1.+.01*(i%3);
            bowl.pairs[n].orientation += .01*(i%5);
        }
        const double width=(i/10)%3*.04, angle=i*.003;
        ModalBank bank;
        require(bank.configure(bowl,200.+i%100,1.,1.,192000.), "orbit bank setup");
        for (std::size_t n=0; n<bowl.pairCount; ++n) {
            const double half=.5*bowl.pairs[n].order*width;
            const double patch=std::abs(half)<1e-8 ? 1.-half*half/6. : std::sin(half)/half;
            require(cached.patchFactor(n,bowl.pairs[n].order,width)==patch, "footprint key mismatch");
        }
        ContactOrbit fresh;
        cached.configure(bowl,bank,width,angle); fresh.configure(bowl,bank,width,angle);
        for (unsigned tick=0; tick<9; ++tick) {
            ModalVector a,b,c,d;
            const double increment=tick%2 ? .00003 : -.005;
            cached.midpoint(increment,a,b,true); fresh.midpoint(increment,c,d,true);
            require(a==c && b==d, "cached footprint changes resynchronized orbit");
            cached.finish(); fresh.finish();
        }
    }
    std::cout << "[PASS] 1000 exact footprint/orbit comparisons across width, order, mass, angle and topology changes\n";
}
int main() { try {
    orbitGeometry();
    unsigned accepted=0, rejected=0;
    for (const auto& seed : seedBowls) {
        ModalBank cached;
        for (unsigned i=0; i<6000; ++i) {
            auto bowl=seed;
            const unsigned phase=i/30;
            const std::size_t n=phase%bowl.pairCount;
            auto& p=bowl.pairs[n];
            switch (phase%10) {
                case 0: p.massA*=1.2; break;
                case 1: p.massB*=.8; break;
                case 2: p.t60A*=1.3; break;
                case 3: p.t60B*=.7; break;
                case 4: p.splitCents*=1.5; break;
                case 5: p.ratio*=1.01; break;
                case 6: p.orientation+=.03; break;
                case 7: p.radiationA*=.8; break;
                case 8: p.radiationB*=1.1; break;
                case 9: bowl.rimRadius*=1.1; break;
            }
            const double rates[]={32000.,44100.,96000.,192000.,384000.};
            double rate=rates[(i/90)%5];
            if (i%37==0) rate=std::nextafter(rate, std::numeric_limits<double>::infinity());
            const double frequency=80.+(i%71)*8.;
            const double decay=(i/60)%2 ? .25 : 4.;
            const double imperfection=((i/45)%5)*.5;
            if (i%101==0) bowl.pairs[bowl.pairCount-1].massB=-1;
            if (i%103==0) bowl.pairs[bowl.pairCount-1].t60A=std::numeric_limits<double>::quiet_NaN();
            const auto before=cached;
            ModalBank fresh;
            const bool a=cached.configure(bowl,frequency,decay,imperfection,rate);
            const bool b=fresh.configure(bowl,frequency,decay,imperfection,rate);
            require(a==b, "cached/fresh acceptance differs");
            if (!a) { ++rejected; same(cached,before); continue; }
            ++accepted;
            for (std::size_t j=0; j<cached.size(); ++j) {
                require(cached.state(j).x==before.state(j).x && cached.state(j).y==before.state(j).y,
                    "configure altered persistent state");
                cached.setState(j,.001*(j+1),-.002*(j+1));
                fresh.setState(j,.001*(j+1),-.002*(j+1));
            }
            const double damping=i%3 ? 0. : .2;
            cached.setAdditionalDamping(damping); fresh.setAdditionalDamping(damping);
            same(cached,fresh);
            const auto left=cached.observer(.2), right=cached.observer(.7);
            require(left==fresh.observer(.2) && right==fresh.observer(.7), "observer changed");
            double l1,r1,l2,r2;
            require(cached.advanceFree(left,right,l1,r1)==fresh.advanceFree(left,right,l2,r2)
                && l1==l2 && r1==r2, "free trajectory differs");
            same(cached,fresh);
        }
    }
    require(accepted>0 && rejected>0, "matrix coverage");
    std::cout << "[PASS] Setup cache: " << accepted << " exact fresh-bank comparisons; "
        << rejected << " transactional rejections\n";
} catch (const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; } }

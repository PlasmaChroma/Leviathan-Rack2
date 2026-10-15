#include "../src/vessel/ContactWorkProfile.hpp"
#include "../src/vessel/ContactSolver.hpp"
#include "../src/vessel/SeedProfiles.hpp"
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#ifndef VESSEL_PROFILE_CONTACT_WORK
#error This test requires offline work instrumentation.
#endif
using namespace vessel;
void require(bool okay) { if (!okay) throw std::runtime_error("contact work accounting"); }
int main() {
    std::mt19937 random(98431);
    std::uniform_real_distribution<double> unit(0.,1.);
    std::uint64_t fallback=0, lookups=0;
    for (auto& m : seedMallets) for (int i=0; i<2000; ++i) {
        contactWork()={};
        const double load=i==0 ? 0. : 15.*unit(random);
        const double bound=frictionNegativeSlopeBound(load,m);
        const double Y=bound>0 ? .89*unit(random)/bound : .001;
        const auto r=solveFriction((2*unit(random)-1)*.2,Y,load,m,(2*unit(random)-1)*load*m.muS);
        const auto w=contactWork().rub;
        require(w.solves==1 && w.lawEvaluations==r.iterations);
        require(w.converged==unsigned(r.converged) && w.zeroLoad==unsigned(load==0));
        require(w.newton+w.fallback==r.iterations-(r.converged && load>0 ? 1 : 0));
        require(w.expEvaluations<=w.lawEvaluations && w.tanhEvaluations<=w.lawEvaluations);
        require(w.tanhEvaluations==0);
        lookups+=w.tanhLookups;
        fallback+=w.fallback;
    }
    require(fallback>0 && lookups>0);
    contactWork()={};
    const auto& m=seedMallets[1];
    require(!solveFriction(0,-1,1,m).converged);
    require(contactWork().rub.solves==1 && contactWork().rub.lawEvaluations==0);
    contactWork()={};
    solveContacts(-1,0,0,0,0,0,0,1./192000,m,m,0,1,0);
    require(contactWork().coupledSolves==1 && contactWork().coupledDepth==0 && contactWork().outerTrials==0);
    contactWork()={};
    const auto r=solveContacts(1e-5,.2,0,0,1e-5,0,1e-5,1./192000,m,m,.1,2.5,0);
    const auto w=contactWork();
    require(r.converged && !w.coupledDepth && w.coupledSolves==1);
    require(w.outerTrials==w.coupled.solves && w.rub.solves==0);
    require(w.coupled.lawEvaluations>=r.innerIterations);
    std::cout << "[PASS] 8000 friction solves, zero-load/rejection, Newton/fallback accounting and coupled scope; fallbacks=" << fallback << '\n';
}

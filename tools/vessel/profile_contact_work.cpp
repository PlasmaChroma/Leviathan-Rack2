// Offline operation counts, never a CPU benchmark. Same stimuli in plain and
// instrumented executables; exact trajectory fingerprints must match.
#include "../../src/vessel/DualBowlAdapter.hpp"
#include "../../src/vessel/SeedProfiles.hpp"
#include "../../src/vessel/ContactWorkProfile.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <vector>
using namespace vessel;
static void hash(std::uint64_t& h, double v) {
    std::uint64_t bits; std::memcpy(&bits,&v,sizeof(bits));
    h=(h^bits)*UINT64_C(1099511628211);
}
static ContactWork counts() {
#if defined(VESSEL_PROFILE_CONTACT_WORK)
    return contactWork();
#else
    return {};
#endif
}
static void fields(const FrictionWork& w) {
    std::cout << ',' << w.solves << ',' << w.converged << ',' << w.zeroLoad << ',' << w.lawEvaluations
        << ',' << w.expEvaluations << ',' << w.tanhEvaluations << ',' << w.newton << ',' << w.fallback
        << ',' << w.gaussianCore << ',' << w.gaussianTail << ',' << w.tanhLookups;
    if (w.solves != w.converged || w.lawEvaluations != w.newton+w.fallback+w.converged-w.zeroLoad)
        throw std::runtime_error("friction accounting/failure");
}
int main() {
    try {
        std::cout << std::setprecision(17);
        std::cout << "bowl,mallet,quality,separation,speed,phase,frames,internal_rate,fingerprint,max_energy_residual";
        for (auto name : {"rub","coupled"}) for (auto field : {"solves","converged","zero_load","laws","exp","tanh","newton","fallback","gaussian_core","gaussian_tail","tanh_lookups"})
            std::cout << ',' << name << '_' << field;
        std::cout << ",outer_solves,outer_trials,bracket_expansions,outer_newton,outer_fallback,laws_per_callback_p50,laws_per_callback_p99,laws_per_callback_max\n";
        for (int bowl : {0,1}) for (int mallet : {0,1,2,3}) for (int quality : {1,2}) for (double separation : {0.,33.}) for (double speed : {.002,.2,1.5}) {
            DualBowlAdapter adapter; EngineSettings settings;
            adapter.setAuditEnabled(true);
            if (!adapter.configure(seedBowls[bowl],seedMallets[mallet],settings,separation,48000.,ProcessingQuality(quality)))
                throw std::runtime_error("setup");
            const char* names[]={"startup","sustained","overlap","release","restart"};
            const int lengths[]={24000,48000,12000,24000,24000};
            for (int phase=0; phase<5; ++phase) {
                VESSEL_WORK(contactWork() = ContactWork{});
                std::uint64_t fingerprint=UINT64_C(14695981039346656037), previous=0;
                std::vector<std::uint64_t> work; work.reserve(lengths[phase]);
                for (int i=0; i<lengths[phase]; ++i) {
                    HostControls c; c.rotate=phase!=3; c.speed=speed; c.pressure=2.5;
                    c.strikeEvent=phase==2 && i%3000==0;
                    c.velocity=i<6000 ? .1 : 1.;
                    const auto frame=adapter.process(c);
                    if (frame.fault) throw std::runtime_error("frame fault");
                    hash(fingerprint,frame.audio.left); hash(fingerprint,frame.audio.right);
                    for (auto* engine : {&adapter.engine(),&adapter.rightEngine()}) {
                        hash(fingerprint,engine->totalEnergy());
                        hash(fingerprint,engine->compression()); hash(fingerprint,engine->strikerVelocity());
                        for (std::size_t j=0; j<engine->bowl().size(); ++j) {
                            hash(fingerprint,engine->bowl().state(j).x); hash(fingerprint,engine->bowl().state(j).y);
                        }
                    }
                    const auto w=counts(); const auto total=w.rub.lawEvaluations+w.coupled.lawEvaluations;
                    work.push_back(total-previous); previous=total;
                }
                const auto w=counts();
                if (w.coupledDepth || w.outerTrials!=w.coupled.solves) throw std::runtime_error("coupled accounting");
                const auto& a=adapter.engine().ledger(); const auto& b=adapter.rightEngine().ledger();
                if (a.solverFaults || b.solverFaults || a.nonfiniteResets || b.nonfiniteResets) throw std::runtime_error("recovery");
                std::sort(work.begin(),work.end());
                std::cout << bowl << ',' << mallet << ',' << quality << ',' << separation << ',' << speed << ',' << names[phase] << ','
                    << lengths[phase] << ',' << adapter.internalRate() << ',' << fingerprint << ',' << std::max(a.maxStepResidual,b.maxStepResidual);
                fields(w.rub); fields(w.coupled);
                std::cout << ',' << w.coupledSolves << ',' << w.outerTrials << ',' << w.bracketExpansions << ',' << w.outerNewton
                    << ',' << w.outerFallback << ',' << work[work.size()/2] << ',' << work[std::size_t(.99*(work.size()-1))] << ',' << work.back() << std::endl;
            }
        }
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}

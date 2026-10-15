#include "../src/vessel/DualBowlAdapter.hpp"
#include "../src/vessel/SeedProfiles.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

namespace {
using namespace vessel;
void require(bool okay, const char* message) { if (!okay) throw std::runtime_error(message); }
void sameEngine(const VesselEngine& a, const VesselEngine& b) {
    require(a.totalEnergy() == b.totalEnergy() && a.compression() == b.compression()
        && a.strikerVelocity() == b.strikerVelocity() && a.strikerActive() == b.strikerActive()
        && a.maxFrictionUniqueness() == b.maxFrictionUniqueness(), "observer update changes contact state/certificate");
    for (std::size_t j=0; j<a.bowl().size(); ++j) {
        const auto& x = a.bowl().coefficients(j); const auto& y = b.bowl().coefficients(j);
        require(x.omega == y.omega && x.sigma == y.sigma && x.mass == y.mass
            && x.inverseD == y.inverseD && x.admittanceWeight == y.admittanceWeight
            && a.bowl().state(j).x == b.bowl().state(j).x
            && a.bowl().state(j).y == b.bowl().state(j).y, "observer cache changes modal coefficients/state");
    }
}
void trajectories() {
    unsigned cases = 0, tailUpdates = 0, contactUpdates = 0;
    for (const auto& bowl : seedBowls) for (const auto& mallet : seedMallets)
    for (double rate : {44100.,48000.,96000.})
    for (auto quality : {ProcessingQuality::Economy,ProcessingQuality::Balanced,ProcessingQuality::Reference})
    for (double separation : {0.,33.}) {
        DualBowlAdapter fast, full;
        full.setObserverFastPathEnabled(false);
        EngineSettings settings;
        require(fast.configure(bowl,mallet,settings,separation,rate,quality)
            && full.configure(bowl,mallet,settings,separation,rate,quality), "observer fixture setup");
        HostControls controls; controls.velocity=.4;
        for (unsigned i=0; i<4096; ++i) {
            controls.rotate = cases%2 != 0;
            controls.strikeEvent = i == 0 || i == 2048;
            if (i == 1 || i == 256 || i == 1024 || i == 1536 || i == 3072) {
                tailUpdates += fast.composedTailActive(); contactUpdates += fast.engine().strikerActive();
                settings.observerCenter = i%3*.23;
                settings.observerSeparation = i%2 ? 0 : pi*.2;
                if (i == 1536) settings.frequency *= 1.1; // Must take full mechanical path.
                require(fast.configure(bowl,mallet,settings,separation,rate,quality)
                    && full.configure(bowl,mallet,settings,separation,rate,quality), "observer reconfiguration");
            }
            const auto a = fast.process(controls), b = full.process(controls);
            require(!a.fault && !b.fault && a.audio.left == b.audio.left && a.audio.right == b.audio.right
                && a.bowlEnergy == b.bowlEnergy, "observer cached/full audio mismatch");
            if (i%17 == 0) { sameEngine(fast.engine(),full.engine()); sameEngine(fast.rightEngine(),full.rightEngine()); }
        }
        ++cases;
    }
    require(tailUpdates && contactUpdates, "observer matrix missed tail/active-contact updates");
    std::cout << "[PASS] " << cases << " exact observer/full trajectories; composed-tail updates="
        << tailUpdates << ", active-contact updates=" << contactUpdates << '\n';
}
void cacheKeys() {
    VesselEngine seed;
    EngineSettings settings;
    seed.setRotation(true,.2,2.5); seed.strike(.5);
    for (unsigned i=0; i<300; ++i) require(!seed.step().fault, "observer cache seed");
    unsigned cases = 0;
    auto check = [&](const BowlDescriptor& bowl, const MalletDescriptor& mallet, const EngineSettings& s, double rate) {
        auto fast=seed, full=seed;
        full.setObserverFastPathEnabled(false);
        const bool a=fast.configure(bowl,mallet,s,rate), b=full.configure(bowl,mallet,s,rate);
        require(a == b, "observer cache bypasses configuration validation");
        sameEngine(fast,full);
        for (unsigned i=0; i<64; ++i) {
            const auto x=fast.step(), y=full.step();
            require(x.fault == y.fault && x.leftVelocity == y.leftVelocity && x.rightVelocity == y.rightVelocity,
                    "observer cache missed changed physical descriptor");
        }
        sameEngine(fast,full); ++cases;
    };
    double ModePairDescriptor::* pairFields[] = {&ModePairDescriptor::ratio,&ModePairDescriptor::splitCents,
        &ModePairDescriptor::orientation,&ModePairDescriptor::massA,&ModePairDescriptor::massB,
        &ModePairDescriptor::t60A,&ModePairDescriptor::t60B,&ModePairDescriptor::radiationA,&ModePairDescriptor::radiationB};
    for (auto field : pairFields) {
        auto bowl=seedBowls[0]; bowl.pairs[2].*field = (bowl.pairs[2].*field)*1.01+.0001;
        check(bowl,seedMallets[1],settings,192000);
    }
    double MalletDescriptor::* malletFields[] = {&MalletDescriptor::mass,&MalletDescriptor::stiffness,
        &MalletDescriptor::exponent,&MalletDescriptor::loadingDamping,&MalletDescriptor::patchWidth,
        &MalletDescriptor::muS,&MalletDescriptor::muK,&MalletDescriptor::weakeningVelocity,&MalletDescriptor::regularizationVelocity};
    for (auto field : malletFields) {
        auto mallet=seedMallets[1]; mallet.*field *= 1.01;
        check(seedBowls[0],mallet,settings,192000);
    }
    auto bowl=seedBowls[0]; bowl.rimRadius *= 1.1; check(bowl,seedMallets[1],settings,192000);
    bowl=seedBowls[0]; ++bowl.pairs[2].order; check(bowl,seedMallets[1],settings,192000);
    bowl=seedBowls[0]; ++bowl.pairCount; check(bowl,seedMallets[1],settings,192000);
    for (double EngineSettings::* field : {&EngineSettings::frequency,&EngineSettings::decayMultiplier,
        &EngineSettings::imperfection,&EngineSettings::strikeAngle,&EngineSettings::observerCenter,&EngineSettings::observerSeparation}) {
        auto changed=settings; changed.*field *= 1.1; check(seedBowls[0],seedMallets[1],changed,192000);
    }
    auto changed=settings; changed.prescribedRadialLoad=true; check(seedBowls[0],seedMallets[1],changed,192000);
    check(seedBowls[0],seedMallets[1],settings,384000);
    check(seedBowls[0],seedMallets[1],settings,std::nextafter(192000.,0.));
    check(seedBowls[0],seedMallets[1],settings,std::nextafter(192000.,4000000.));
    const double nan=std::numeric_limits<double>::quiet_NaN();
    changed=settings; changed.observerSeparation=nan; check(seedBowls[0],seedMallets[1],changed,192000);
    changed=settings; changed.imperfection=nan; check(seedBowls[0],seedMallets[1],changed,192000);
    bowl=seedBowls[0]; bowl.pairs[2].massA=nan; check(bowl,seedMallets[1],settings,192000);
    check(seedBowls[0],seedMallets[1],settings,nan);
    std::cout << "[PASS] " << cases << " physical cache-key, rate and transactional rejection checks\n";
}
void modalBandBoundary() {
    EngineSettings settings; settings.frequency=1500;
    VesselEngine seed;
    require(seed.configure(seedBowls[1],seedMallets[1],settings,192000), "modal edge setup");
    double highest=0;
    for (std::size_t j=0; j<seed.bowl().size(); ++j)
        highest=std::max(highest,seed.bowl().coefficients(j).frequency);
    const double boundary=highest/.4;
    double acceptedRate=boundary;
    for (unsigned i=0; i<8; ++i) acceptedRate=std::nextafter(acceptedRate,4000000.);
    require(seed.configure(seedBowls[1],seedMallets[1],settings,acceptedRate), "modal edge valid rate");
    double rate=boundary;
    for (unsigned i=0; i<8; ++i) rate=std::nextafter(rate,0.);
    unsigned accepts=0,rejects=0;
    for (unsigned i=0; i<17; ++i) {
        auto fast=seed, full=seed; full.setObserverFastPathEnabled(false);
        const bool a=fast.configure(seedBowls[1],seedMallets[1],settings,rate);
        const bool b=full.configure(seedBowls[1],seedMallets[1],settings,rate);
        require(a == b, "cached observer configuration bypasses strict modal band limit");
        sameEngine(fast,full); accepts+=a; rejects+=!a;
        rate=std::nextafter(rate,4000000.);
    }
    require(accepts && rejects, "modal boundary cases must straddle acceptance limit");
    std::cout << "[PASS] Adjacent sample rates preserve the strict modal band boundary\n";
}
}
int main() {
    try { trajectories(); cacheKeys(); modalBandBoundary(); }
    catch (const std::exception& e) { std::cerr << "[FAIL] " << e.what() << '\n'; return 1; }
}

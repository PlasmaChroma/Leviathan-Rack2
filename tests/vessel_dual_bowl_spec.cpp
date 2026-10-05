#include "../src/vessel/DualBowlAdapter.hpp"
#include "../src/vessel/SeedProfiles.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
namespace {
using namespace vessel;
void require(bool okay, const char* message) { if (!okay) throw std::runtime_error(message); }
void referenceEquivalence() {
    for (double rate : {32000., 44100., 48000., 88200., 96000., 176400., 192000.})
    for (double delta : {0., 1., 10., 33.})
    for (auto quality : {ProcessingQuality::Economy, ProcessingQuality::Balanced, ProcessingQuality::Reference}) {
        EngineSettings s; s.observerSeparation = .3;
        DualBowlAdapter dual(false); HostRateAdapter left, right;
        require(dual.configure(seedBowls[0], seedMallets[1], s, delta, rate, quality), "dual setup");
        auto ls=s, rs=s; ls.frequency-=delta/2; rs.frequency+=delta/2;
        require(left.configure(seedBowls[0], seedMallets[1], ls, rate, quality)
            && right.configure(seedBowls[0], seedMallets[1], rs, rate, quality), "independent reference setup");
        HostControls c; c.rotate=true;
        for (int i=0; i<1000; ++i) {
            c.strikeEvent=i==0 || i==500;
            const auto a=dual.process(c);
            const auto l=left.process(c), r=right.process(c);
            require(!a.fault && a.audio.left==l.audio.left && a.audio.right==r.audio.right
                && a.leftEnergy==l.bowlEnergy && a.rightEnergy==r.bowlEnergy
                && a.bowlEnergy==.5*(l.bowlEnergy+r.bowlEnergy), "dual differs from independent reference");
            if (delta==0) require(a.leftEnergy==a.rightEnergy, "zero separation mechanics diverge");
        }
        require(dual.engine().ledger().strikes==2 && dual.rightEngine().ledger().strikes==2, "shared events not delivered once per bowl");
    }
    std::cout<<"[PASS] Seven rates, three qualities and four separations match two independent references bit-for-bit; shared events and zero-difference energy\n";
}
void transactionalTuning() {
    EngineSettings s; DualBowlAdapter d;
    for (double center : {20., 36.5, 261.625565, 1983.5, 2000.}) {
        s.frequency=center;
        require(d.configure(seedBowls[0], seedMallets[1], s, 33, 48000), "boundary setup");
        require(d.rightEngine().settings().frequency-d.engine().settings().frequency==33
            && d.engine().settings().frequency>=20 && d.rightEngine().settings().frequency<=2000, "boundary loses separation or exceeds range");
    }
    s.frequency=261.625565; require(d.configure(seedBowls[0], seedMallets[1], s, 10, 48000), "live setup");
    HostControls c; c.rotate=true; c.strikeEvent=true; d.process(c); c.strikeEvent=false;
    for (int i=0;i<10000;++i) d.process(c);
    const double le=d.engine().totalEnergy(), re=d.rightEngine().totalEnergy();
    require(d.configure(seedBowls[1], seedMallets[2], s, 0, 44100), "live retune to zero");
    require(d.engine().totalEnergy()==le && d.rightEngine().totalEnergy()==re, "retune clones or clears state");
    require(d.engine().bowl().state(0).x!=d.rightEngine().bowl().state(0).x, "zero retune forcibly resynchronizes bowls");
    // Only the higher-pitched candidate exceeds the structural bandwidth limit.
    auto bad=seedBowls[0]; bad.pairs[0].ratio=192000*.4/261.625565; bad.pairs[0].splitCents=0;
    HostRateAdapter validLeft; auto low=s; low.frequency-=16.5;
    require(validLeft.configure(bad, seedMallets[1], low, 48000), "rejection fixture must admit left candidate");
    require(!d.configure(bad, seedMallets[1], s, 33, 48000), "invalid right bowl accepted");
    require(d.engine().totalEnergy()==le && d.rightEngine().totalEnergy()==re && d.hostRate()==44100
        && d.separationHz()==0, "failed pair update partially applied");
    // A rejected right candidate must preserve the entire future stream,
    // including FIR/rate-transition state. Exercise active and sleeping right
    // paths: the latter must not clone/wake before both candidates validate.
    for (bool single : {false, true}) {
        DualBowlAdapter subject = d;
        if (single) {
            subject.reset();
            require(subject.configure(seedBowls[0], seedMallets[1], s, 0, 44100), "single rollback setup");
            c.strikeEvent = true; subject.process(c); c.strikeEvent = false;
            for (int i = 0; i < 256; ++i) subject.process(c);
            require(!subject.secondBowlActive(), "rollback must exercise inactive right");
        }
        auto unchanged = subject;
        require(!subject.configure(bad, seedMallets[1], s, 33, 48000), "right rejection accepted");
        require(subject.secondBowlActive() == unchanged.secondBowlActive()
            && subject.dualMix() == unchanged.dualMix(), "failed update changes activation/fade");
        for (int i = 0; i < 2048; ++i) {
            const auto actual = subject.process(c), expected = unchanged.process(c);
            require(actual.audio.left == expected.audio.left && actual.audio.right == expected.audio.right
                && actual.leftEnergy == expected.leftEnergy && actual.rightEnergy == expected.rightEnergy
                && actual.fault == expected.fault, "failed configuration changes future audio/state");
        }
    }
    for (double invalid : {-1.,34.,std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::infinity()})
        require(!d.configure(seedBowls[0], seedMallets[1], s, invalid, 48000), "invalid separation accepted");
    d.reset(); require(d.meanEnergy()==0 && !d.engine().strikerActive() && !d.rightEngine().strikerActive(), "dual reset");
    std::cout<<"[PASS] Exact Hz tuning and boundary policy, live state continuity through zero/rate/material changes, transactional rejection and reset\n";
}
double residual(const VesselEngine& e) {
    const auto& l=e.ledger(); return e.totalEnergy()+l.modalLoss+l.contactLoss+l.frictionLoss+l.retiredEnergy+l.recoveryLoss
        -l.launchWork-l.speedCapWork-l.handWork-l.radialWork;
}
void sustainedEnergy() {
    double maximum=0;
    for (double delta : {0.,1.,10.,33.}) {
        DualBowlAdapter d(false); EngineSettings s;
        require(d.configure(seedBowls[0],seedMallets[1],s,delta,48000), "sustained setup"); d.setAuditEnabled(true);
        HostControls c; double last=0;
        for (int i=0;i<15*48000;++i) {
            c.rotate=i<12*48000; c.strikeEvent=i==2400 || i==8*48000;
            const auto f=d.process(c); require(!f.fault && std::isfinite(f.audio.left) && std::isfinite(f.audio.right), "sustained fault");
            maximum=std::max(maximum,std::max(std::abs(residual(d.engine())),std::abs(residual(d.rightEngine()))));
            if (i==12*48000) last=f.bowlEnergy;
            if (i==15*48000-1) require(f.bowlEnergy<last, "lifted pair fails to decay");
        }
        require(d.engine().ledger().solverFaults==0 && d.rightEngine().ledger().solverFaults==0
            && d.engine().ledger().speedCaps==0 && d.rightEngine().ledger().speedCaps==0, "sustained recovery or cap");
    }
    require(maximum<1e-8, "dual cumulative energy residual");
    std::cout<<"[PASS] Four 15-second coupled/lift trajectories; independent energy ledgers, no faults/caps, max residual "<<maximum<<" J\n";
}
void singlePathTransitions() {
    DualBowlAdapter d; HostRateAdapter left, right; EngineSettings s;
    double rate=48000, delta=0; const BowlDescriptor* bowl=&seedBowls[0];
    require(d.configure(*bowl,seedMallets[1],s,0,rate) && left.configure(*bowl,seedMallets[1],s,rate), "single setup");
    require(!d.secondBowlActive(), "startup zero runs second bowl");
    VesselEngine rawLeft, rawRight;
    StereoDecimator filter;
    unsigned factor = HostRateAdapter::factorForRate(rate);
    require(rawLeft.configure(*bowl,seedMallets[1],s,rate*factor), "raw setup");
    filter.configure(factor);
    double filterRate = rate;
    double gain = 1.0, gainIncrement = 1.0, previousExpected = 0.0, held = 0.0;
    HostControls c; c.rotate=true;
    bool sawFade=false, sawSingle=false, sawWake=false;
    for(int i=0;i<16000;++i) {
        bool change=false;
        if(i==8 || i==6000 || i==11000) {delta=33;change=true;}
        if(i==512 || i==3000 || i==8000 || i==13000) {delta=0;change=true;}
        if(i==1000) {delta=10;change=true;}
        if(i==8000) {rate=44100;s.observerSeparation=0;}
        if(i==11000) bowl=&seedBowls[1];
        if(i==13000) bowl=&seedBowls[0];
        if(change) {
            const bool wake=delta>0 && !d.secondBowlActive();
            const double energy=d.engine().totalEnergy();
            const double compression=d.engine().compression(), velocity=d.engine().strikerVelocity();
            if(i==8) require(d.engine().strikerActive() && compression>0, "active-impact wake not exercised");
            if(wake) { right=left; rawRight=rawLeft; sawWake=true; }
            auto ls=s,rs=s;ls.frequency-=delta/2;rs.frequency+=delta/2;
            require(d.configure(*bowl,seedMallets[1],s,delta,rate) && left.configure(*bowl,seedMallets[1],ls,rate), "transition configure");
            if(d.secondBowlActive()) require(right.configure(*bowl,seedMallets[1],rs,rate), "right oracle configure");
            const unsigned nextFactor = HostRateAdapter::factorForRate(rate);
            if (filterRate != rate || factor != nextFactor) {
                filter.configure(nextFactor); gain = 0; held = previousExpected;
                gainIncrement = 1.0/(.005*rate);
            }
            factor = nextFactor; filterRate = rate;
            require(rawLeft.configure(*bowl,seedMallets[1],ls,rate*factor), "raw left retune");
            if(d.secondBowlActive()) require(rawRight.configure(*bowl,seedMallets[1],rs,rate*factor), "raw right retune");
            if(wake) require(d.rightEngine().totalEnergy()==energy && d.rightEngine().compression()==compression
                && d.rightEngine().strikerVelocity()==velocity, "wake loses active contact/tail state");
        }
        c.strikeEvent=i==0 || i==6000 || i==13000;
        const bool active=d.secondBowlActive();
        const auto l=left.process(c); if(active) right.process(c);
        rawLeft.setRotation(c.rotate,c.speed,c.pressure);
        if(c.strikeEvent) rawLeft.strike(c.velocity,c.strikeVelocityScale);
        if(active) {
            rawRight.setRotation(c.rotate,c.speed,c.pressure);
            if(c.strikeEvent) rawRight.strike(c.velocity,c.strikeVelocityScale);
        }
        const double startMix = d.dualMix();
        StereoSample filtered;
        for(unsigned k=0;k<factor;++k) {
            const auto a=rawLeft.step();
            const auto b=active?rawRight.step():a;
            const double direction = delta==0 ? -1.0 : 1.0;
            const double weight=std::max(0.0,std::min(1.0,startMix+direction*(k+1)/(.05*rate*factor)));
            StereoSample input; input.left=a.leftVelocity;
            input.right=a.rightVelocity+weight*(b.rightVelocity-a.rightVelocity);
            filter.push(input,filtered);
        }
        gain=std::min(1.0,gain+gainIncrement);
        const double expected=held*(1-gain)+filtered.right*gain;
        previousExpected=expected;
        const auto f=d.process(c);const double mix=d.dualMix();
        require(!f.fault && f.audio.left==l.audio.left && std::abs(f.audio.right-expected)<1e-12,
            "fade differs from independent internal-rate mix/filter oracle");
        require(delta==0 ? mix<=startMix : mix>=startMix, "fade reverses direction");
        require(d.engine().totalEnergy()==left.engine().totalEnergy(), "fold/wake changes surviving mechanics");
        if(d.secondBowlActive()) require(d.rightEngine().totalEnergy()==right.engine().totalEnergy(), "cloned branch differs from independent oracle");
        require(f.bowlEnergy>=std::min(f.leftEnergy,f.rightEnergy)-1e-15
            && f.bowlEnergy<=std::max(f.leftEnergy,f.rightEnergy)+1e-15, "fade energy outside physical bounds");
        sawFade=sawFade || (mix>0 && mix<1);sawSingle=sawSingle || (!d.secondBowlActive() && i>64);
    }
    require(sawFade && sawSingle && sawWake && !d.secondBowlActive(), "transition scenarios not reached");
    std::cout<<"[PASS] Single startup/fold, cloned contact wake, interrupted fades, shared strikes, width/material/rate changes match pre-filter fade oracle\n";
}
void fadeTimingAndSettling() {
    for (double rate : {32000.,44100.,48000.,88200.,96000.,176400.,192000.})
    for (auto quality : {ProcessingQuality::Economy,ProcessingQuality::Balanced,ProcessingQuality::Reference})
    for (bool rubbing : {false,true}) {
        DualBowlAdapter d; HostRateAdapter survivor; EngineSettings s;
        auto low=s; low.frequency-=16.5;
        require(d.configure(seedBowls[0],seedMallets[1],s,33,rate,quality)
            && survivor.configure(seedBowls[0],seedMallets[1],low,rate,quality), "fade timing setup");
        HostControls c; c.rotate=rubbing; c.strikeEvent=true;
        d.process(c); survivor.process(c); c.strikeEvent=false;
        for(int i=0;i<512;++i) { d.process(c); survivor.process(c); }
        require(d.configure(seedBowls[0],seedMallets[1],s,0,rate,quality)
            && survivor.configure(seedBowls[0],seedMallets[1],s,rate,quality), "fold configure");
        const int duration=int(std::lround(.05*rate));
        for(int i=0;i<duration+256;++i) {
            const auto actual=d.process(c); const auto expected=survivor.process(c);
            require(!actual.fault && actual.audio.left==expected.audio.left, "fold alters left output");
            require(d.secondBowlActive()==(i+1<duration), "fold sleep is not exactly 50 ms");
            require(std::abs(d.dualMix()-std::max(0.0,1.0-double(i+1)/duration))<2e-12,
                "fold duration depends on oversampling");
            if(i>=duration+128) require(actual.audio.right==expected.audio.right, "shared FIR fails to settle to single reference");
        }
        require(d.configure(seedBowls[0],seedMallets[1],s,10,rate,quality), "wake configure");
        for(int i=0;i<duration;++i) {
            require(!d.process(c).fault && d.secondBowlActive(), "wake fault or premature sleep");
            require(std::abs(d.dualMix()-double(i+1)/duration)<2e-12, "wake duration depends on oversampling");
        }
        require(d.dualMix()==1, "wake fails to reach exact endpoint");
        require(d.configure(seedBowls[0],seedMallets[1],s,33,rate,quality), "positive retune");
        d.process(c); require(d.dualMix()==1, "positive retune restarts fade");
    }
    std::cout<<"[PASS] 50 ms fold/wake at seven rates and three qualities, tails/rubbing, FIR settling and positive retunes\n";
}

}
int main() {
    try { referenceEquivalence(); transactionalTuning(); sustainedEnergy(); singlePathTransitions(); fadeTimingAndSettling(); std::cout<<"Vessel dual-bowl reference: 5 groups PASS\n"; }
    catch (const std::exception& e) { std::cerr<<"[FAIL] "<<e.what()<<'\n';return 1; }
}

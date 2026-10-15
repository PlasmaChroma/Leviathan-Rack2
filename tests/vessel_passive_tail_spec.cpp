#include "../src/vessel/DualBowlAdapter.hpp"
#include "../src/vessel/SeedProfiles.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>

static bool trap=false;
static unsigned allocations=0;
void* operator new(std::size_t n) { if(trap) ++allocations; if(auto p=std::malloc(n?n:1)) return p; throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
namespace {
using namespace vessel;
void require(bool okay, const char* message) { if(!okay) throw std::runtime_error(message); }
struct Difference {
    double signal=0, error=0, peak=0, state=0, energy=0;
    unsigned long long count=0;
    void engine(const VesselEngine& a,const VesselEngine& b) {
        require(a.bowl().size()==b.bowl().size(),"mode count changed");
        for(std::size_t j=0;j<a.bowl().size();++j) {
            state=std::max(state,std::abs(a.bowl().state(j).x-b.bowl().state(j).x));
            state=std::max(state,std::abs(a.bowl().state(j).y-b.bowl().state(j).y));
        }
        require(a.ledger().strikes==b.ledger().strikes && a.ledger().solverFaults==b.ledger().solverFaults
            && a.ledger().nonfiniteResets==b.ledger().nonfiniteResets,"event/fault ledger changed");
    }
    void add(const DualBowlFrame& a,const DualBowlFrame& b,const DualBowlAdapter& ref,const DualBowlAdapter& candidate) {
        require(!a.fault && !b.fault,"unexpected fault");
        for(auto pair:{std::make_pair(a.audio.left,b.audio.left),std::make_pair(a.audio.right,b.audio.right)}) {
            require(std::isfinite(pair.second),"nonfinite audio");
            const double e=pair.first-pair.second; error+=e*e; signal+=pair.first*pair.first;
            peak=std::max(peak,std::abs(e)); ++count;
        }
        energy=std::max(energy,std::max(std::abs(a.leftEnergy-b.leftEnergy),std::abs(a.rightEnergy-b.rightEnergy)));
        require(ref.secondBowlActive()==candidate.secondBowlActive() && ref.dualMix()==candidate.dualMix(),"fold/wake timing changed");
        engine(ref.engine(),candidate.engine()); engine(ref.rightEngine(),candidate.rightEngine());
    }
    void gate() const {
        require(error<=signal*1e-10 || error<=count*1e-24,"relative/absolute RMS equivalence");
        require(peak<1e-9 && state<1e-8 && energy<1e-8,"peak/state/energy equivalence");
    }
};
void seed(DualBowlAdapter& h,double amplitude=.003) {
    for(const VesselEngine* engine:{&h.engine(),&h.rightEngine()}) {
        auto& bank=const_cast<ModalBank&>(engine->bowl());
        for(std::size_t j=0;j<bank.size();++j)
            require(bank.setState(j,amplitude*std::sin(double(j+1)),amplitude*std::cos(double(2*j+1))),"seed");
    }
}
void compare(DualBowlAdapter& ref,DualBowlAdapter& candidate,const HostControls& c,Difference& d) {
    trap=true;
    const auto a=ref.process(c),b=candidate.process(c);
    trap=false; d.add(a,b,ref,candidate);
}
void matrix() {
    Difference all;
    for(int bowl:{0,1}) for(double rate:{32000.,44100.,48000.,88200.,96000.,176400.,192000.})
    for(auto quality:{ProcessingQuality::Economy,ProcessingQuality::Balanced,ProcessingQuality::Reference})
    for(double delta:{0.,1.,33.}) for(double pitch:{20.,261.625565,880.,2000.}) {
        DualBowlAdapter ref,candidate;ref.setComposedTailEnabled(false);EngineSettings settings;settings.frequency=pitch;
        require(ref.configure(seedBowls[bowl],seedMallets[1],settings,delta,rate,quality)
            &&candidate.configure(seedBowls[bowl],seedMallets[1],settings,delta,rate,quality),"matrix setup");
        seed(ref);seed(candidate);HostControls c;
        double last=candidate.meanEnergy();
        for(int frame=0;frame<4096;++frame) {
            compare(ref,candidate,c,all);
            require(candidate.meanEnergy()<=last+1e-14,"free energy increased"); last=candidate.meanEnergy();
        }
        require(candidate.composedTailActive()==(candidate.internalRate()>rate),"composition not exercised");
        // Resume physical contact at a fully composed state, then strike again
        // while the history correction is still active.
        for(int frame=0;frame<256;++frame) {
            c.strikeEvent=frame==0||frame==1||frame==64||frame==96;
            c.rotate=frame>=4&&frame<192;
            compare(ref,candidate,c,all);
        }
    }
    all.gate();std::cout<<"[PASS] 504 configurations and contact re-entry; relative RMS "<<std::sqrt(all.error/all.signal)
        <<", peak "<<all.peak<<", modal-state "<<all.state<<"\n";
}
void transitions() {
    Difference all;
    for(int bowl:{0,1}) for(double rate:{32000.,48000.})
    for(auto quality:{ProcessingQuality::Balanced,ProcessingQuality::Reference}) {
        DualBowlAdapter ref,candidate;ref.setComposedTailEnabled(false);EngineSettings settings;
        auto descriptor=seedBowls[bowl];double delta=33,currentRate=rate;
        auto configure=[&]() {
            trap=true;const bool a=ref.configure(descriptor,seedMallets[1],settings,delta,currentRate,quality);
            const bool b=candidate.configure(descriptor,seedMallets[1],settings,delta,currentRate,quality);
            trap=false;require(a&&b,"transition configure");
        };
        configure();seed(ref);seed(candidate);HostControls c;
        for(int frame=0;frame<40000;++frame) {
            c.strikeEvent=frame==8002||frame==8010||frame==16000;
            c.rotate=frame>=22000&&frame<25000;
            c.speed=frame<23000?.4:-.4;c.pressure=frame<23000?2.5:15;
            if(frame==4000) {require(candidate.composedTailActive(),"fold did not leave composition");delta=0;configure();}
            if(frame==4010) {delta=10;configure();} // Reverse during history handoff.
            if(frame==4500) {delta=0;configure();} // Reverse during 50 ms fade.
            if(frame==8000) {require(candidate.composedTailActive(),"single tail did not compose");delta=33;configure();}
            if(frame==8001) {settings.observerSeparation=0;configure();}
            if(frame==8003) {descriptor=seedBowls[1-bowl];configure();}
            if(frame==12000) {settings.frequency=880;configure();}
            if(frame==12001) {settings.observerSeparation=pi/2;configure();}
            if(frame==12002) {currentRate=rate==48000?44100:48000;configure();}
            if(frame==15000) {currentRate=rate;configure();}
            if(frame==19000) {
                auto unchanged=candidate;auto bad=settings;bad.frequency=-1;
                require(!candidate.configure(descriptor,seedMallets[1],bad,delta,rate,quality),"invalid accepted");
                for(int k=0;k<128;++k) {
                    const auto a=unchanged.process(c),b=candidate.process(c);
                    require(a.audio.left==b.audio.left&&a.audio.right==b.audio.right,"rejection changed future stream");
                    all.add(ref.process(c),b,ref,candidate);
                }
            }
            if(frame==28000) {candidate.setComposedTailEnabled(false);}
            if(frame==28200) candidate.setComposedTailEnabled(true);
            if(frame==30000) {require(candidate.composedTailActive(),"audit toggle missed composition");ref.setAuditEnabled(true);candidate.setAuditEnabled(true);}
            if(frame==30500) {ref.setAuditEnabled(false);candidate.setAuditEnabled(false);}
            if(frame==32000) {ref.reset();candidate.reset();seed(ref,.15);seed(candidate,.15);}
            if(frame%48==0) {ref.updateHighEnergyDamping();candidate.updateHighEnergyDamping();}
            compare(ref,candidate,c,all);
        }
    }
    all.gate();std::cout<<"[PASS] Fold/wake, interrupted fades, mid-handoff tuning/material/rate, audit/disable, high-energy and reset equivalence\n";
}
void cacheAndFaults() {
    for(bool dual:{false,true}) {
        DualBowlAdapter ref,candidate;ref.setComposedTailEnabled(false);EngineSettings s;HostControls c;Difference d;
        const double delta=dual?33:0;
        auto configure=[&]() {require(ref.configure(seedBowls[0],seedMallets[1],s,delta,48000)
            &&candidate.configure(seedBowls[0],seedMallets[1],s,delta,48000),"cache setup");};
        configure();seed(ref);seed(candidate);
        // Ongoing modulation must never adopt a cache built from mixed tuning.
        for(int frame=0;frame<5000;++frame) {
            if(frame%48==0) {s.frequency=240+.01*frame;configure();require(!candidate.tailCacheReady(),"configuration did synchronous preparation");}
            compare(ref,candidate,c,d);require(!candidate.composedTailActive(),"composed during continual retuning");
        }
        for(int frame=0;frame<4096;++frame)compare(ref,candidate,c,d);
        require(candidate.composedTailActive(),"settled modulation did not compose");d.gate();
        // Deliberate internal corruption tests finite recovery and accounting.
        for(DualBowlAdapter* h:{&ref,&candidate})
            const_cast<ModalState&>(h->engine().bowl().state(0)).x=std::numeric_limits<double>::quiet_NaN();
        const auto a=ref.process(c),b=candidate.process(c);
        require(a.fault&&b.fault&&b.audio.left==0&&b.audio.right==0,"nonfinite recovery");
        require(ref.engine().ledger().nonfiniteResets==candidate.engine().ledger().nonfiniteResets,"fault counter mismatch");
        Difference recovered;
        for(int frame=0;frame<4096;++frame)compare(ref,candidate,c,recovered);
        recovered.gate();
    }
    require(allocations==0,"audio processing allocated");
    std::cout<<"[PASS] Incremental cache invalidation/recovery, finite fault accounting and allocation-free processing/configuration\n";
}
void longTails() {
    Difference all;
    for(int bowl:{0,1}) for(auto quality:{ProcessingQuality::Balanced,ProcessingQuality::Reference}) {
        DualBowlAdapter ref,candidate;ref.setComposedTailEnabled(false);EngineSettings s;
        require(ref.configure(seedBowls[bowl],seedMallets[1],s,33,48000,quality)
            &&candidate.configure(seedBowls[bowl],seedMallets[1],s,33,48000,quality),"long-tail setup");
        seed(ref);seed(candidate);HostControls c;
        for(int frame=0;frame<60*48000;++frame)compare(ref,candidate,c,all);
    }
    all.gate();std::cout<<"[PASS] Four 60-second independent binaural tails; relative RMS "<<std::sqrt(all.error/all.signal)<<"\n";
}
}
int main() {
    try {matrix();transitions();cacheAndFaults();longTails();require(allocations==0,"allocation");}
    catch(const std::exception& e) {trap=false;std::cerr<<"[FAIL] "<<e.what()<<'\n';return 1;}
}

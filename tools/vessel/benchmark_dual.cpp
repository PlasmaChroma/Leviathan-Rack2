// Offline DSP timing, excluding setup and all file/UI work.
#include "../../src/vessel/DualBowlAdapter.hpp"
#include "../../src/vessel/SeedProfiles.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>
using namespace vessel;
volatile double sink=0;
void prime(DualBowlAdapter& h, bool rubbing) {
    HostControls c; c.rotate=rubbing;
    for (int i=0;i<3*48000;++i) { c.strikeEvent=i==0; if(h.process(c).fault) throw std::runtime_error("prime fault"); }
}
template<class Host> double measure(Host& h, bool rubbing) {
    HostControls c; c.rotate=rubbing; c.strikeEvent=true; h.process(c); c.strikeEvent=false;
    for (int i=0;i<3*48000;++i) h.process(c);
    double sum=0;
    auto begin=std::chrono::steady_clock::now();
    for (int i=0;i<4*48000;++i) {
        c.strikeEvent=rubbing && i%(2*48000)==0;
        auto f=h.process(c); if(f.fault) throw std::runtime_error("benchmark fault");
        sum+=f.audio.left+f.audio.right;
    }
    const double seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count();
    sink=sum; return seconds/(4*48000)*1e6;
}
int main() {
    std::cout<<"case,single_us_per_host_frame,always_dual_zero_us,optimized_zero_us,folded_zero_us,dual_33_us,folded_savings_percent\n"<<std::setprecision(8);
    for (bool rub : {false,true}) {
        double single=0,dualZero=0,zero=0,folded=0,dual=0;
        for(int repeat=0;repeat<3;++repeat) {
            HostRateAdapter a; DualBowlAdapter b(false), off, collapse, tuned; EngineSettings s;
            if(!a.configure(seedBowls[0],seedMallets[1],s,48000)
                || !b.configure(seedBowls[0],seedMallets[1],s,33,48000)
                || !off.configure(seedBowls[0],seedMallets[1],s,0,48000)
                || !collapse.configure(seedBowls[0],seedMallets[1],s,33,48000)
                || !tuned.configure(seedBowls[0],seedMallets[1],s,33,48000)) return 1;
            // Match both histories before requesting zero, so the savings
            // comparison includes a genuine divergent-to-single transition.
            prime(b,rub); prime(collapse,rub);
            if(!b.configure(seedBowls[0],seedMallets[1],s,0,48000)
                || !collapse.configure(seedBowls[0],seedMallets[1],s,0,48000)) return 1;
            if(repeat%2) { dual+=measure(tuned,rub);folded+=measure(collapse,rub);zero+=measure(off,rub);dualZero+=measure(b,rub);single+=measure(a,rub); }
            else { single+=measure(a,rub);dualZero+=measure(b,rub);zero+=measure(off,rub);folded+=measure(collapse,rub);dual+=measure(tuned,rub); }
            if(off.secondBowlActive() || collapse.secondBowlActive()) return 1;
        }
        std::cout<<(rub?"coupled_rubbing":"passive_tail")<<','<<single/3<<','<<dualZero/3<<','<<zero/3<<','<<folded/3<<','<<dual/3<<','<<100*(1-folded/dualZero)<<'\n';
    }
    DualBowlAdapter h; EngineSettings s; double sum=0;
    const auto begin=std::chrono::steady_clock::now();
    for(int i=0;i<4000;++i) { if(!h.configure(seedBowls[0],seedMallets[1],s,(i%34),48000)) return 1; sum+=h.separationHz(); }
    const double us=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count()*1e6/4000;
    sink=sum; std::cout<<"dual_configuration_us,"<<us<<",,,,,\n";
    for (double separation : {0.,33.}) {
        DualBowlAdapter tuning; EngineSettings settings;
        if (!tuning.configure(seedBowls[0],seedMallets[1],settings,separation,48000)) return 1;
        const auto start=std::chrono::steady_clock::now();
        for (int i=0;i<4000;++i) {
            settings.frequency=240.+.1*(i%400);
            if (!tuning.configure(seedBowls[0],seedMallets[1],settings,separation,48000)) return 1;
            sum+=tuning.centerFrequency();
        }
        sink=sum;
        const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()*1e6/4000;
        std::cout<<(separation==0?"single_pitch_configuration_us,":"dual_pitch_configuration_us,")<<elapsed<<",,,,,\n";
    }
}

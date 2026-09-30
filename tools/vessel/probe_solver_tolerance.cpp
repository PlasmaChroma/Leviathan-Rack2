// Offline-only experiment. The runner compiles isolated copies of the core.
#include "vessel/DualBowlAdapter.hpp"
#include "vessel/SeedProfiles.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace vessel;
volatile double probeSink = 0;
struct Fixture { const char* name; int bowl, mallet; double speed, pressure, pitch, separation; bool reverse, sweep; };
const Fixture fixtures[] = {
    {"metal_suede",0,1,.4,2.5,261.625565,0,false,false},
    {"metal_wood_coupled",0,0,.7,8,261.625565,0,false,false},
    {"crystal_suede",1,1,.4,2.5,261.625565,0,false,false},
    {"silicone_reversal",0,2,.4,15,110,0,true,false},
    {"felt_slow",0,3,.0001,15,110,0,false,false},
    {"metal_pitch_sweep",0,1,.4,2.5,600,0,false,true},
    {"metal_binaural",0,1,.4,2.5,261.625565,33,false,false},
    {"crystal_high",1,0,.7,8,880,0,false,false}
};
struct Result { double us=0, residual=0, maxStep=0, iterations=0, rms=0; unsigned faults=0, rateChanges=0, maxIterations=0; double startRate=0,endRate=0; };
Result run(const Fixture& f, ProcessingQuality quality, double seconds, bool audit, const std::string& path) {
    const int frames=int(seconds*48000);
    DualBowlAdapter h; EngineSettings s; s.frequency=f.pitch;
    if(!h.configure(seedBowls[f.bowl],seedMallets[f.mallet],s,f.separation,48000,quality)) throw std::runtime_error("setup rejected");
    h.setAuditEnabled(audit); Result r; r.startRate=h.internalRate();
    std::vector<std::array<double,2>> audio;
    if(audit) audio.resize(frames);
    double square=0, lastRate=h.internalRate(), sum=0;
    const auto begin=std::chrono::steady_clock::now();
    for(int i=0;i<frames;++i) {
        const double t=double(i)/48000;
        if(f.sweep && i%48==0) {
            const double phase=t/seconds;
            s.frequency=600+800*(phase<.5?2*phase:2*(1-phase));
            if(!h.configure(seedBowls[f.bowl],seedMallets[f.mallet],s,f.separation,48000,quality)) throw std::runtime_error("retune rejected");
        }
        if(lastRate!=h.internalRate()) {++r.rateChanges;lastRate=h.internalRate();}
        HostControls c; c.rotate=t<seconds*.75;
        c.speed=f.reverse && t>=seconds*.4 ? -f.speed:f.speed; c.pressure=f.pressure;
        c.strikeEvent=i==2400 || i==int(seconds*.4*48000); c.velocity=.5;
        const auto frame=h.process(c);
        r.faults+=frame.fault;
        sum+=frame.audio.left+frame.audio.right;
        if(audit) {audio[i]={{frame.audio.left,frame.audio.right}}; square+=frame.audio.left*frame.audio.left+frame.audio.right*frame.audio.right;}
    }
    r.us=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count()*1e6/frames;
    probeSink=sum; r.endRate=h.internalRate();
    auto collect=[&](const VesselEngine& e) {
        const auto& l=e.ledger();
        const double residual=e.totalEnergy()+l.modalLoss+l.contactLoss+l.frictionLoss+l.retiredEnergy+l.recoveryLoss
            -l.launchWork-l.speedCapWork-l.handWork-l.radialWork;
        r.residual=std::max(r.residual,std::abs(residual));r.maxStep=std::max(r.maxStep,l.maxStepResidual);
        r.maxIterations=std::max(r.maxIterations,l.maxFrictionIterations);
        double steps=0,iterations=0;
        for(unsigned i=0;i<l.frictionIterationHistogram.size();++i) {steps+=l.frictionIterationHistogram[i];iterations+=i*l.frictionIterationHistogram[i];}
        if(steps)r.iterations=std::max(r.iterations,iterations/steps);
    };
    collect(h.engine());if(h.secondBowlActive())collect(h.rightEngine());
    if(audit) {
        r.rms=std::sqrt(square/(2*frames));
        std::ofstream out(path,std::ios::binary);out.write(reinterpret_cast<const char*>(audio.data()),audio.size()*sizeof(audio[0]));
        if(!out)throw std::runtime_error("audio write failed");
    }
    return r;
}
int main(int argc,char** argv) {
    if(argc!=4) {std::cerr<<"usage: probe OUTPUT_DIR SECONDS REPEATS\n";return 1;}
    try {
        const double seconds=std::stod(argv[2]);const int repeats=std::stoi(argv[3]);
        if(seconds<1 || seconds>120 || repeats<1 || repeats>10)throw std::runtime_error("invalid duration/repeats");
        std::cout<<"case,quality,start_rate,end_rate,rate_changes,us_per_frame,faults,max_ledger_residual,max_step_residual,mean_friction_iterations,max_friction_iterations,rms\n"<<std::setprecision(12);
        for(const auto& f:fixtures)for(auto quality:{ProcessingQuality::Reference,ProcessingQuality::Economy}) {
            const std::string id=std::string(f.name)+"_"+std::to_string(int(quality));
            // A separate run captures diagnostics/audio; timed runs disable auditing and I/O.
            const auto audited=run(f,quality,seconds,true,std::string(argv[1])+"/"+id+".f64");
            double mean=0;unsigned faults=audited.faults;
            for(int i=0;i<repeats;++i) {const auto timed=run(f,quality,seconds,false,"");mean+=timed.us;faults+=timed.faults;}
            std::cout<<f.name<<','<<int(quality)<<','<<audited.startRate<<','<<audited.endRate<<','<<audited.rateChanges<<','<<mean/repeats<<','<<faults<<','<<audited.residual<<','<<audited.maxStep<<','<<audited.iterations<<','<<audited.maxIterations<<','<<audited.rms<<'\n';
        }
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}

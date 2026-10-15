// Offline experiment. Compile only against the runner's instrumented core copy.
#include "vessel/DualBowlAdapter.hpp"
#include "vessel/SeedProfiles.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
using namespace vessel;
namespace {
constexpr double hostRate = 48000, internalRate = 192000;
volatile double sink = 0;
struct Fixture { const char* name; int bowl, mallet; double pitch, delta; bool impact; };
const Fixture fixtures[] = {
    {"metal_suede_zero",0,1,261.625565,0,false},
    {"metal_suede_1",0,1,261.625565,1,false},
    {"metal_suede_10",0,1,261.625565,10,false},
    {"metal_suede_33",0,1,261.625565,33,false},
    {"crystal_suede_33",1,1,261.625565,33,false},
    {"metal_felt_33",0,3,261.625565,33,false},
    {"crystal_wood_33",1,0,261.625565,33,false},
    {"metal_suede_low_33",0,1,55,33,false},
    {"metal_wood_impact_33",0,0,261.625565,33,true},
    {"metal_silicone_impact_33",0,2,261.625565,33,true},
};
double residual(const VesselEngine& e) {
    const auto& l=e.ledger();
    return e.totalEnergy()+l.modalLoss+l.contactLoss+l.frictionLoss+l.retiredEnergy+l.recoveryLoss
        -l.launchWork-l.speedCapWork-l.handWork-l.radialWork;
}
class Shared {
public:
    VesselEngine master;
    ModalBank slave;
    StereoDecimator filter;
    ModalVector observer;
    bool audit;
    double work=0, loss=0, maxResidual=0;
    unsigned faults=0;
    explicit Shared(const Fixture& f, bool audited) : audit(audited) {
        const auto& bowl=seedBowls[f.bowl]; const auto& mallet=seedMallets[f.mallet];
        EngineSettings s; s.frequency=f.pitch-.5*f.delta;
        if(!master.configure(bowl,mallet,s,internalRate)
            || !slave.configure(bowl,f.pitch+.5*f.delta,s.decayMultiplier,s.imperfection,internalRate))
            throw std::runtime_error("shared configuration rejected");
        master.probeCaptureForce=true; master.setAuditEnabled(audit);
        observer=slave.observer(s.observerCenter+.5*s.observerSeparation);
        filter.configure(4);
    }
    void damping() {
        master.updateHighEnergyDamping();
        const double excess=std::min(1e6,std::max(0.0,(slave.energy()-.04)/.04));
        const double square=excess*excess;
        slave.setAdditionalDamping(.5*square/(1+square));
    }
    StereoSample process(const HostControls& c) {
        master.setRotation(c.rotate,c.speed,c.pressure);
        if(c.strikeEvent) master.strike(c.velocity,c.strikeVelocityScale);
        StereoSample out;
        for(unsigned k=0;k<4;++k) {
            const auto m=master.step();
            if(m.fault) ++faults;
            double velocity=0;
            if(audit) {
                const auto free=slave.freeMidpoint();
                const auto a=slave.commit(free,master.probeForce,true);
                work+=a.work; loss+=a.dampingLoss;
                maxResidual=std::max(maxResidual,std::abs(a.energyAfter+loss-work));
                velocity=slave.velocity(observer);
            } else if(!slave.probeAdvanceForced(master.probeForce,observer,velocity)) ++faults;
            StereoSample input; input.left=m.leftVelocity; input.right=velocity;
            filter.push(input,out);
        }
        // Match the reference adapter's per-host energy telemetry workload.
        sink=.5*(master.bowl().energy()+slave.energy());
        return out;
    }
};
void verifyFused(Shared audited, bool impact, int count) {
    auto fused=audited; fused.audit=false; fused.master.setAuditEnabled(false);
    for(int i=0;i<count;++i) {
        HostControls c; c.rotate=!impact;
        c.strikeEvent=i==0 || i==2048;
        c.speed=i<count/2 ? .4 : -.4;
        c.pressure=i<count/2 ? 2.5 : 15;
        if(i%48==0) { audited.damping(); fused.damping(); }
        const auto a=audited.process(c), b=fused.process(c);
        if(a.left!=b.left || a.right!=b.right || audited.slave.energy()!=fused.slave.energy()
            || audited.faults || fused.faults)
            throw std::runtime_error("audited/fused candidate mismatch or fault");
    }
}
HostControls controls(const Fixture& f, int i, int count) {
    HostControls c; c.rotate=!f.impact && i<count-int(5*hostRate);
    c.strikeEvent=f.impact && i==int(.05*hostRate);
    return c;
}
void write(const std::string& path,const std::vector<StereoSample>& data) {
    static_assert(sizeof(StereoSample)==2*sizeof(double),"packed audio");
    std::ofstream file(path,std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()),data.size()*sizeof(StereoSample));
    if(!file) throw std::runtime_error("capture write failed");
}
void run(const Fixture& f,const std::string& output,double seconds,int repeats) {
    const int count=int(seconds*hostRate);
    DualBowlAdapter reference(false); EngineSettings settings; settings.frequency=f.pitch;
    if(!reference.configure(seedBowls[f.bowl],seedMallets[f.mallet],settings,f.delta,hostRate))
        throw std::runtime_error("reference configuration rejected");
    reference.setAuditEnabled(true); Shared shared(f,true);
    std::vector<StereoSample> ref(count),candidate(count);
    double peakLeftError=0,peakZeroError=0,maxEnergy=0,maxRefResidual=0;
    unsigned refFaults=0;
    // Snapshot the established rub/tail, then time copies with auditing disabled.
    DualBowlAdapter settledRef=reference; Shared settledShared=shared;
    const int snapshot=count-int(10*hostRate);
    for(int i=0;i<count;++i) {
        if(i%48==0) { reference.updateHighEnergyDamping(); shared.damping(); }
        const auto c=controls(f,i,count);
        const auto r=reference.process(c); const auto s=shared.process(c);
        ref[i]=r.audio; candidate[i]=s; refFaults+=r.fault;
        peakLeftError=std::max(peakLeftError,std::abs(r.audio.left-s.left));
        if(f.delta==0) peakZeroError=std::max(peakZeroError,std::abs(r.audio.right-s.right));
        maxEnergy=std::max(maxEnergy,shared.slave.energy());
        if(i%48==0) maxRefResidual=std::max(maxRefResidual,
            std::max(std::abs(residual(reference.engine())),std::abs(residual(reference.rightEngine()))));
        if(i==snapshot) { settledRef=reference; settledShared=shared; }
    }
    if(peakLeftError!=0 || peakZeroError!=0 || refFaults || shared.faults
        || shared.maxResidual>1e-8 || maxRefResidual>1e-8)
        throw std::runtime_error("equivalence/fault/energy screening gate failed");
    write(output+"/"+f.name+"_reference.f64",ref);
    write(output+"/"+f.name+"_shared.f64",candidate);
    verifyFused(settledShared,f.impact,512);
    double refUs=0,sharedUs=0;
    auto timeReference=[&]() {
        auto h=settledRef; h.setAuditEnabled(false); double sum=0;
        HostControls c; c.rotate=!f.impact;
        const auto begin=std::chrono::steady_clock::now();
        for(int i=0;i<3*hostRate;++i) { if(i%48==0)h.updateHighEnergyDamping();
            const auto r=h.process(c); if(r.fault)throw std::runtime_error("timed reference fault"); sum+=r.audio.left+r.audio.right; }
        const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count();
        sink=sum; return elapsed*1e6/(3*hostRate);
    };
    auto timeShared=[&]() {
        auto h=settledShared; h.audit=false; h.master.setAuditEnabled(false); double sum=0;
        HostControls c; c.rotate=!f.impact;
        const auto begin=std::chrono::steady_clock::now();
        for(int i=0;i<3*hostRate;++i) { if(i%48==0)h.damping(); const auto r=h.process(c); sum+=r.left+r.right; }
        const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count();
        if(h.faults)throw std::runtime_error("timed shared fault");
        sink=sum; return elapsed*1e6/(3*hostRate);
    };
    for(int i=0;i<repeats;++i) {
        if(i%2) { sharedUs+=timeShared(); refUs+=timeReference(); }
        else { refUs+=timeReference(); sharedUs+=timeShared(); }
    }
    std::cout<<f.name<<','<<f.pitch<<','<<f.delta<<','<<f.impact<<','<<refUs/repeats<<','<<sharedUs/repeats
        <<','<<refFaults<<','<<shared.faults<<','<<maxRefResidual<<','<<shared.maxResidual
        <<','<<shared.work<<','<<shared.loss<<','<<maxEnergy<<','<<peakLeftError<<','<<peakZeroError<<std::endl;
}
}
int main(int argc,char** argv) {
    if(argc==2 && std::string(argv[1])=="--verify") {
        try {
            for(const auto& f:fixtures) { verifyFused(Shared(f,true),f.impact,4096);
                std::cout<<f.name<<": audited/fused equality PASS\n"; }
        } catch(const std::exception& e) { std::cerr<<e.what()<<'\n';return 1; }
        return 0;
    }
    if(argc!=4) { std::cerr<<"usage: probe OUTPUT SECONDS REPEATS\n"; return 1; }
    try {
        const double seconds=std::stod(argv[2]);const int repeats=std::stoi(argv[3]);
        if(seconds<20 || seconds>120 || repeats<1 || repeats>10)throw std::runtime_error("duration/repeats out of range");
        std::cout<<std::setprecision(17)<<"case,pitch,delta,impact,reference_us,shared_us,reference_faults,shared_faults,reference_residual,slave_residual,slave_input_work,slave_loss,slave_peak_energy,left_peak_error,zero_right_peak_error\n";
        for(const auto& f:fixtures) run(f,argv[1],seconds,repeats);
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n';return 1; }
}

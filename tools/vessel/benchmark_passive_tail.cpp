// Integrated shared-adapter A/B timing. All clocks are unpinned; no callback
// deadline guarantee. Copies and quantile collection are outside timed regions.
#include "../../src/vessel/DualBowlAdapter.hpp"
#include "../../src/vessel/SeedProfiles.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace vessel;
using Clock=std::chrono::steady_clock;
volatile double sink=0;
void require(bool okay) {if(!okay)throw std::runtime_error("benchmark invariant");}
double elapsed(Clock::time_point start) {return std::chrono::duration<double>(Clock::now()-start).count()*1e6;}
void seed(DualBowlAdapter& h) {
    for(const auto* e:{&h.engine(),&h.rightEngine()}) {
        auto& bank=const_cast<ModalBank&>(e->bowl());
        for(std::size_t j=0;j<bank.size();++j)require(bank.setState(j,.003*std::sin(double(j+1)),.003*std::cos(double(2*j+1))));
    }
}
double batch(std::vector<DualBowlAdapter>& hosts,const HostControls& c) {
    double sum=0;const auto start=Clock::now();
    for(auto& h:hosts) {const auto f=h.process(c);require(!f.fault);sum+=f.audio.left;}
    const double us=elapsed(start);sink=sum;return us;
}
void row(const char* phase,int quality,double delta,unsigned instances,const char* variant,std::vector<double> values) {
    std::sort(values.begin(),values.end());
    double sum=0;for(double x:values)sum+=x;
    std::cout<<phase<<','<<quality<<','<<delta<<','<<instances<<','<<variant<<','<<values.size()<<','
        <<sum/values.size()<<','<<values[values.size()/2]<<','<<values[(values.size()-1)*99/100]<<','<<values.back()<<'\n';
}
int main() {
    std::cout<<std::setprecision(10)<<"phase,quality,separation,instances,variant,samples,mean_us,median_us,p99_us,max_us\n";
    for(auto quality:{ProcessingQuality::Balanced,ProcessingQuality::Reference})for(double delta:{0.,33.}) {
        DualBowlAdapter reference,candidate;reference.setComposedTailEnabled(false);EngineSettings settings;
        require(reference.configure(seedBowls[0],seedMallets[1],settings,delta,48000,quality)
            &&candidate.configure(seedBowls[0],seedMallets[1],settings,delta,48000,quality));
        seed(reference);seed(candidate);HostControls c;
        std::vector<double> prepA,prepB;
        for(int frame=0;frame<4096;++frame) {
            auto start=Clock::now();const auto a=reference.process(c);const double at=elapsed(start);
            start=Clock::now();const auto b=candidate.process(c);const double bt=elapsed(start);
            require(!a.fault&&!b.fault);sink=a.audio.left+b.audio.left;
            if(!candidate.tailCacheReady()) {prepA.push_back(at);prepB.push_back(bt);}
        }
        require(candidate.composedTailActive());
        row("cache_preparation",int(quality),delta,1,"reference",prepA);
        row("cache_preparation",int(quality),delta,1,"candidate",prepB);
        for(bool rubbing:{false,true}) {
        HostControls steadyControls;steadyControls.rotate=rubbing;
        std::vector<double> steadyA,steadyB;
        for(int repeat=0;repeat<7;++repeat) {
            auto a=reference,b=candidate;
            if(rubbing)for(int frame=0;frame<4096;++frame) {a.process(steadyControls);b.process(steadyControls);}
            auto run=[&](DualBowlAdapter& h) {double sum=0;const auto start=Clock::now();
                for(int frame=0;frame<96000;++frame) {if(frame%48==0)h.updateHighEnergyDamping();const auto f=h.process(steadyControls);require(!f.fault);sum+=f.audio.left;}
                const double us=elapsed(start)/96000;sink=sum;return us;};
            double x,y;if(repeat%2) {y=run(b);x=run(a);}else {x=run(a);y=run(b);}
            steadyA.push_back(x);steadyB.push_back(y);
        }
        row(rubbing?"steady_rubbing":"steady_tail",int(quality),delta,1,"reference",steadyA);
        row(rubbing?"steady_rubbing":"steady_tail",int(quality),delta,1,"candidate",steadyB);
        }
        for(unsigned instances:{1u,8u,32u})for(bool rub:{false,true}) {
            std::vector<double> firstA,firstB,streamA,streamB;
            std::vector<DualBowlAdapter> a(instances),b(instances);
            for(int repeat=0;repeat<200;++repeat) {
                std::fill(a.begin(),a.end(),reference);std::fill(b.begin(),b.end(),candidate);
                HostControls contact;contact.rotate=rub;contact.strikeEvent=!rub;
                double x,y;if(repeat%2) {y=batch(b,contact);x=batch(a,contact);}else {x=batch(a,contact);y=batch(b,contact);}
                firstA.push_back(x);firstB.push_back(y);contact.strikeEvent=false;
                while(b.front().tailHandoffActive()) {
                    if(repeat%2) {y=batch(b,contact);x=batch(a,contact);}else {x=batch(a,contact);y=batch(b,contact);}
                    streamA.push_back(x);streamB.push_back(y);
                }
            }
            row(rub?"rub_onset":"strike_onset",int(quality),delta,instances,"reference",firstA);
            row(rub?"rub_onset":"strike_onset",int(quality),delta,instances,"candidate",firstB);
            row(rub?"rub_handoff":"strike_handoff",int(quality),delta,instances,"reference",streamA);
            row(rub?"rub_handoff":"strike_handoff",int(quality),delta,instances,"candidate",streamB);
        }
    }
}

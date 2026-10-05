// Rack-linked headless Vessel::process timing, including controls/telemetry.
// This does not include Rack's scheduler, audio driver, or GUI.
#include <context.hpp>
#include <engine/Engine.hpp>
#undef PRIVATE
#include "../../src/Vessel.hpp"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

bool isDragonKingDebugEnabled() { return false; }
Model* modelVessel=nullptr;
Model* modelVTune=nullptr;
namespace rack { Context::~Context() {} }
namespace {
using Clock=std::chrono::steady_clock;
volatile float sink=0;
void require(bool okay) {if(!okay)throw std::runtime_error("module benchmark invariant");}
double elapsed(Clock::time_point start) {return std::chrono::duration<double>(Clock::now()-start).count()*1e6;}
Module::ProcessArgs args() {Module::ProcessArgs a;a.sampleRate=48000;a.sampleTime=1.f/48000;a.frame=0;return a;}
void step(Vessel& h,int frames) {auto a=args();for(int i=0;i<frames;++i) {a.frame=i;h.process(a);}require(!h.visualFault.load());}
void prime(Vessel& h,int quality,double delta,bool enable) {
    h.requestedQuality.store(quality);h.params[Vessel::BINAURAL_PARAM].setValue(float(delta));
    h.audio.setComposedTailEnabled(enable);
    h.inputs[Vessel::STRIKE_INPUT].channels=1;h.inputs[Vessel::ROTATE_INPUT].channels=1;
    h.inputs[Vessel::STRIKE_INPUT].setVoltage(10);step(h,1);
    h.inputs[Vessel::STRIKE_INPUT].setVoltage(0);step(h,48000);
    require(!h.visualSleeping.load());if(enable)require(h.audio.composedTailActive());
}
void row(const char* phase,int quality,double delta,unsigned instances,const char* variant,std::vector<double> values) {
    std::sort(values.begin(),values.end());double sum=0;for(double v:values)sum+=v;
    std::cout<<phase<<','<<quality<<','<<delta<<','<<instances<<','<<variant<<','<<values.size()<<','
        <<sum/values.size()<<','<<values[values.size()/2]<<','<<values[(values.size()-1)*99/100]<<','<<values.back()<<'\n';
}
using Group=std::vector<std::unique_ptr<Vessel>>;
double process(Group& modules) {
    const auto a=args();const auto start=Clock::now();float sum=0;
    for(auto& m:modules) {m->process(a);sum+=m->outputs[Vessel::LEFT_OUTPUT].getVoltage();}
    const double us=elapsed(start);sink=sum;return us;
}
void run() {
    std::cout<<std::setprecision(10)<<"phase,quality,separation,instances,variant,samples,mean_us,median_us,p99_us,max_us\n";
    for(int quality:{1,2})for(double delta:{0.,33.}) {
        Vessel reference,candidate;prime(reference,quality,delta,false);prime(candidate,quality,delta,true);
        const auto referenceSeed=reference.audio,candidateSeed=candidate.audio;
        for(bool rubbing:{false,true}) {
        reference.inputs[Vessel::ROTATE_INPUT].setVoltage(rubbing?10:0);
        candidate.inputs[Vessel::ROTATE_INPUT].setVoltage(rubbing?10:0);
        std::vector<double> steadyA,steadyB;
        for(int repeat=0;repeat<7;++repeat) {
            reference.audio=referenceSeed;candidate.audio=candidateSeed;
            if(rubbing) {step(reference,4096);step(candidate,4096);}
            auto timed=[&](Vessel& h) {auto a=args();float sum=0;const auto start=Clock::now();
                for(int frame=0;frame<48000;++frame) {a.frame=frame;h.process(a);sum+=h.outputs[Vessel::LEFT_OUTPUT].getVoltage();}
                const double us=elapsed(start)/48000;sink=sum;return us;};
            double a,b;if(repeat%2) {b=timed(candidate);a=timed(reference);}else {a=timed(reference);b=timed(candidate);}
            require(!candidate.visualSleeping.load()&&!candidate.visualFault.load());steadyA.push_back(a);steadyB.push_back(b);
        }
        row(rubbing?"steady_rubbing":"steady_tail",quality,delta,1,"reference",steadyA);row(rubbing?"steady_rubbing":"steady_tail",quality,delta,1,"candidate",steadyB);
        }
        for(unsigned instances:{1u,8u,32u})for(bool rub:{false,true}) {
            Group a,b;
            for(unsigned i=0;i<instances;++i) {a.emplace_back(new Vessel);b.emplace_back(new Vessel);
                prime(*a.back(),quality,delta,false);prime(*b.back(),quality,delta,true);}
            std::vector<double> firstA,firstB,streamA,streamB;
            for(int repeat=0;repeat<100;++repeat) {
                for(unsigned i=0;i<instances;++i) {
                    // Deliver low gates before restoring the saved tail. Module
                    // control/visual clocks intentionally keep their real cadence.
                    a[i]->inputs[Vessel::STRIKE_INPUT].setVoltage(0);b[i]->inputs[Vessel::STRIKE_INPUT].setVoltage(0);
                    a[i]->inputs[Vessel::ROTATE_INPUT].setVoltage(0);b[i]->inputs[Vessel::ROTATE_INPUT].setVoltage(0);
                    step(*a[i],1);step(*b[i],1);a[i]->audio=referenceSeed;b[i]->audio=candidateSeed;
                    const int input=rub?Vessel::ROTATE_INPUT:Vessel::STRIKE_INPUT;
                    a[i]->inputs[input].setVoltage(10);b[i]->inputs[input].setVoltage(10);
                }
                double x,y;if(repeat%2) {y=process(b);x=process(a);}else {x=process(a);y=process(b);}
                firstA.push_back(x);firstB.push_back(y);
                while(b.front()->audio.tailHandoffActive()) {
                    if(repeat%2) {y=process(b);x=process(a);}else {x=process(a);y=process(b);}
                    streamA.push_back(x);streamB.push_back(y);
                }
                for(unsigned i=0;i<instances;++i)require(!a[i]->visualFault.load()&&!b[i]->visualFault.load());
            }
            row(rub?"rub_onset":"strike_onset",quality,delta,instances,"reference",firstA);
            row(rub?"rub_onset":"strike_onset",quality,delta,instances,"candidate",firstB);
            row(rub?"rub_handoff":"strike_handoff",quality,delta,instances,"reference",streamA);
            row(rub?"rub_handoff":"strike_handoff",quality,delta,instances,"candidate",streamB);
        }
    }
}
}
int main() {
    rack::Context context;rack::contextSet(&context);int result=0;
    {rack::engine::Engine engine;context.engine=&engine;
        try {run();}catch(const std::exception& e) {std::cerr<<e.what()<<'\n';result=1;}
        context.engine=nullptr;
    }
    rack::contextSet(nullptr);return result;
}

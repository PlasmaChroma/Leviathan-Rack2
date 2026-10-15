#include "vessel/HostRateAdapter.hpp"
#include "vessel/SeedProfiles.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace vessel;
volatile double fastPathSink=0;
double measure(ProcessingQuality quality, bool rub, bool fastTail) {
    HostRateAdapter h; EngineSettings s;
    h.setFastTailEnabled(fastTail);
    if(!h.configure(seedBowls[0],seedMallets[1],s,48000,quality))throw std::runtime_error("setup");
    HostControls c;c.rotate=rub;
    for(int i=0;i<2*48000;++i) {c.strikeEvent=i==0;if(h.process(c).fault)throw std::runtime_error("warmup fault");}
    c.strikeEvent=false; double sum=0;
    const auto start=std::chrono::steady_clock::now();
    for(int i=0;i<4*48000;++i) {const auto f=h.process(c);if(f.fault)throw std::runtime_error("process fault");sum+=f.audio.left+f.audio.right;}
    fastPathSink=sum;
    return std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()*1e6/(4*48000);
}
int main() {
    try {
        std::cout<<"quality,case,reference_us_per_frame,fast_us_per_frame,saving_percent\n"<<std::setprecision(10);
        for(auto q:{ProcessingQuality::Economy,ProcessingQuality::Balanced,ProcessingQuality::Reference})
        for(bool rub:{false}) {
            double reference=0,fast=0;
            for(int repeat=0;repeat<3;++repeat) {
                if(repeat%2) {fast+=measure(q,rub,true);reference+=measure(q,rub,false);}
                else {reference+=measure(q,rub,false);fast+=measure(q,rub,true);}
            }
            std::cout<<int(q)<<','<<(rub?"friction":"free_tail")<<','<<reference/3<<','<<fast/3<<','<<100*(1-fast/reference)<<'\n';
        }
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}

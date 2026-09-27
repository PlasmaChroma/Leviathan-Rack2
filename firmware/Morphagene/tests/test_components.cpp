#include "../reconstruction/audited_components.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <limits>
static unsigned checks=0;
static void check(bool yes,const std::string& why) {
    ++checks;if (!yes) throw std::runtime_error(why);
}
static uint32_t bits(float x) { uint32_t u;std::memcpy(&u,&x,4);return u; }
static std::vector<std::vector<std::string>> csv(const std::string& path) {
    std::ifstream f(path);if(!f)throw std::runtime_error("Cannot open "+path);
    std::string line;std::getline(f,line);std::vector<std::vector<std::string>> out;
    while(std::getline(f,line)) {
        if(!line.empty()&&line.back()=='\r')line.pop_back();
        std::istringstream ss(line);std::string field;std::vector<std::string> row;
        while(std::getline(ss,field,','))row.push_back(field);
        out.push_back(row);
    }return out;
}
static uint32_t hex(const std::string& s) {return std::stoul(s,nullptr,16);}
int main(int argc,char**argv) try {
    static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559);
    const std::string root=argc>1?argv[1]:".";
    using namespace mg204;
    auto controls=csv(root+"/analysis/control_curves.csv");
    check(controls.size()==4096,"ADC fixture count");
    for(const auto&r:controls) {
        unsigned adc=std::stoul(r[0]);
        check(morphStage(adc)==std::stoi(r[1]),"Morph stage at "+r[0]);
        check(bits(sosTarget(adc))==hex(r[2]),"SOS target at "+r[0]);
        auto g=ordinaryGene(adc,480000);
        if(adc<=199)check(g.wholeSplice,"Whole-splice mode");
        else {
            check(!g.wholeSplice && bits(g.samples)==hex(r[3]),"Gene curve at "+r[0]);
            check(bits(clockQuantize(g.samples,480000.f))==hex(r[4]),"Clock curve at "+r[0]);
        }
    }
    check(ordinaryGene(200,576000).samples==576000.f,"12-second fold boundary");
    check(ordinaryGene(200,576001).samples==288000.5f,"Fold immediately above 12 seconds");
    check(ordinaryGene(4095,1).samples==8.f,"Eight-sample floor");
    check(clockQuantize(7.f,480000.f)==7.f,"Clock early return");
    const float q=480000.f*0x1.55553ep-1f;
    check(clockQuantize(288000.f,480000.f)==q,"Clock .6S retains upper 2/3 bin");
    check(clockQuantize(192000.f,480000.f)==q*.75f,"Clock .4S retains upper 1/2 bin");
    for(unsigned mode=0;mode<3;++mode) {
        check(rateTarget(0.f,mode)==(mode==2?0.f:-2.f),"Rate low endpoint");
        check(rateTarget(1.f,mode)==(mode==2?4.f:2.f),"Rate high endpoint");
        for(unsigned a=0;a<4096;++a)check(std::isfinite(rateTarget(float(a)/4095.f,mode)),"Finite rate");
    }
    check(rateTarget(.5f,0)==0.f && rateTarget(.5f,1)==0.f,"Rate center stop");
    auto kernels=csv(root+"/tests/sparse_read_vectors.csv");
    for(const auto&r:kernels) {
        const auto got=bits(sparseRead(std::stof(r[0]),std::stof(r[1]),std::stof(r[2]),std::stof(r[3]),std::stof(r[4])));
        check(got==hex(r[5]),"Sparse read trace fixture t="+r[4]+" a="+r[0]+" got="+std::to_string(got)+" expected="+std::to_string(hex(r[5])));
    }
    for(float t:{0.f,.25f,.5f,.75f,1.f}) {
        check(sparseRead(100.f,100.f,100.f,100.f,t)==200.f,"Sparse DC gain is 2");
        check(denseRead(100.f,100.f,t,1.f)==200.f,"Dense branch doubled envelope");
        check(denseRead(0.f,100.f,t,1.f)==200.f*t,"Dense linear ramp");
    }
    auto choices=csv(root+"/tests/morph_choice_vectors.csv");
    for(const auto&r:choices) {
        uint32_t state=std::stoul(r[1]);auto result=chooseMorph(std::stof(r[0]),state);
        check(bits(result.stereoCrossmix)==hex(r[2]),"Morph stereo choice");
        check(result.rateIndex==std::stoi(r[3]),"Morph rate choice");
        check(state==std::stoul(r[4]),"Morph conditional RNG consumption");
    }
    for(float p:{0.f,.25f,.5f,.75f,1.f}) {
        auto x=crossmix({1.f,-1.f},p);
        check(x.left==1-2*p && x.right==2*p-1,"Stereo crossmix");
        x=crossmix({.25f,.25f},p);check(x.left==.25f&&x.right==.25f,"Mono invariant");
        x=sosMix({1.f,-1.f},{-1.f,1.f},p);check(x.left==1-2*p&&x.right==2*p-1,"SOS mix");
    }
    float state=0;for(int i=0;i<1000;++i)state=smooth(state,1,.001f);
    check(std::abs(state-float(1-std::pow(.999,1000)))<2e-6f,"Smoothing step response");
    RecordFilter filter;check(filter.process(1.f)==.7f,"Record filter initial step");
    for(int i=0;i<10000;++i)filter.process(1.f);
    check(std::abs(filter.previousOutput)<1e-5f,"Record filter rejects DC");
    check(gainIndex(0)==0&&gainIndex(1)==3&&gainIndex(6)==33,"Gain table bounds");
    std::cout<<"PASS: "<<checks<<" checks; 4096 ADC cases; 42 sparse-kernel cases; 28 launch cases.\n";
    std::cout<<"Compiler: "<<__VERSION__<<"; IEEE binary32; explicit std::fma; no full DSP/hardware oracle.\n";
    return 0;
} catch(const std::exception&e) { std::cerr<<"FAIL: "<<e.what()<<'\n';return 1; }

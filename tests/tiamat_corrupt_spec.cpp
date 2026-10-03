#include "Tiamat/TiamatCore.hpp"
#include "Tiamat/TiamatMath.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <sstream>
#include <string>
#include <vector>

static bool realtime = false;
static unsigned allocations = 0, deletions = 0;
void* operator new(std::size_t n) { if (realtime) ++allocations; if (void* p = std::malloc(n ? n : 1)) return p; throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { if (realtime && p) ++deletions; std::free(p); }
void operator delete[](void* p) noexcept { ::operator delete(p); }
namespace tiamat {
struct CorruptTestAccess {
    static void process(Corrupt& c, const float* x, float* y, unsigned n, CorruptRoutingState& s, Random& r) { c.processFrames(x,y,n,s,r); }
    static VinylState& vinyl(Corrupt& c) { return c.vinyl_; }
};
struct OutputTestAccess {
    static float gain(const Output& o, float x) { return o.sineGain(x); }
    static OutputState& state(Output& o) { return o.state_; }
    static void bypassTone(Output& o) { o.tone_ = {1.f, 0.f}; }
    static void process(Output& o, const float* x, const float* w, float* y, unsigned n, float mix, float width) { o.processFrames(x,w,y,n,mix,width); }
};
struct RandomTestAccess {
    static void inject(Random& r, const std::uint32_t* sequence) { r.sequence_=sequence; r.sequenceSize_=1; r.sequenceIndex_=0; }
    static unsigned draws(const Random& r) { return r.sequenceIndex_; }
};
}
using namespace tiamat;
static std::uint32_t bits(float x) { std::uint32_t b; std::memcpy(&b,&x,4); return b; }
static float readFloat(std::istream& in) { std::uint32_t b; in >> b; float x; std::memcpy(&x,&b,4); assert(in); return x; }
static void tag(std::istream& in, const char* expected) { std::string s; in >> s; assert(s==expected); }
static void exact(float a, float b, const char* where, unsigned i) {
    if (bits(a)!=bits(b)) { std::cerr << where << " at " << i << " got " << bits(a) << " expected " << bits(b) << '\n'; std::abort(); }
}
static std::uint64_t hash(const float* samples, unsigned count) {
    std::uint64_t h=UINT64_C(14695981039346656037);
    for (unsigned i=0;i<count;++i) {
        const auto b=bits(samples[i]);
        for(unsigned shift=0;shift<32;shift+=8) h=(h ^ ((b>>shift)&255u))*UINT64_C(1099511628211);
    }
    return h;
}
static void writeFloatWave(const char* path, const std::vector<float>& samples) {
    std::ofstream f(path,std::ios::binary); assert(f);
    auto integer=[&](std::uint32_t n,unsigned bytes) { for(unsigned i=0;i<bytes;++i) f.put(char((n>>(i*8))&255u)); };
    const auto size=std::uint32_t(samples.size()*4);
    f.write("RIFF",4); integer(36+size,4); f.write("WAVEfmt ",8); integer(16,4);
    integer(3,2); integer(2,2); integer(48000,4); integer(48000*8,4); integer(8,2); integer(32,2);
    f.write("data",4); integer(size,4);
    for(auto x:samples) integer(bits(x),4);
    assert(f);
}
static void fixtures() {
    std::ifstream file("tests/fixtures/tiamat/corrupt_v1.txt"); assert(file);
    std::string text,line;
    while(std::getline(file,line)) if(!line.empty() && line[0]!='#') text+=line+'\n';
    std::istringstream in(text);
    float input[256],output[256];
    tag(in,"INPUT"); for(auto& x:input) x=readFloat(in);
    unsigned shorts=0,longs=0,rares=0,dropouts=0,mixes=0;
    float djPeak=0;
    std::string kind;
    while(in>>kind) {
        if(kind=="SHORT") {
            int mode; in>>mode; float u=readFloat(in);
            Corrupt c; Random random; CorruptRoutingState route; route.primary=Effect(mode); route.amount=u;
            CorruptTestAccess::process(c,input,output,128,route,random);
            for(unsigned i=0;i<256;++i) {
                const float e=readFloat(in);
                if(mode==4) { djPeak=std::max(djPeak,std::abs(output[i]-e)); assert(std::abs(output[i]-e)<=2e-5f); }
                else if(bits(output[i])!=bits(e)) { std::cerr<<"mode="<<mode<<" amount="<<u<<" "; exact(output[i],e,"short",i); }
            }
            ++shorts;
        } else if(kind=="LONG") {
            const float amount=readFloat(in); bool pause; unsigned blocks; in>>pause>>blocks;
            Corrupt c; Random random; CorruptRoutingState route; route.primary=Effect::Vinyl;
            float zero[192]={};
            for(unsigned b=0;b<blocks;++b) {
                route.amount=pause && b*96>=47904 && b*96<48096 ? .05f : amount;
                c.processBlock(zero,output,route,random);
                tag(in,"BLOCK"); std::uint64_t expected; in>>expected;
                if(hash(output,192)!=expected) { std::cerr<<"long amount="<<amount<<" pause="<<pause<<" block="<<b<<" hash mismatch\n"; std::abort(); }
                const auto& v=c.vinyl();
                const std::uint32_t actual[]={v.dustSeed,v.slowSeed,v.fastSeed,v.slowCounter,v.modulationCounter,bits(v.modulation),bits(v.slowValue),
                    v.dust[0].counter,v.dust[1].counter,v.dust[2].counter,v.dust[3].counter,v.dust[4].counter,
                    bits(v.dust[0].value),bits(v.dust[1].value),bits(v.dust[2].value),bits(v.dust[3].value),bits(v.dust[4].value)};
                for(auto a:actual) { std::uint32_t e; in>>e; assert(a==e); }
            }
            ++longs;
        } else if(kind=="RARE") {
            unsigned layer,seed,frames; in>>layer>>seed>>frames;
            Corrupt c; Random random; CorruptRoutingState route; route.primary=Effect::Vinyl; route.amount=1;
            auto& v=CorruptTestAccess::vinyl(c); v.dustSeed=seed;
            for(unsigned i=0;i<5;++i) v.dust[i].counter=i<2 ? 4 : 14;
            const float x[]={.2f,-.1f};
            for(unsigned f=0;f<frames;++f) {
                CorruptTestAccess::process(c,x,output,1,route,random); tag(in,"SAMPLE");
                for(unsigned ch=0;ch<2;++ch) exact(output[ch],readFloat(in),"rare audio",f);
                for(unsigned d=0;d<5;++d) exact(v.dust[d].value,readFloat(in),"rare dust",f);
            }
            ++rares;
        } else if(kind=="DROPOUT") {
            Corrupt c; Random random; CorruptRoutingState route; route.primary=Effect::Dropout; route.amount=readFloat(in);
            bool finalGate; unsigned value,draws,frames; in>>route.dropoutOpen>>value>>finalGate>>draws>>frames;
            float x[192]; for(unsigned i=0;i<frames*2;++i) x[i]=readFloat(in);
            RandomTestAccess::inject(random,&value);
            CorruptTestAccess::process(c,x,output,frames,route,random);
            for(unsigned i=0;i<frames*2;++i) exact(output[i],readFloat(in),"dropout",i);
            assert(route.dropoutOpen==finalGate && RandomTestAccess::draws(random)==draws);
            ++dropouts;
        } else if(kind=="MIX") {
            const float initial=readFloat(in),width=readFloat(in),smoothed=readFloat(in);
            const float expected[]={readFloat(in),readFloat(in)};
            Output o; OutputTestAccess::bypassTone(o); OutputTestAccess::state(o).mix=initial;
            const float dry[]={1,0},wet[]={0,1};
            OutputTestAccess::process(o,dry,wet,output,1,initial==0 && smoothed>0 ? 1.f : initial,width);
            exact(o.state().mix,smoothed,"mix smoothing",mixes);
            for(unsigned i=0;i<2;++i) assert(std::abs(output[i]-expected[i])<2e-7f);
            ++mixes;
        } else { std::cerr<<"Unknown record "<<kind; std::abort(); }
        assert(in);
    }
    assert(shorts==64 && longs==3 && rares==5 && dropouts==7 && mixes==16);
    std::cout<<"64 short, 864288 long Vinyl frames, 5 rare layers, 7 Dropout, 16 output fixtures; DJ peak error "<<djPeak<<'\n';
}

static void routingAndRestart() {
    float x[192],a[192],b[192]; for(unsigned i=0;i<192;++i) x[i]=float(int(i%17)-8)*.08f;
    Corrupt c,other; Random rng,control; CorruptRoutingState route;
    route.primary=Effect::Retained; route.retainedEnabled=false;
    c.processBlock(x,a,route,rng); assert(std::memcmp(x,a,sizeof(x))==0 && rng.state()==1);
    route.retained=Effect::Dropout; route.retainedAmount=1; route.retainedEnabled=true;
    control.next(); c.processBlock(x,a,route,rng); assert(rng.state()==control.state());
    const auto state=rng.state(); route.primary=Effect::Vinyl; route.amount=.8f;
    other.processBlock(x,b,route,control); c.processBlock(x,a,route,rng);
    assert(std::memcmp(a,b,sizeof(a))==0 && rng.state()==state);
    // A third instance must not influence either stream.
    Corrupt interloper(77); CorruptRoutingState noise=route; Random unused;
    for(unsigned i=0;i<40;++i) interloper.processBlock(x,b,noise,unused);
    other.processBlock(x,b,route,control); c.processBlock(x,a,route,rng); assert(std::memcmp(a,b,sizeof(a))==0);
    auto before=c.vinyl(); c.restartRandom(UINT64_C(0x80000000));
    Corrupt fresh(1); const auto& reset=c.vinyl();
    assert(reset.dustSeed==fresh.vinyl().dustSeed && reset.fastSeed==1 && reset.slowSeed==1);
    assert(reset.modulationCounter==0 && reset.slowCounter==0 && reset.modulationPeriod==48014 && reset.modulation==0 && reset.slowValue==0);
    for(unsigned i=0;i<5;++i) { assert(reset.dust[i].counter==0 && reset.dust[i].value==0); assert(reset.dust[i].threshold==before.dust[i].threshold); }
    for(unsigned i=0;i<2;++i) { assert(reset.signalHighpass[i].previous==before.signalHighpass[i].previous); assert(reset.noiseHighpass[i].previous==before.noiseHighpass[i].previous); }
    route.amount=.05f; before=c.vinyl(); c.processBlock(x,a,route,rng);
    assert(std::memcmp(x,a,sizeof(x))==0 && before.dustSeed==c.vinyl().dustSeed && before.modulationCounter==c.vinyl().modulationCounter);
    route.primary=Effect::Destroy; route.amount=1; c.processBlock(x,a,route,rng);
    const float blend=c.destroyBlend(); c.restartRandom(9); assert(c.destroyBlend()==blend);
    // Persistent Dropout toggles, with bypass forcing open and consuming no draw.
    route.primary=Effect::Dropout; route.amount=1; route.dropoutOpen=true;
    const std::uint32_t zero=0; RandomTestAccess::inject(rng,&zero);
    c.processBlock(x,a,route,rng); assert(!route.dropoutOpen);
    c.processBlock(x,a,route,rng); assert(route.dropoutOpen && RandomTestAccess::draws(rng)==2);
    route.amount=.03f; c.processBlock(x,a,route,rng); assert(route.dropoutOpen && RandomTestAccess::draws(rng)==2);
    // Original-code focused probe: initial gate open, four frames, only rand
    // substituted. Reversing the two float multiplies flips both decisions.
    const std::uint32_t amounts[]={1028865070u,1038887682u},draws[]={1u,5u};
    for(unsigned i=0;i<2;++i) {
        std::memcpy(&route.amount,&amounts[i],4); route.dropoutOpen=true;
        RandomTestAccess::inject(rng,&draws[i]);
        CorruptTestAccess::process(c,x,a,4,route,rng);
        assert(route.dropoutOpen==(i==0) && RandomTestAccess::draws(rng)==1);
    }
}

static void outputAndSafety() {
    Output o; float peak=0;
    for(unsigned i=0;i<=1000000;++i) {
        const float phase=float(i)/1000000.f;
        const float reference=std::sin(phase*float(1.5707963267948966));
        peak=std::max(peak,std::abs(OutputTestAccess::gain(o,phase)-reference));
    }
    assert(peak<=2e-7f);
    float l=0,r=1; Output::width(l,r,.5f); assert(l==.5f && r==.75f);
    float x[192]={},wet[192]={},out[192]; x[0]=2.f; x[1]=-3.f;
    o.processBlock(x,wet,out,0,0);
    const auto coeff=outputToneCoefficients(); float hl=0,hr=0;
    for(unsigned i=0;i<96;++i) {
        hl=multiplyAdd(coeff.feedforward,x[i*2],coeff.feedback*hl);
        hr=multiplyAdd(coeff.feedforward,x[i*2+1],coeff.feedback*hr);
        exact(out[i*2],hl,"dry Tone L",i); exact(out[i*2+1],hr,"dry Tone R",i);
    }
    assert(out[0]>1.f); // No nominal-range clip.
    // Full-chain A/B against per-frame reference sine, including a moving target.
    Output test; OutputState reference; float audioPeak=0; const auto tone=outputToneCoefficients();
    const bool render=std::getenv("TIAMAT_AB_RENDER")!=nullptr;
    std::vector<float> actualRender,referenceRender;
    for(unsigned b=0;b<600;++b) {
        const float target=float((b/31)%11)/10.f;
        for(unsigned i=0;i<192;++i) { x[i]=float(int((i+b)%31)-15)/8.f; wet[i]=float(int((i*7+b)%29)-14)/8.f; }
        test.processBlock(x,wet,out,target,.3f);
        for(unsigned f=0;f<96;++f) {
            reference.mix=multiplyAdd(target-reference.mix,.001f,reference.mix);
            const float gd=std::sin((1-reference.mix)*float(1.5707963267948966)),gw=std::sin(reference.mix*float(1.5707963267948966));
            float pair[2];
            for(unsigned ch=0;ch<2;++ch) {
                const unsigned i=f*2+ch;
                const float mixed=multiplyAdd(gw,wet[i],x[i]*gd);
                pair[ch]=reference.toneHistory[ch]=multiplyAdd(tone.feedforward,mixed,tone.feedback*reference.toneHistory[ch]);
            }
            Output::width(pair[0],pair[1],.3f);
            for(unsigned ch=0;ch<2;++ch) {
                audioPeak=std::max(audioPeak,std::abs(pair[ch]-out[f*2+ch]));
                if(render) { actualRender.push_back(out[f*2+ch]); referenceRender.push_back(pair[ch]); }
            }
        }
        exact(test.state().mix,reference.mix,"A/B smoother",b);
    }
    assert(audioPeak<1e-6f);
    if(render) {
        writeFloatWave("build/tiamat-output-lookup.wav",actualRender);
        writeFloatWave("build/tiamat-output-reference.wav",referenceRender);
    }
    Corrupt c; CorruptRoutingState route; Random random;
    for(unsigned mode=1;mode<=5;++mode) {
        route.primary=Effect(mode); route.amount=1;
        for(unsigned i=0;i<192;++i) x[i]=i%3==0 ? std::numeric_limits<float>::quiet_NaN() : i%3==1 ? std::numeric_limits<float>::infinity() : std::numeric_limits<float>::max();
        c.processBlock(x,wet,route,random); o.processBlock(x,wet,out,1,.5f);
        for(auto y:out) assert(std::isfinite(y));
        std::fill(x,x+192,0.f); c.processBlock(x,out,route,random); for(auto y:out) assert(std::isfinite(y));
    }
    std::cout<<"Sine lookup peak gain error "<<peak<<"; 57600-frame moving-mix A/B peak "<<audioPeak<<'\n';
}

static void coreIntegration() {
    Core core; BufferBlockInput block; float input[192],output[192];
    for(unsigned i=0;i<192;++i) input[i]=float(int(i%23)-11)*.06f;
    block.primary.mix=0; core.processBlock(input,output,block);
    Output dryReference; float dry[192],zero[192]={}; dryReference.processBlock(input,zero,dry,0,0);
    assert(std::memcmp(dry,output,sizeof(dry))==0);
    // Restart joins both streams at the same boundary without clearing buffer,
    // clock phase or output filter state. Macro flags are disabled here.
    block.restartRandom=true; block.seed=57;
    core.eventState().controls.effect=Effect::Vinyl; block.primary.corrupt=1;
    const auto frames=core.bufferEngine().buffer().channel(0).writer.writePosition;
    core.processBlock(input,output,block);
    Corrupt stochastic(57); CorruptRoutingState route; route.primary=Effect::Vinyl; route.amount=1; Random random;
    stochastic.processBlock(zero,dry,route,random);
    assert(core.corrupt().vinyl().dustSeed==stochastic.vinyl().dustSeed);
    assert(core.bufferEngine().randomState()==57 && core.bufferEngine().buffer().channel(0).writer.writePosition!=frames);
    block.restartRandom=false;
    double total=0,max=0;
    realtime=true;
    for(unsigned b=0;b<2500;++b) {
        core.eventState().controls.effect=Effect(1+b%5);
        block.primary.corrupt=float(b%101)/100.f;
        block.primary.mix=float(b%61)/60.f;
        block.primary.bend=float(b%37)/36.f;
        const auto start=std::chrono::steady_clock::now();
        core.processBlock(input,output,block);
        const double us=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-start).count();
        total+=us; max=std::max(max,us);
        for(auto y:output) assert(std::isfinite(y));
    }
    realtime=false;
    assert(allocations==0 && deletions==0);
    std::cout<<"Complete core: 2500 blocks, zero allocations/deletions, mean "<<total/2500<<" us, max "<<max<<" us\n";
}

static void compositionOrder() {
    Core complete; BufferEngine buffer; Corrupt corrupt; Output output;
    BufferBlockInput block;
    block.primary.time=1; block.primary.bend=.9f; block.primary.brk=.8f;
    float x[192],actual[192],wet[192],expected[192];
    for(unsigned b=0;b<400;++b) {
        auto& c=complete.eventState(); auto& e=buffer.eventState();
        c.controls.macroBend=e.controls.macroBend=true;
        c.controls.macroBreak=e.controls.macroBreak=true;
        c.controls.effect=e.controls.effect=Effect(1+(b/17)%5);
        c.settings.unique=e.settings.unique=(b/43)%2==0;
        c.controls.buttonFreeze=e.controls.buttonFreeze=b>=130 && b<260;
        block.primary.corrupt=float(b%101)/100.f;
        block.primary.mix=float(b%61)/60.f;
        block.restartRandom=b==71 || b==211; block.seed=UINT64_C(0x123456789);
        for(unsigned i=0;i<192;++i) x[i]=float(int((i*11+b)%47)-23)/24.f;
        // Explicitly reconstruct the required order, carrying Dropout's stream
        // state back to the Buffer stage through its shared-stream entry point.
        if(block.restartRandom) corrupt.restartRandom(block.seed);
        buffer.processBlock(x,wet,block);
        buffer.processCorrupt(corrupt,wet,wet);
        output.processBlock(x,wet,expected,buffer.mapped().mixTarget,buffer.mapped().crossfeed);
        // Exercise the supported in-place full core path as well.
        std::copy(x,x+192,actual); complete.processBlock(actual,actual,block);
        assert(std::memcmp(actual,expected,sizeof(actual))==0);
        assert(complete.bufferEngine().randomState()==buffer.randomState());
        assert(complete.bufferEngine().corruptRouting().dropoutOpen==buffer.corruptRouting().dropoutOpen);
    }
    assert(buffer.clockBoundaryCount()>30);
    std::cout<<"400 complete-core composition blocks: Macro, Freeze, effect switches, restart, aliasing and shared RNG order passed\n";
}
int main() { fixtures(); routingAndRestart(); outputAndSafety(); coreIntegration(); compositionOrder(); std::cout<<"Tiamat Corrupt/output tests passed\n"; }

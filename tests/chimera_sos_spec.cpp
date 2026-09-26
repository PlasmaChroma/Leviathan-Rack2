#include "ChimeraSlice.hpp"
#include <cstdio>
#include <cstdlib>
#include <new>
#include <limits>

static bool audioThread = false;
void* operator new(std::size_t n) {
    if (audioThread) { std::fprintf(stderr,"SOS audio allocation\n"); std::abort(); }
    if (void* p=std::malloc(n)) return p;
    throw std::bad_alloc();
}
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
static void need(bool value,const char* message) {
    if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}
}
static bool near(double a,double b,double tolerance=2e-6){return std::fabs(a-b)<tolerance;}
static chimera::CoreInput input(float control) {
    chimera::CoreInput in{};
    in.controls.sos=control;in.controls.rate=5.f/6.f;
    in.controls.morph=400.f/4096.f;
    in.live={5.f,-2.5f};
    return in;
}
int main() {
    using namespace chimera;
    namespace s = soundOnSound;
    need(s::targetAdc(0)==0 && s::targetAdc(185)==0 && s::targetAdc(186)>0 &&
         near(s::targetAdc(2048),.490817) && near(s::targetAdc(3980),.999908) &&
         s::targetAdc(3981)==1 && s::targetAdc(4095)==1,"recovered ADC boundaries");
    for(int code=0;code<4096;++code) {
        need(s::adc(code/4096.f)==code,"normalized control preserves every ADC bin");
        need(near(s::target(code/4096.f),profile1::clamp(code*.0002635046258-.04884000123,0,1)),
             "entire calibration follows recovered linear law");
    }
    need(s::adc(1)==4095 && s::adc(-1)==0 &&
         s::adc(std::numeric_limits<float>::quiet_NaN())==0,"bounded ADC conversion");
    need(profile1::sos(.75,false,0)==.75 && profile1::sos(.75,true,8)==.75 &&
         profile1::sos(.75,true,4)==.375 && profile1::sos(.75,true,0)==0 &&
         profile1::sos(.75,true,-5)==0 && profile1::sos(.75,true,24)==.75,
         "CV clamps before knob attenuation at eight volts");
    for(float coefficient : {0.f,.5f,1.f}) {
        const auto mix=s::mix({1,-.25f},{-1,.75f},coefficient);
        need(near(mix.l,1-2*coefficient) && near(mix.r,-.25+coefficient),
             "one complementary stereo crossfade including exact half mix");
    }
    Core controls;auto in=input(0);controls.step(in);in.controls.sos=1;
    double expected=0;
    for(int i=0;i<6000;++i){expected+=(1-expected)*.001;
        need(near(controls.step(in).sos,expected,1e-7),"48 kHz step uses recovered smoothing");}
    in.controls.sos=0;
    for(int i=0;i<6000;++i){expected+=(0-expected)*.001;
        need(near(controls.step(in).sos,expected,1e-7),"falling control step is equally smoothed");}
    // Every record mode uses the monitor mix by default; inop selects live
    // after that mix, without touching monitoring or applying SOS twice.
    for(bool append : {false,true}) for(bool inop : {false,true})
    for(float control : {0.f,2083.f/4096.f,1.f}) {
        Reel reel(4,4);for(unsigned i=0;i<128;++i)reel.write(i,{-.25f,.75f},i);
        Slice slice(&reel);slice.setConditioning(false);slice.setInop(inop);
        in=input(control);for(int i=0;i<128;++i)slice.step(in);
        need(append?slice.startAppend():slice.startCurrent(),"record mode starts");
        const float coefficient=s::target(control);
        const auto expectedMix=s::mix({1,-.5f},{-.25f,.75f},coefficient);
        audioThread=true;
        for(unsigned i=0;i<32;++i){const unsigned address=slice.writerPosition();
            const auto out=slice.step(in);const auto recorded=reel.readActive(address);
            need(near(out.audio.l,expectedMix.l) && near(out.audio.r,expectedMix.r),"monitor remains SOS mix");
            need(near(recorded.l,inop?1:out.audio.l) && near(recorded.r,inop?-.5:out.audio.r),
                 "Current and Append share bus or select conditioned live via inop");
            need(slice.writerPosition()==address+1,"record head advances forward");}
        audioThread=false;
    }
    for(float control : {0.f,2083.f/4096.f,.75f,1.f}) {
        Reel reel(1,1);reel.write(0,{.8f,-.3f},0);
        Slice slice(&reel);slice.setConditioning(false);in=input(control);in.live={0,0};
        slice.step(in);need(slice.startCurrent(),"identity overdub starts");
        const double coefficient=s::target(control);
        for(int pass=1;pass<=20;++pass){const auto out=slice.step(in);
            need(near(out.audio.l,.8*std::pow(coefficient,pass)) &&
                 near(reel.readActive(0).r,-.3*std::pow(coefficient,pass)),
                 "repeated identity passes retain SOS to the Nth power");}
    }
    // A separately rendered reference sees the same transformations; capture
    // that signal while the independent writer moves forward in both modes.
    for(bool append : {false,true}) for(bool inop : {false,true}) {
        Reel reel(20,20), reference(20,20);
        for(unsigned i=0;i<2400;++i){StereoFrame frame{float(i%211)/211.f,-float(i%127)/127.f};
            reel.write(i,frame,i);reference.write(i,frame,i);}
        Slice slice(&reel), monitor(&reference);slice.setConditioning(false);monitor.setConditioning(false);
        slice.setInop(inop);slice.setChordRatios(2,-1.5,4.0/3);monitor.setChordRatios(2,-1.5,4.0/3);
        in=input(.75f);in.controls.rate=1.f/6.f;in.controls.gene=1564.f/4095.f;
        in.controls.morph=1;in.controls.slide=.7f;
        for(int i=0;i<3000;++i){slice.step(in);monitor.step(in);}
        need(append?slice.startAppend():slice.startCurrent(),"transformed capture starts");
        audioThread=true;
        for(unsigned i=0;i<32;++i){const unsigned before=slice.writerPosition();
            const auto expectedOut=monitor.step(in);const auto out=slice.step(in);
            const unsigned address=(!append && i==0)?slice.recordSegmentStartFrame():before;
            const auto recorded=reel.readActive(address);
            need(near(out.audio.l,expectedOut.audio.l) && near(out.audio.r,expectedOut.audio.r),
                 "reversed Morph/Gene/Slide playback matches independent renderer");
            need(near(recorded.l,inop?1:out.audio.l) && near(recorded.r,inop?-.5:out.audio.r),
                 "rendered transformations enter recording only with inop zero");
            if (!append) need(reference.write(address,recorded,3000+i),
                 "reference renderer receives the same completed overdub write");
            need(slice.writerPosition()==address+1,"reverse playback never reverses writer");}
        audioThread=false;
    }
    Slice extreme;in=input(1);extreme.step(in);
    audioThread=true;
    for(int i=0;i<10000;++i){in.controls.sosCv=(i&1)?24:-24;in.controls.sosPatched=true;
        in.live={std::numeric_limits<float>::max(),-std::numeric_limits<float>::max()};
        const auto out=extreme.step(in);need(profile1::finite(out.audio.l)&&profile1::finite(out.audio.r),
            "extreme controls/audio stay finite without allocation");}
    audioThread=false;
    std::puts("PASS: SOS calibration, smoothing, CV, stereo, inop, overdubs, transformed capture and allocation trap");
}

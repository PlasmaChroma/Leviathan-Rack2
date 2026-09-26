#include "ChimeraGrains.hpp"
#include "ChimeraOptionsText.hpp"
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <new>

static bool audioThread = false;
void* operator new(std::size_t n) {
    if (audioThread) { std::fprintf(stderr, "allocation in Morph audio path\n"); std::abort(); }
    if (void* p = std::malloc(n)) return p;
    throw std::bad_alloc();
}
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
static void need(bool ok, const char* name) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", name); std::exit(1); }
}
int main() {
    using namespace chimera;
    const int starts[] = {0,121,242,362,483,603,724,844,965,1085,1206,
        1326,1447,1567,1688,1808,1929,2049,2170,2290,2411,2531,4096};
    for (int s = 0; s < 22; ++s)
        for (int code = starts[s]; code < starts[s+1]; ++code)
            need(morph::stageIndex(code/4096.f) == s, "all 4096 ADC bins match recovered ranges");
    need(morph::stageIndex(1.f) == 21 && morph::stageIndex(-1.f) == 0 &&
         morph::stageIndex(std::numeric_limits<float>::quiet_NaN()) == 0 &&
         morph::stageIndex(std::numeric_limits<float>::infinity()) == 0,
         "endpoints and malformed control bounded");
    need(!morph::stretchCandidate(1084.f/4096.f) &&
         morph::stretchCandidate(1085.f/4096.f), "clock integration at factor one-half");
    for (int s = 0; s < 22; ++s) {
        const auto factor = morph::stages[s];
        need(factor.cycleDenominator == factor.denominator, "recovered rational cycle");
        for (unsigned duration : {8u, 601u, 1383u}) {
            morph::Scheduler scheduler;
            scheduler.configure(duration, factor);
            unsigned launches = 0;
            for (unsigned frame = 1; frame <= 1000000; ++frame) {
                launches += scheduler.step();
                need(launches == std::uint64_t(frame)*factor.denominator /
                    (duration*factor.numerator), "rational schedules never gain or lose a launch");
            }
        }
    }
    morph::Continuous continuous;
    float expected = 0;
    for (int i = 0; i < 2000; ++i) {
        const float control = i < 1000 ? 0.75f : 0.25f;
        expected += (control - expected)*0.01f;
        need(continuous.step(control) == expected, "recovered one-pole rising/falling update");
    }
    need(morph::panDepth(.5) == 0 && morph::panDepth(.75) == .5 &&
         morph::pitchDepth(.6) == 0 && std::fabs(morph::pitchDepth(.8)-.5) < 1e-12,
         "independent progressive pan/pitch depths");
    const double ratios[] = {2,1.5,4.0/3.0};
    optionsText::Values defaults;
    need(defaults.mcr[0] == 2.f && defaults.mcr[1] == 1.5f &&
         defaults.mcr[2] == 4.f/3.f, "option parser shares recovered defaults");
    for (double control : {0., .5, .6}) {
        profile1::Xorshift32 random(17);
        for (int i=0;i<1000;++i) {
            const auto choice=profile1::chooseOnset(random,i%4,control,ratios);
            need(choice.ratio==1 && (control>.5 || choice.pan==0),
                "random distribution obeys recovered thresholds");
        }
    }
    profile1::Xorshift32 randomA(17), randomB(17);
    for (unsigned i = 0; i < 1000; ++i) {
        const auto a = profile1::chooseOnset(randomA,i%4,1,ratios);
        const auto b = profile1::chooseOnset(randomB,i%4,1,ratios);
        need(a.pan == b.pan && a.ratio == b.ratio &&
             a.ratio == (i%4 ? ratios[i%4-1] : 1), "seeded per-slot default ratios");
    }
    Reel reel(20,20);
    for (unsigned i=0;i<4800;++i) need(reel.write(i,{1.f,1.f},i),"prepare source");
    const Region region{0,4800};
    const int stages[] = {0,3,9,15,21};
    for (int s : stages) for (float rate : {0.f,.5f,1.f,2.f,-1.f,32.f,-32.f}) {
        Grains grains;
        CoreOutput c{}; c.gene=1564.f/4095.f; c.morph=starts[s]/4096.f; c.rate=rate;
        unsigned firstCompletion=0;
        audioThread=true;
        for (unsigned frame=0;frame<6000;++frame) {
            const auto out=grains.step(reel,region,c);
            if(out.completions && !firstCompletion) firstCompletion=frame;
            need(grains.onsetCount()==1+std::uint64_t(frame)*morph::stages[s].denominator /
                (600*morph::stages[s].numerator),"DSP output-time cadence independent of Vari-Speed");
            need(out.readers<=4 && std::isfinite(out.audio.l),"four bounded independent voices");
            if(s==0 && frame>=600 && frame<1200) need(out.readers==0 && out.audio.l==0,
                "counterclockwise produces real silence between genes");
            if(s==3) need(std::fabs(out.audio.l-1.f)<1e-5,"unity stage is contiguous");
            need(c.rate==rate,"Morph never rewrites base rate");
        }
        audioThread=false;
        need(firstCompletion==600,"all finite Gene lifetimes are output-time");
    }
    // Duration doubles with the splice in this mapping; cadence must scale too.
    for(unsigned length : {2400u,4800u}) {
        Grains grains; CoreOutput c{}; c.gene=1564.f/4095.f;c.morph=1100.f/4096.f;c.rate=1;
        const unsigned duration=length/8;
        for(unsigned i=0;i<2400;++i) {
            grains.step(reel,{0,length},c);
            need(grains.onsetCount()==1+i*2/duration,"Gene duration scales launch intervals");
        }
    }
    Grains transition; CoreOutput c{};c.gene=1564.f/4095.f;c.morph=1850.f/4096.f;c.rate=1;
    for(int i=0;i<450;++i)transition.step(reel,region,c);
    unsigned ages[3];double positions[3],gains[3],savedRatios[3];
    for(int i=0;i<3;++i){ages[i]=transition.slotAge(i);positions[i]=transition.slotPosition(i);
        gains[i]=transition.slotPanGain(i);savedRatios[i]=transition.slotRatio(i);}
    const auto count=transition.onsetCount();c.morph=0;
    const auto out=transition.step(reel,region,c);
    need(transition.onsetCount()==count && std::fabs(out.audio.l-1)<1e-5,
        "stage change preserves audio and does not retrigger");
    for(int i=0;i<3;++i)need(transition.slotAge(i)==ages[i]+1 &&
        transition.slotPosition(i)==positions[i]+1 && transition.slotPanGain(i)==gains[i] &&
        transition.slotRatio(i)==savedRatios[i],"stage changes preserve independent voices and latched choices");
    Grains defaultsDsp; c.morph=1;
    for(int i=0;i<1800;++i)defaultsDsp.step(reel,region,c);
    need(defaultsDsp.slotRatio(0)==1 && defaultsDsp.slotRatio(1)==2 &&
         defaultsDsp.slotRatio(2)==1.5 && defaultsDsp.slotRatio(3)==4.0/3.0,
         "DSP uses recovered default voice ratios");
    Grains reverse;reverse.setChordRatios(2,-1.5,4.0/3.0);c.morph=1;
    for(int i=0;i<1800;++i)reverse.step(reel,region,c);
    need(reverse.slotRatio(0)==1 && reverse.slotRatio(1)==2 &&
         reverse.slotRatio(2)==-1.5 && reverse.slotRatio(3)==4.0/3.0,"only configured voice reverses");
    double before[4];for(int i=0;i<4;++i)before[i]=reverse.slotPosition(i);
    reverse.step(reel,region,c); // slot zero launches here; other heads retain position.
    for(int i=1;i<4;++i)need(std::fabs(reverse.slotPosition(i)-
        profile1::wrapPosition(before[i]+reverse.slotRatio(i),region))<1e-8,"signed per-voice source increments");
    double latchedPan[4],latchedRatio[4];
    for(int i=0;i<4;++i){latchedPan[i]=reverse.slotPanGain(i);latchedRatio[i]=reverse.slotRatio(i);}
    c.morph=.7f; // Still stage 21; continuous depth moves, existing choices must not.
    for(int frame=0;frame<100;++frame) {
        reverse.step(reel,region,c);
        for(int i=0;i<4;++i)need(reverse.slotPanGain(i)==latchedPan[i] &&
            reverse.slotRatio(i)==latchedRatio[i],"upper continuous choices remain latched for each Gene");
    }
    Grains stress; stress.setChordRatios(-16,.0625,16);
    audioThread=true;
    for(int frame=0;frame<20000;++frame) {
        c.gene=(frame/83)%2 ? 1.f : .049f;
        c.morph=(frame/37)%2 ? 1.f : 0.f;
        c.rate=(frame/71)%2 ? -32.f : 32.f;
        c.slide=(frame%97)/97.f;
        const auto result=stress.step(reel,region,c,false,false,false,frame%2 ? -960 : 960);
        need(result.readers<=8 && std::isfinite(result.audio.l) && std::isfinite(result.audio.r),
            "extreme durations, reverse ratios, wrap and stage changes remain allocation-free and bounded");
    }
    audioThread=false;
    std::puts("PASS: recovered Morph bins, rational timing, smoothing, thresholds, voices and allocation trap");
}

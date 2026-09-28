#include "ChimeraCore.hpp"
#include "ChimeraGrains.hpp"
#include "../firmware/Morphagene/reconstruction/audited_components.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void need(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
static bool near(float a, float b) { return std::fabs(a-b) <= 2e-6f * std::max(1.f, std::fabs(b)); }
int main() {
    using namespace chimera;
    for(unsigned i=0;i<1024;++i) need(firmware::gene_size_exp[i]==mg204::gene_size_exp[i],"all Gene ROM words preserved");
    for(unsigned i=0;i<34;++i) {
        need(firmware::morph_launch[i]==mg204::morph_launch[i] &&
             firmware::morph_density[i]==mg204::morph_density[i],"exact Morph launch/density words");
    }
    // Fixed offline vectors from the audited bundle, independent of reference
    // arithmetic being optimized in this translation unit.
    const double fixtureRatios[]={2,1.5,firmware::defaultChordThird};
    struct Fixture { float morph; unsigned seed, crossmixBits, state; double ratio; };
    const Fixture fixtures[]={
        {0.f,0,0,2641306770u,1},
        {.8f,0,0x3f1d6f25,4111285669u,2},
        {.8f,1,0x3ea3a7af,290041650u,1},
        {.8f,1234567,0,2175249377u,2},
        {.9f,0,0x3f1d6f25,4111285669u,1.5},
        {.999755859375f,0,0x3f1d6f25,4111285669u,firmware::defaultChordThird}
    };
    for(const auto& fixture:fixtures) {
        profile1::FirmwareRandom random(fixture.seed);
        const auto choice=profile1::chooseOnset(random,0,fixture.morph,fixtureRatios);
        unsigned bits; std::memcpy(&bits,&choice.crossmix,sizeof(bits));
        need(bits==fixture.crossmixBits && random.state==fixture.state && choice.ratio==fixture.ratio,
             "offline Morph vector, including pitch changes on slot zero");
    }
    need(geneSize::quantize(288000,480000)==319999.65625f &&
         geneSize::quantize(192000,480000)==239999.75f,"offline upper-bin examples");
    for (unsigned code=0; code<4096; ++code) {
        const float x=code/4096.f;
        for(int mode=0;mode<3;++mode)
            need(profile1::rateTarget(x,mode)==mg204::rateTarget(x,mode), "all rate targets match ROM transcription");
        for(unsigned span : {3u,480000u,576001u,8352000u}) {
            const auto ref=mg204::ordinaryGene(code,span);
            const auto actual=geneSize::mapAdc(span,code);
            need(actual.wholeSpliceMode==ref.wholeSplice && near(actual.durationSamples,ref.samples), "Gene table and ordered curve match");
            if(!ref.wholeSplice)
                need(near(geneSize::mapAdc(span,code,true).durationSamples, mg204::clockQuantize(ref.samples,float(span))), "all clocked durations match upper-bin rule");
        }
    }
    need(profile1::rateTarget(1,2)==4 && profile1::rateTarget(firmware::forwardUnityKnob,0)==1 &&
         profile1::rateTarget(firmware::forwardUnityKnob,1)==1, "forward range and default unity");
    for(int mode=0;mode<3;++mode) {
        Core core; core.setRateMode(mode); CoreInput input{};
        input.controls.rate=.75f;
        need(core.step(input).rate==mg204::rateTarget(.75f,mode),"Core routes each vsop mode through its firmware target");
    }
    // Check nextafter on both sides of every rate bin in every mode.
    for(int mode=0;mode<3;++mode) for(int i=0;i<=400;++i) {
        float edge=i/(mode==2 ? 328.f : 400.f);
        for(float x : {std::nextafter(edge,0.f),edge,std::nextafter(edge,2.f)}) {
            x=std::max(0.f,std::min(1.f,x));
            need(profile1::rateTarget(x,mode)==mg204::rateTarget(x,mode), "rate bin edge");
        }
    }
    const double ratios[]={2,-1.5,firmware::defaultChordThird};
    for(float m : {0.f,.5f,.6f,.65f,.7f,.8f,.9f,1.f}) for(unsigned seed : {0u,1u,0xffffffffu,1234567u}) {
        unsigned state=seed;
        profile1::FirmwareRandom random(seed);
        for(unsigned n=0;n<1024;++n) {
            auto ref=mg204::chooseMorph(m,state);
            auto actual=profile1::chooseOnset(random,n%4,m,ratios);
            need(actual.crossmix==ref.stereoCrossmix && random.state==state &&
                 actual.ratio==(ref.rateIndex ? ratios[ref.rateIndex-1] : 1), "launch distribution and conditional RNG consumption");
            if(m==.65f) need(actual.ratio==1,"no premature chords below first pitch-choice boundary");
        }
    }
    for(float p : {0.f,.25f,.5f,.75f,1.f}) {
        auto mono=profile1::stereoCrossmix({.75f,.75f},p);
        auto side=profile1::stereoCrossmix({1,0},p);
        need(mono.l==.75f && mono.r==.75f && side.l==1-p && side.r==p,"mono invariance and channel transfer");
    }
    // Exercise the actual voice renderer, not just the crossmix helper.
    Reel reel(4,4);
    for(unsigned i=0;i<1024;++i) need(reel.write(i,{1,0},i),"prepare stereo source");
    Grains grains; CoreOutput c{};c.rate=1;c.gene=1564.f/4095.f;c.morph=1;
    bool crossed=false;
    for(unsigned i=0;i<5000;++i) {
        auto out=grains.step(reel,{0,1024},c);
        crossed=crossed || out.audio.r>.1f;
        need(out.audio.l>=-1e-6f && out.audio.r>=-1e-6f && out.audio.l+out.audio.r<=1.00001f,"stereo crossmix conserves voice sum");
    }
    need(crossed,"upper Morph sends left-only source to right output");
    std::puts("PASS: MG204 control tables, quantizer, launch choices and rendered stereo crossmix");
}

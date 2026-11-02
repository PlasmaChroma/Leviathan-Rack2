#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
// Test harness needs Rack engine construction, which is not a plugin API.
#include <context.hpp>
#include <engine/Engine.hpp>
#undef PRIVATE
#include "../src/plugin.hpp"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// Stack-owned Rack fixture; application teardown is outside this test.
namespace rack { Context::~Context() {} }

Plugin* pluginInstance = nullptr;

bool testDragonKingDebugEnabled = false;
bool isDragonKingDebugEnabled() {
	return testDragonKingDebugEnabled;
}

static bool previewOptionsEnabled = true;
bool isDragonKingPreviewWidgetOptionsEnabled() {
	return previewOptionsEnabled;
}

bool isModuleTeardownLoggingEnabled() {
	return false;
}

void refreshDragonKingDebugEnabled() {
}

ModuleTeardownTimer::ModuleTeardownTimer(const char* moduleName)
	: moduleName(moduleName) {
}

void ModuleTeardownTimer::begin(int moduleId) {
	this->moduleId = moduleId;
}

ModuleTeardownTimer::~ModuleTeardownTimer() {
}

#define SIL_HEADLESS_TEST 1
#include "../src/sil/Sil.cpp"

#include <complex>
#include <limits>

namespace {
constexpr double pi = 3.14159265358979323846;
void require(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}
void setup(Sil& m, float sr) {
    rack::engine::Module::SampleRateChangeEvent e;
    e.sampleRate = sr; e.sampleTime = 1.f / sr; m.onSampleRateChange(e);
}
float sine(int i, float sr, float hz) { return std::sin(2 * pi * hz * i / sr); }
double db(double x) { return 20 * std::log10(std::max(x, 1e-20)); }

void detectorEdges(float sr) {
    for (float hz : {90.f, 315.f, 1450.f, 6000.f, 12000.f}) {
        sil::OnePoleRC filter;
        filter.setCutoff(sil::rcCutoffForHz(hz, sr));
        sil::DetectorBank4 bank;
        bank.setLane(0, sil::rcCutoffForHz(hz, sr), sil::rcCutoffForHz(hz, sr));
        double low = 0, high = 0, band = 0;
        for (int i = 0; i < int(2 * sr); ++i) {
            const float x = sine(i, sr, hz);
            filter.process(x);
            bank.process(sil::DetectorBank4::f4(x), .9f, .99f);
            if (i >= int(sr)) {
                low += double(filter.lowpass()) * filter.lowpass();
                high += double(filter.highpass()) * filter.highpass();
                band += double(bank.lpY[0]) * bank.lpY[0];
            }
        }
        require(std::fabs(db(std::sqrt(2 * low / sr)) + 3.0103) < .005, "Low-pass Hz edge is not -3 dB");
        require(std::fabs(db(std::sqrt(2 * high / sr)) + 3.0103) < .005, "High-pass Hz edge is not -3 dB");
        require(std::fabs(db(std::sqrt(2 * band / sr)) + 6.0206) < .01, "Detector bank Hz edge is wrong");
    }
}

double eqMagnitude(const Sil::Biquad& b, float sr, float hz) {
    const std::complex<double> z = std::polar(1.0, -2 * pi * hz / sr);
    return std::abs((double(b.b0) + double(b.b1)*z + double(b.b2)*z*z)
        / (1.0 + double(b.a1)*z + double(b.a2)*z*z));
}
void eqDesign(float sr) {
    for (auto test : {std::pair<float,float>(315.f,-2.5f), {1450.f,.85f}, {350.f,-2.f}, {6000.f,2.f}}) {
        Sil::Biquad b;
        b.setPeaking(sr, test.first, test.first == 350 ? 7.3f : .72f, test.second);
        require(std::fabs(db(eqMagnitude(b,sr,test.first)) - test.second) < .025, "Peaking EQ center/gain is wrong");
        require(std::fabs(db(eqMagnitude(b,sr,0))) < .025, "Peaking EQ changes DC gain");
        require(std::fabs(db(eqMagnitude(b,sr,sr*.5f))) < .005, "Peaking EQ changes Nyquist gain");
    }
    Sil::Biquad shelf;
    shelf.setHighShelf(sr,1000,.707,.75);
    require(std::fabs(db(eqMagnitude(shelf,sr,0))) < .025, "Air shelf changes DC gain");
    require(std::fabs(db(eqMagnitude(shelf,sr,sr*.5f))-.75) < .005, "Air shelf maximum is wrong");
}

float mudRun(float sr, int kind) {
    Sil m; setup(m,sr); float l=0,r=0;
    for (int i=0;i<int(4*sr);++i) {
        const float broad = sine(i,sr,230)+sine(i,sr,320)+sine(i,sr,450);
        float x = kind >= 10 ? sine(i,sr,float(kind)) : broad;
        if (kind==2) x*=1e-6f;
        if (kind==3) x=.1f*x+3*sine(i,sr,100)+3*sine(i,sr,1500);
        m.processRemoveMud(x,kind==1?-x:x,i%16==0,l,r);
        require(std::isfinite(l)&&std::isfinite(r), "Mud output is nonfinite");
        require(m.removeMud.smoothedCutDb >= -2.501f && m.removeMud.smoothedCutDb <= .001f,"Mud exceeds its cut bound");
    }
    return -m.removeMud.smoothedCutDb;
}
void mudTests(float sr) {
    for (int hz : {80,180,220,260,300,315,360,440,520,1000}) {
        const float cut=mudRun(sr,hz); if(cut>=.16f)std::printf("tone mud hz%d %.3f dB\n",hz,cut);
        require(cut<.16f,"Mud mistakes a single pitched tone for congestion");
    }
    const float broad=mudRun(sr,0), opposite=mudRun(sr,1);
    std::printf("  %.0f Hz: broad mud %.2f dB; opposite-polarity %.2f dB\n",sr,broad,opposite);
    require(broad>.4f,"Distributed low-mid buildup does not trigger cleanup");
    require(std::fabs(broad-opposite)<.02f,"Mud detector loses opposing stereo content");
    require(mudRun(sr,2)<.001f,"Mud reacts to sub-floor material");
    require(mudRun(sr,3)<broad*.5f,"Mud ignores bass/presence reference balance");
}

void airTests(float sr) {
    for (float level : {1.f,.001f}) {
        Sil m;setup(m,sr);float l,r,peak=0;
        for (int i=0;i<int(3*sr);++i) {
            const float trigger=i>=int(.1f*sr)&&i<int(.2f*sr)?level*sine(i,sr,60):0.f;
            m.processImpactAir(trigger,1,1,i%16==0,l,r);
            peak=std::max(peak,m.impactAir.smoothedLiftDb);
            require(std::fabs(l-r)<1e-7f,"Impact Air changes stereo balance");
        }
        require(peak<=.751f,"Air shelf exceeds its gain cap");
        require(level<.01f?peak<.001f:peak>.2f,"Impact Air trigger/floor is wrong");
        require(m.impactAir.smoothedLiftDb<.001f,"Impact Air does not release");
    }
    Sil m;setup(m,sr);float l,r;
    for(int i=0;i<int(4*sr);++i)m.processImpactAir(sine(i,sr,80),1,1,i%16==0,l,r);
    require(m.impactAir.smoothedLiftDb<.02f,"Sustained bass becomes a permanent air lift");
}

float midrangeRun(float sr,int kind) {
    Sil m;setup(m,sr);float l,r;
    for(int i=0;i<int(5*sr);++i) {
        const float body=sine(i,sr,300), core=sine(i,sr,1450), top=sine(i,sr,5000);
        float x=kind==0?body+.03f*core:kind==1?body+core:body+.03f*core+2*top;
        if(kind==3)x*=1e-6f;
        m.processMidrange(x,x,i%16==0,l,r);
        require(std::fabs(l-r)<1e-6f,"Midrange Enhance changes stereo balance");
        require(m.midEnhance.smoothedLiftDb>=0&&m.midEnhance.smoothedLiftDb<=.851f,"Midrange boost exceeds its cap");
    }
    require(std::fabs(m.midEnhance.ledAmount-m.midEnhance.smoothedLiftDb/.85f)<.001f,
        "Midrange feedback is not proportional to applied EQ gain");
    return m.midEnhance.smoothedLiftDb;
}
void midrangeTests(float sr) {
    const float recessed=midrangeRun(sr,0),full=midrangeRun(sr,1),bright=midrangeRun(sr,2);
    std::printf("  midrange recessed %.3f dB, supported %.3f dB, bright %.3f dB\n",recessed,full,bright);
    require(recessed>.08f,"Midrange deficit does not receive a lift");
    require(full<recessed*.5f,"Supported midrange receives the same lift as recessed mids");
    require(bright<recessed*.5f,"Presence guard does not back off bright material");
    require(midrangeRun(sr,3)<.001f,"Midrange Enhance boosts sub-floor material");
}

float glueRun(float sr,float over,bool opposite) {
    Sil m;setup(m,sr);
    m.glueThresholdUpdateDivider.setDivision(std::numeric_limits<int>::max());
    m.glueAdaptiveThresholdDb=-18;
    const float amplitude=5*std::pow(10.f,(-18+over)/20)*std::sqrt(2.f);
    float l,r;
    for(int i=0;i<int(3*sr);++i) {
        float x=amplitude*sine(i,sr,997);
        m.processGlue(x,opposite?-x:x,i%16==0,l,r);
        require(std::fabs(std::fabs(l)-std::fabs(r))<1e-6,"Glue is not stereo linked");
        require(m.glue.gainReductionDb<=3.001f&&m.glue.makeupDb<=2.001f,"Glue exceeds reduction/makeup bounds");
    }
    return m.glue.gainReductionDb;
}
void glueTests(float sr) {
    const float below=glueRun(sr,-2,false),high=glueRun(sr,8,false),anti=glueRun(sr,8,true);
    require(below>.06f&&below<.11f,"Glue soft knee does not start below threshold");
    require(std::fabs(high-8.f/3.f)<.025f,"Glue ratio is wrong above knee");
    require(std::fabs(high-anti)<.001f,"Glue detector cancels opposing stereo");
    Sil a,b;setup(a,sr);setup(b,sr);
    for(int i=0;i<int(sr);++i){float x=sine(i,sr,997);a.pushRollingSample(x,x);b.pushRollingSample(x,-x);}
    require(std::fabs(a.estimateRollingProgramDbFs()-b.estimateRollingProgramDbFs())<.001f,
        "Adaptive threshold's program reference cancels opposing stereo");
}

void stereoTests(float sr) {
    for(int kind=0;kind<4;++kind) {
        Sil m;setup(m,sr);float l,r;double inE=0,outE=0;
        const float hz=kind==0?350.f:kind==1?80.f:kind==2?6000.f:2000.f;
        for(int i=0;i<int(5*sr);++i) {
            float x=sine(i,sr,hz);
            m.processStereo(x,kind>=2?-x:x,i%16==0,l,r);
            require(kind>=2?std::fabs(l+r)<1e-6f:std::fabs(l-r)<1e-6f,"M/S EQ leaks between legs");
            if(i>=int(4*sr)){inE+=x*x;outE+=l*l;}
        }
        require(std::fabs(m.stereoEnhance.ledAmount-.25f*(-m.stereoEnhance.smoothedMidCutDb+m.stereoEnhance.smoothedSideLiftDb))<.001f,
            "Stereo feedback does not reflect the combined EQ controls");
        if(kind==0) require(m.stereoEnhance.smoothedMidCutDb < -1.8f&&db(std::sqrt(outE/inE)) < -1.8,
            "Stereo Enhance does not cut congested 350 Hz Mid");
        if(kind==1) require(m.stereoEnhance.smoothedMidCutDb > -.05f,"Bass drives the 350 Hz focus detector");
        if(kind==2) require(m.stereoEnhance.smoothedSideLiftDb < .05f,"Already-bright 6 kHz Side is boosted");
        if(kind==3) require(m.stereoEnhance.smoothedSideLiftDb > 1.f,"Duller Side never receives air");
    }
}

void saturatorTests(float sr) {
    Sil m;setup(m,sr);float l,r;
    rack::engine::Module::ProcessArgs a;a.sampleRate=sr;a.sampleTime=1/sr;
    for(int i=0;i<int(4*sr);++i) m.processSaturator(0,0,a,l,r);
    require(l==0&&r==0&&m.saturator.makeupDb==0&&m.saturator.ledAmount==0,"Silence requests saturation/makeup");
    for(int i=0;i<int(5*sr);++i){float x=.3f*sine(i,sr,997);m.processSaturator(x,x,a,l,r);}
    require(m.saturator.makeupDb>2.f,"Quiet material never receives bounded makeup");
    // The asymmetric tube contribution must not leave a DC offset on an AC tone.
    double mean=0;
    for(int i=0;i<int(10*sr);++i) {
        float x=4*sine(i,sr,997);m.processSaturator(x,x,a,l,r);
        if(i>=int(9*sr)) mean+=l;
    }
    require(std::fabs(mean/sr)<.002,"Asymmetric saturation adds a DC offset");
    require(m.saturator.makeupDb<=3.251f,"Saturator makeup exceeds its cap");
    require(m.saturator.drive<=1.601f,"Saturator drive exceeds its cap");
    for(int i=0;i<int(10*sr);++i){
        m.limiterTriggerEma=1;m.limiterRecentGrDb=4;
        float x=8*sine(i,sr,997);m.processSaturator(x,x,a,l,r);
        require(std::isfinite(l)&&std::isfinite(r),"Saturator output is nonfinite");
    }
    require(m.saturator.makeupDb<.3f,"Saturator does not back off an overworked limiter");
    for(int i=0;i<int(3*sr);++i)m.processSaturator(0,0,a,l,r);
    require(m.saturator.ledAmount==0,"Saturator feedback remains active on silence");

    // Leaving neutral must not step the level. The shaper has more small-signal
    // gain than unity, so engaging it all at once gave a ~1.8 dB jump.
    Sil fresh;setup(fresh,sr);
    const int period=48;const float hz=sr/float(period);
    float previousPeak=-1.f,worstStepDb=0.f;
    for(int block=0;block<int(3*sr)/period;++block) {
        float peak=0.f;
        for(int j=0;j<period;++j) {
            const int i=block*period+j;
            float x=.3f*sine(i,sr,hz);fresh.processSaturator(x,x,a,l,r);
            peak=std::max(peak,std::fabs(l));
        }
        if(previousPeak>0.f) worstStepDb=std::max(worstStepDb,float(std::fabs(db(peak/previousPeak))));
        previousPeak=peak;
    }
    require(fresh.saturator.makeupDb>1.f,"Saturator transition fixture never engaged");
    if(worstStepDb>=.1f)std::fprintf(stderr,"saturator worst per-period level step %.3f dB\n",worstStepDb);
    require(worstStepDb<.1f,"Saturator engagement steps the output level");
}

// Independent 16x, 129-tap/source-sample windowed-sinc reconstruction. This does
// not reuse Rack's detector FIR or Hermite peak estimate, and includes starts,
// stops, and gain transitions rather than measuring only a stationary tone.
double reconstructedPeak(const std::vector<float>& audio) {
    double taps[16][129];
    for(int phase=0;phase<16;++phase) {
        const double frac=double(phase)/16;double sum=0;
        for(int j=-64;j<=64;++j) {
            const double x=j-frac;
            double w=std::fabs(x)<=64?.5*(1+std::cos(pi*x/64)):0;
            const double v=std::fabs(x)<1e-12?1:std::sin(pi*x)/(pi*x);
            sum+=(taps[phase][j+64]=w*v);
        }
        for(double& x:taps[phase])x/=sum;
    }
    double peak=0;
    for(int i=64;i<int(audio.size())-64;++i)for(int phase=0;phase<16;++phase) {
        double y=0;for(int j=-64;j<=64;++j)y+=audio[i+j]*taps[phase][j+64];
        peak=std::max(peak,std::fabs(y));
    }
    return peak;
}
void limiterTests(float sr) {
    double worst=-100;
    for(int kind=0;kind<14;++kind) {
        Sil m;setup(m,sr);float l,r;
        std::vector<float> audio,audioR,originalL,originalR;
        audio.reserve(8192);audioR.reserve(8192);originalL.reserve(8192);originalR.reserve(8192);
        uint32_t rng=123;
        for(int i=0;i<8192;++i) {
            float x=0;
            if(i>=1024&&i<6144) {
                const float hz=kind<8?sr*(.0625f+.062f*kind):997.f;
                x=10*std::sin(2*pi*hz*i/sr+.47);
                if(kind==8)x=i==2000?30.f:0.f;
                if(kind==9)x=(i%513<64?20.f:0.f)*sine(i,sr,7313);
                if(kind==10){rng=rng*1664525u+1013904223u;x=20*(float(int32_t(rng))/2147483648.f);}
                if(kind==11)x=15*sine(i,sr,997)+10*sine(i,sr,11003);
            }
            float inL=kind==12?.2f*x:x;
            float inR=kind==12?x:kind==13&&i>=1024&&i<6144?10*sine(i,sr,11003):.7f*x;
            originalL.push_back(inL);originalR.push_back(inR);
            m.processLimiter(inL,inR,l,r);
            require(std::fabs(l)<=m.limiterCeiling+1e-5&&std::fabs(r)<=m.limiterCeiling+1e-5,"Limiter exceeds sample ceiling");
            const int source=i-m.limiterLatencySamples;
            if(source>=0) require(std::fabs(l*originalR[source]-r*originalL[source])<.001f,
                "Limiter changes stereo balance");
            audio.push_back(l);
            audioR.push_back(r);
        }
        const double peakDb=db((kind>=12?std::max(reconstructedPeak(audio),reconstructedPeak(audioR))
            :reconstructedPeak(audio))/5);
        worst=std::max(worst,peakDb);
        if(peakDb>-.99)std::fprintf(stderr,"true peak failure sr %.0f fixture %d: %.3f dBFS\n",sr,kind,peakDb);
        require(peakDb<=-.99,"Independent reconstruction exceeds limiter ceiling");
    }
    std::printf("  %.0f Hz: worst independently reconstructed limiter peak %.3f dBFS\n",sr,worst);
    // Attack is a linear ramp completed before the peak, never a one-sample gain step.
    for(float amplitude:{30.f,8.f}) {
        Sil m;setup(m,sr);float l,r;
        float previousGain=1.f,minGain=1.f,worstDrop=0.f,peakAbs=0.f;int peakOut=-1;
        for(int i=0;i<4096;++i) {
            float x=0.f;
            if(amplitude>10.f) x=i==1000?amplitude:0.f;                 // impulse
            else if(i>=1000) x=amplitude*sine(i,sr,997);                 // abrupt loud onset
            const float gain=1.f-m.processLimiter(x,x,l,r);
            worstDrop=std::max(worstDrop,previousGain-gain);
            minGain=std::min(minGain,gain);previousGain=gain;
            if(std::fabs(l)>peakAbs){peakAbs=std::fabs(l);peakOut=i;}
        }
        const float allowed=(1.f-minGain)/float(m.limiterRampSamples)*1.05f+1e-4f;
        if(worstDrop>allowed)std::fprintf(stderr,"limiter sr %.0f amp %.0f: drop %.4f > %.4f\n",sr,amplitude,worstDrop,allowed);
        require(minGain<.9f,"Limiter attack fixture never limited");
        require(worstDrop<=allowed,"Limiter attack steps instead of ramping");
        require(amplitude<10.f||peakOut==1000+m.limiterLatencySamples,"Limiter latency does not match its delay");
    }
}

void spectrumAndMakeupTests() {
    Sil m;
    alignas(16) float fft[Sil::FFT_SIZE] = {};
    for (float sr : {22050.f, 44100.f, 48000.f, 96000.f, 192000.f}) {
        m.updateSpectrumBinMap(sr);
        for (int bin = int(3000.f * Sil::FFT_SIZE / sr) + 1;
                bin < std::min(Sil::FFT_SIZE / 2, int(19500.f * Sil::FFT_SIZE / sr)); ++bin) {
            fft[2*bin] = 1.f;
            float peak=0.f;
            for (const auto& entry : m.specBinMap) peak=std::max(peak,Sil::spectrumBarPower(fft,entry));
            require(peak > .2f, "Spectrum mapping loses a high-frequency bin");
            fft[2*bin]=0.f;
        }
        for (const auto& entry : m.specBinMap) if (!entry.active)
            require(Sil::spectrumBarPower(fft,entry)==0.f,"Above-Nyquist bars must be empty");
        setup(m,sr);
        std::fill(std::begin(m.spec.bufferL),std::end(m.spec.bufferL),3.f);
        std::fill(std::begin(m.spec.bufferR),std::end(m.spec.bufferR),-2.f);
        m.publishSpectrumSnapshot();m.updateSpectrumDisplayFromLatestSnapshot();
        for(int i=0;i<Sil::SPEC_FREQ_BINS;++i)
            require(m.spec.magnitudesL[i]==0.f && m.spec.magnitudesR[i]==0.f,"DC leaks into spectrum");
        for(int i=0;i<Sil::FFT_SIZE;++i)m.spec.bufferL[i]=sine(i,sr,6000);
        m.publishSpectrumSnapshot();m.updateSpectrumDisplayFromLatestSnapshot();
        float reference[Sil::SPEC_FREQ_BINS];std::copy(std::begin(m.spec.magnitudesL),std::end(m.spec.magnitudesL),reference);
        for(int i=0;i<Sil::FFT_SIZE;++i)m.spec.bufferL[i]+=3.f;
        std::fill(std::begin(m.spec.magnitudesL),std::end(m.spec.magnitudesL),0.f);
        m.publishSpectrumSnapshot();m.updateSpectrumDisplayFromLatestSnapshot();
        for(int i=0;i<Sil::SPEC_FREQ_BINS;++i)
            require(std::fabs(m.spec.magnitudesL[i]-reference[i])<1e-7f,"DC changes tone display");
    }
    for (float sr : {44100.f,48000.f,96000.f,192000.f}) {
        setup(m,sr);
        rack::engine::Module::ProcessArgs args={};args.sampleRate=sr;args.sampleTime=1.f/sr;
        float largestBoundaryStep=0.f, largestStep=0.f;
        for(int i=0;i<int(sr*2);++i) {
            const float before=m.saturator.makeupLinear;
            const bool boundary=m.saturator.updateDivider.getClock()+1==m.saturator.updateDivider.getDivision();
            float l,r;m.processSaturator(.5f*sine(i,sr,997),.5f*sine(i,sr,997),args,l,r);
            const float delta=std::fabs(m.saturator.makeupLinear-before);
            largestStep=std::max(largestStep,delta);
            if(boundary)largestBoundaryStep=std::max(largestBoundaryStep,delta);
            require(delta<=std::fabs(m.saturator.makeupStep)+3e-7f,"Makeup jumps at a control boundary");
            require(std::isfinite(l)&&m.saturator.makeupLinear>=.999f,"Makeup ramp invalid");
        }
        require(m.saturator.makeupLinear>1.05f,"Makeup ramp fixture failed to engage");
        std::printf("  %.0f Hz makeup: max gain increment %.8f, boundary %.8f\n",sr,largestStep,largestBoundaryStep);
    }
}

void revisionSafetyTests() {
    Sil m;
    for (float sr : {0.f, -1.f, std::numeric_limits<float>::quiet_NaN(),
            std::numeric_limits<float>::infinity(), std::numeric_limits<float>::max(),
            384000.f, 768000.f, 1024000.f, 1365000.f, 1366000.f, 4096000.f}) {
        const int look = m.limiterLookaheadSamplesForRate(sr);
        const int ramp = m.limiterRampSamplesForRate(sr, look);
        require(look >= sil::TruePeakDetector::kLatencySamples + 2 && ramp >= 1
            && look + ramp <= sil::LimiterPeakWindow::kMaximumDelaySamples, "Limiter capacity invariant");
    }
    for (float sr : {384000.f, 768000.f, 1024000.f}) {
        setup(m, sr);
        require(m.limiterLatencySamples == int(sr * .0015f + .5f), "High-rate full limiter duration");
    }
    setup(m, 48000.f);
    m.params[Sil::BOOST_PARAM].setValue(4.f);
    m.params[Sil::MASTERING_ENABLED_PARAM].setValue(0.f);
    setup(m, 96000.f);
    require(m.masteringMix == 0.f && std::fabs(m.boostGain - std::pow(10.f, .2f)) < .001f,
        "Rate change must preserve current control values");
    m.inputs[Sil::INPUT_L_INPUT].setChannels(1);
    m.inputs[Sil::INPUT_R_INPUT].setChannels(1);
    rack::engine::Module::ProcessArgs args = {}; args.sampleRate=96000.f;args.sampleTime=1.f/96000.f;
    for (int i=0;i<20000;++i) {
        m.inputs[Sil::INPUT_L_INPUT].setVoltage(20*sine(i,96000,315));
        m.inputs[Sil::INPUT_R_INPUT].setVoltage(15*sine(i,96000,500)); m.process(args);
    }
    m.colorScheme=Sil::SCHEME_FIRE;
    {
        rack::Context context;
        rack::engine::Engine engine;
        context.engine = &engine;
        rack::contextSet(&context);
        m.onReset({});
        context.engine = nullptr;
        rack::contextSet(nullptr);
    }
    require(m.colorScheme==Sil::SCHEME_FIRE && m.params[Sil::BOOST_PARAM].getValue()==0.f
        && m.params[Sil::MASTERING_ENABLED_PARAM].getValue()==1.f, "Reset control/theme policy");
    Sil fresh;setup(fresh,96000.f);
    fresh.inputs[Sil::INPUT_L_INPUT].setChannels(1);fresh.inputs[Sil::INPUT_R_INPUT].setChannels(1);
    for (int i=0;i<12000;++i) {
        const float x=i<3000?0.f:2*sine(i,96000,350);
        for (Sil* module : {&m,&fresh}) {
            module->inputs[Sil::INPUT_L_INPUT].setVoltage(x);
            module->inputs[Sil::INPUT_R_INPUT].setVoltage(-.7f*x);module->process(args);
        }
        require(m.outputs[Sil::OUTPUT_L_OUTPUT].getVoltage()==fresh.outputs[Sil::OUTPUT_L_OUTPUT].getVoltage()
            && m.outputs[Sil::OUTPUT_R_OUTPUT].getVoltage()==fresh.outputs[Sil::OUTPUT_R_OUTPUT].getVoltage(),
            "Initialize must match a fresh module including tails and adaptation");
    }
    for (bool bypass : {false,true}) for (int mono : {0,1,2}) {
        Sil guarded, reference;setup(guarded,48000);setup(reference,48000);
        args.sampleRate=48000;args.sampleTime=1.f/48000;
        for (Sil* module : {&guarded,&reference}) {
            module->params[Sil::MASTERING_ENABLED_PARAM].setValue(bypass?0.f:1.f);
            module->inputs[Sil::INPUT_L_INPUT].setChannels(mono==2?0:1);
            module->inputs[Sil::INPUT_R_INPUT].setChannels(mono==1?0:1);
        }
        const float bad[]={std::numeric_limits<float>::quiet_NaN(),
            std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity()};
        for(int i=0;i<8000;++i) {
            for(int ch=0;ch<2;++ch) {
                const bool inject=(i==123+ch || (i>=1000+ch*100 && i<1100+ch*100));
                float x=2*sine(i,48000,ch?777:315);
                guarded.inputs[ch].setVoltage(inject?bad[i%3]:x);
                reference.inputs[ch].setVoltage(inject?0.f:x);
            }
            guarded.process(args);reference.process(args);
            for(int ch=0;ch<2;++ch) require(std::isfinite(guarded.outputs[ch].getVoltage())
                && guarded.outputs[ch].getVoltage()==reference.outputs[ch].getVoltage(),
                "Nonfinite samples must behave exactly like zero, with recovery");
        }
    }
}

void detectorBoundTests() {
    sil::TruePeakDetector bounded, full;
    uint32_t rng=456;
    for(int i=0;i<10000;++i) {
        rng=rng*1664525u+1013904223u;
        const float amplitude=i%2000<1000?.01f:20.f;
        const float left=amplitude*float(int32_t(rng))/2147483648.f;
        const float right=i%97<48?.7f*left:-2.f*left;
        const float gain=i%137<68?1.f:2.f;
        const float limit=4.2f;
        const float exact=full.process(left,right,gain,0.f); // force all reconstruction phases
        const float fast=bounded.process(left,right,gain,limit);
        require(exact*gain<=limit || std::fabs(exact-fast)<1e-5f,
            "Quiet-path bound skips an interpolation peak that needs limiting");
    }
}
}

int main(int argc, char**) {
    if (argc>1) {for(float sr:{44100.f,48000.f,96000.f,192000.f,384000.f,768000.f})limiterTests(sr);return 0;}
    for(float sr:{44100.f,48000.f,96000.f,192000.f}) {detectorEdges(sr);eqDesign(sr);}
    spectrumAndMakeupTests();
    revisionSafetyTests();
    detectorBoundTests();
    for(float sr:{48000.f,96000.f}) {
        mudTests(sr);airTests(sr);midrangeTests(sr);glueTests(sr);stereoTests(sr);saturatorTests(sr);
    }
    for(float sr:{44100.f,48000.f,96000.f,192000.f,384000.f,768000.f})limiterTests(sr);
    std::puts("Sil isolated stage behavior tests passed");
}

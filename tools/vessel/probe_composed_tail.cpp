#include "ComposedTailHost.hpp"
#include "vessel/DualBowlAdapter.hpp"
#include "vessel/SeedProfiles.hpp"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <new>
#include <stdexcept>
#include <vector>
using namespace vessel;
bool countAllocations=false;
std::uint64_t allocations=0;
void* operator new(std::size_t n) {if(countAllocations)++allocations;if(auto p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n) {return ::operator new(n);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
namespace {
volatile double sink=0;
std::string outputDirectory;
void require(bool okay,const char* message) {if(!okay)throw std::runtime_error(message);}
struct Difference {
    double signal=0,error=0,peak=0,energy=0,state=0;
    std::uint64_t count=0;
    void add(const HostFrame& a,const HostFrame& b,const VesselEngine& ae,const VesselEngine& be) {
        require(!a.fault&&!b.fault,"processing fault");
        for(auto pair:{std::make_pair(a.audio.left,b.audio.left),std::make_pair(a.audio.right,b.audio.right)}) {
            require(std::isfinite(pair.second),"nonfinite output");
            signal+=pair.first*pair.first;error+=(pair.second-pair.first)*(pair.second-pair.first);
            peak=std::max(peak,std::abs(pair.second-pair.first));++count;
        }
        energy=std::max(energy,std::abs(a.bowlEnergy-b.bowlEnergy));
        for(std::size_t j=0;j<ae.bowl().size();++j) {
            state=std::max(state,std::abs(ae.bowl().state(j).x-be.bowl().state(j).x));
            state=std::max(state,std::abs(ae.bowl().state(j).y-be.bowl().state(j).y));
        }
    }
    double relative() const {return std::sqrt(error/std::max(signal,1e-300));}
    void gate() const {
        require(relative()<1e-5 || std::sqrt(error/count)<1e-12,"audio RMS equivalence gate");
        require(peak<1e-9,"peak audio equivalence gate");
        require(state<1e-8 && energy<1e-8,"state/energy equivalence gate");
    }
};
template<class H> void seed(H& h,bool high=false) {
    auto& b=h.probeEngine().probeBank();
    for(std::size_t j=0;j<b.size();++j)
        require(b.setState(j,(high?.1:.003)*std::sin(double(j+1)),(high?.1:.003)*std::cos(double(2*j+1))),"seed state");
}
struct Pair {
    ComposedTailHost left,right;
    bool configure(const BowlDescriptor& b,const MalletDescriptor& m,const EngineSettings& center,
                   double delta,double rate,ProcessingQuality q) {
        auto l=center,r=center;l.frequency-=.5*delta;r.frequency+=.5*delta;
        return left.configure(b,m,l,rate,q)&&right.configure(b,m,r,rate,q);
    }
    DualBowlFrame process(const HostControls& c) {
        const auto l=left.process(c),r=right.process(c);DualBowlFrame f;
        f.audio.left=l.audio.left;f.audio.right=r.audio.right;
        f.leftEnergy=l.bowlEnergy;f.rightEnergy=r.bowlEnergy;
        f.bowlEnergy=.5*(l.bowlEnergy+r.bowlEnergy);f.fault=l.fault||r.fault;return f;
    }
    void updateHighEnergyDamping() {left.updateHighEnergyDamping();right.updateHighEnergyDamping();}
};
void damping(HostRateAdapter& h) {h.probeEngine().updateHighEnergyDamping();}
void damping(ComposedTailHost& h) {h.updateHighEnergyDamping();}
void damping(DualBowlAdapter& h) {h.updateHighEnergyDamping();}
void damping(Pair& h) {h.updateHighEnergyDamping();}
void row(const char* group,int bowl,double rate,int quality,double pitch,const Difference& d,
         const ComposedTailHost& h,double refUs=0,double candidateUs=0,double maxExit=0,double configUs=0) {
    d.gate();require(allocations==0,"audio allocation");
    std::cout<<group<<','<<bowl<<','<<rate<<','<<quality<<','<<pitch<<','<<h.factor()<<','<<d.relative()<<','
        <<d.peak<<','<<d.energy<<','<<d.state<<','<<h.composedFrames()<<','<<h.restorations()<<','
        <<refUs<<','<<candidateUs<<','<<maxExit<<','<<configUs<<','<<allocations<<std::endl;
}
void fixed(int bowl,double rate,ProcessingQuality quality,double pitch,double seconds,bool near=false) {
    HostRateAdapter reference;ComposedTailHost candidate;EngineSettings s;s.frequency=pitch;
    auto descriptor=seedBowls[bowl];
    if(near)descriptor.pairs[descriptor.pairCount-1].ratio=.399*rate*HostRateAdapter::factorForRate(rate,quality)/pitch;
    require(reference.configure(descriptor,seedMallets[1],s,rate,quality),"reference configure");
    auto begin=std::chrono::steady_clock::now();
    require(candidate.configure(descriptor,seedMallets[1],s,rate,quality),"candidate configure");
    const double configUs=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count()*1e6;
    seed(reference);seed(candidate);Difference d;HostControls c;
    double previousEnergy=candidate.engine().bowl().energy();
    for(int i=0;i<int(seconds*rate);++i) {
        countAllocations=true;
        const auto a=reference.process(c),b=candidate.process(c);
        countAllocations=false;d.add(a,b,reference.engine(),candidate.engine());
        require(b.bowlEnergy<=previousEnergy+1e-14,"unforced energy increased");
        previousEnergy=b.bowlEnergy;
    }
    row(near?"near_guard":"fixed",bowl,rate,int(quality),pitch,d,candidate,0,0,0,configUs);
}
void transitions(int bowl,double rate,ProcessingQuality quality) {
    HostRateAdapter reference;ComposedTailHost candidate;EngineSettings s;auto descriptor=seedBowls[bowl];
    auto configure=[&](double r,ProcessingQuality q) {
        countAllocations=true;
        const bool a=reference.configure(descriptor,seedMallets[1],s,r,q);
        const bool b=candidate.configure(descriptor,seedMallets[1],s,r,q);
        countAllocations=false;require(a&&b,"transition configure");
    };
    configure(rate,quality);HostControls c;Difference d;double maxExit=0;
    for(int i=0;i<int(11*rate);++i) {
        const double t=i/rate;
        c.strikeEvent=i==0 || i==int(3*rate) || i==int(9*rate)+1;
        c.rotate=t>=1 && t<2;c.speed=t<1.5?.4:-.4;c.pressure=t<1.5?2.5:15;
        if(i==int(4*rate)) {s.frequency=880;configure(rate,quality);}
        if(i==int(5*rate)) {s.observerSeparation=0;configure(rate,quality);}
        if(i==int(6*rate)) {descriptor=seedBowls[1-bowl];configure(rate,quality);}
        if(i==int(7*rate))configure(rate==48000?44100:48000,quality);
        if(i==int(8*rate)) {s.frequency=261.625565;configure(rate,ProcessingQuality::Economy);}
        if(i==int(9*rate)) {reference.reset();candidate.reset();}
        if(i==int(10*rate)) {reference.reset();candidate.reset();seed(reference,true);seed(candidate,true);}
        if(i%48==0) {reference.probeEngine().updateHighEnergyDamping();candidate.updateHighEnergyDamping();}
        const bool exit=candidate.running()&&(c.strikeEvent||c.rotate);
        countAllocations=true;
        const auto a=reference.process(c);
        const auto begin=std::chrono::steady_clock::now();const auto b=candidate.process(c);
        const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count()*1e6;
        countAllocations=false;
        if(exit)maxExit=std::max(maxExit,elapsed);
        d.add(a,b,reference.engine(),candidate.engine());
    }
    // Rejected tuning must not terminate composition or alter future samples.
    auto before=candidate;auto bad=s;bad.frequency=-1;
    require(!candidate.configure(descriptor,seedMallets[1],bad,rate,quality),"invalid accepted");
    for(int i=0;i<256;++i) {
        const auto a=before.process(c),b=candidate.process(c);
        require(a.audio.left==b.audio.left&&a.audio.right==b.audio.right,"rejected configuration changed stream");
    }
    row("transitions",bowl,rate,int(quality),s.frequency,d,candidate,0,0,maxExit);
}
template<class H> double timed(H h) {
    HostControls c;double sum=0;
    auto begin=std::chrono::steady_clock::now();
    for(int i=0;i<2*48000;++i) {
        if(i%48==0)damping(h);
        const auto f=h.process(c);require(!f.fault,"timing fault");sum+=f.audio.left+f.audio.right;
    }
    sink=sum;return std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count()*1e6/(2*48000);
}
void benchmark(int bowl,ProcessingQuality quality) {
    HostRateAdapter reference;ComposedTailHost candidate;EngineSettings s;
    require(reference.configure(seedBowls[bowl],seedMallets[1],s,48000,quality)
        &&candidate.configure(seedBowls[bowl],seedMallets[1],s,48000,quality),"benchmark setup");
    seed(reference);seed(candidate);Difference d;HostControls c;
    for(int i=0;i<512;++i)d.add(reference.process(c),candidate.process(c),reference.engine(),candidate.engine());
    double a=0,b=0;
    for(int repeat=0;repeat<5;++repeat) {
        if(repeat%2) {b+=timed(candidate);a+=timed(reference);}
        else {a+=timed(reference);b+=timed(candidate);}
    }
    row("timing",bowl,48000,int(quality),s.frequency,d,candidate,a/5,b/5);
}
void paired(int bowl,ProcessingQuality quality,double delta) {
    DualBowlAdapter reference(false);reference.setComposedTailEnabled(false);Pair candidate;EngineSettings s;Difference d;HostControls c;
    require(reference.configure(seedBowls[bowl],seedMallets[1],s,delta,48000,quality)
        &&candidate.configure(seedBowls[bowl],seedMallets[1],s,delta,48000,quality),"pair setup");
    double maxExit=0;
    for(int i=0;i<8*48000;++i) {
        c.strikeEvent=i==0||i==6*48000;c.rotate=i>=48000&&i<3*48000;
        if(i%48==0) {damping(reference);damping(candidate);}
        const bool exit=candidate.left.running()&&(c.strikeEvent||c.rotate);
        countAllocations=true;
        const auto a=reference.process(c);const auto begin=std::chrono::steady_clock::now();
        const auto b=candidate.process(c);
        const double us=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count()*1e6;
        countAllocations=false;if(exit)maxExit=std::max(maxExit,us);
        d.add(a,b,reference.engine(),candidate.left.engine());
        HostFrame ar,br;ar.bowlEnergy=a.rightEnergy;br.bowlEnergy=b.rightEnergy;
        d.add(ar,br,reference.rightEngine(),candidate.right.engine());
    }
    double a=0,b=0;
    for(int r=0;r<5;++r) {
        if(r%2) {b+=timed(candidate);a+=timed(reference);}
        else {a+=timed(reference);b+=timed(candidate);}
    }
    row("paired",bowl,48000,int(quality),delta,d,candidate.left,a/5,b/5,maxExit);
}
void handoffStress(int bowl,double rate,ProcessingQuality quality) {
    HostRateAdapter reference;ComposedTailHost candidate;EngineSettings settings;
    auto descriptor=seedBowls[bowl];HostControls controls;Difference difference;
    require(reference.configure(descriptor,seedMallets[1],settings,rate,quality)
        &&candidate.configure(descriptor,seedMallets[1],settings,rate,quality),"handoff stress setup");
    seed(reference);seed(candidate);
    const unsigned duration=unsigned(128*(candidate.factor()-1)+1+candidate.factor()-1)/candidate.factor();
    for(int cycle=0;cycle<12;++cycle) {
        controls={};
        // Low-energy seeds ensure actual composition before each interruption.
        if(cycle) {reference.reset();candidate.reset();seed(reference);seed(candidate);}
        for(int frame=0;frame<512;++frame)
            difference.add(reference.process(controls),candidate.process(controls),reference.engine(),candidate.engine());
        require(candidate.running(),"stress did not enter composed tail");
        for(unsigned frame=0;frame<duration+256;++frame) {
            controls.strikeEvent=frame==0||frame==1||frame==duration/2||frame==duration-1||frame==duration;
            controls.rotate=(cycle%2) && frame>=2 && frame<duration+8;
            if(frame==2) {
                require(candidate.handingOff(),"missing handoff");
                if(cycle%3==0)settings.frequency=cycle%2?880:261.625565;
                if(cycle%3==1)settings.observerSeparation=cycle%2?0:pi/2;
                if(cycle%3==2)descriptor=seedBowls[1-bowl];
                countAllocations=true;
                const bool a=reference.configure(descriptor,seedMallets[1],settings,rate,quality);
                const bool b=candidate.configure(descriptor,seedMallets[1],settings,rate,quality);
                countAllocations=false;require(a&&b,"mid-handoff configure");
                auto unchanged=candidate;auto bad=settings;bad.frequency=-1;
                require(!candidate.configure(descriptor,seedMallets[1],bad,rate,quality),"mid-handoff invalid accepted");
                const auto x=unchanged.process(controls),y=candidate.process(controls);
                require(x.audio.left==y.audio.left&&x.audio.right==y.audio.right,"mid-handoff rejection changed stream");
                // Advance the reference by that same one frame.
                difference.add(reference.process(controls),y,reference.engine(),candidate.engine());
            }
            if(frame==4 && cycle>=9) {
                // A rate/factor change deliberately discards histories in the
                // production adapter; the correction must be discarded too.
                const double alternate=rate==48000?44100:48000;
                require(reference.configure(descriptor,seedMallets[1],settings,alternate,quality)
                    &&candidate.configure(descriptor,seedMallets[1],settings,alternate,quality),"handoff rate change");
                require(!candidate.handingOff(),"old-rate correction retained");
            }
            countAllocations=true;
            const auto a=reference.process(controls),b=candidate.process(controls);countAllocations=false;
            difference.add(a,b,reference.engine(),candidate.engine());
        }
        require(reference.configure(descriptor,seedMallets[1],settings,rate,quality)
            &&candidate.configure(descriptor,seedMallets[1],settings,rate,quality),"stress rate restore");
    }
    row("handoff_stress",bowl,rate,int(quality),settings.frequency,difference,candidate);
}
double quantile(std::vector<double> values,double fraction) {
    std::sort(values.begin(),values.end());return values[std::size_t((values.size()-1)*fraction)];
}
void reentryTiming() {
    if(outputDirectory.empty())return;
    std::ofstream report(outputDirectory+"/reentry_timing.csv");
    report<<std::setprecision(17)<<"quality,instances,event,phase,variant,samples,median_us,p99_us,max_us\n";
    for(auto quality:{ProcessingQuality::Balanced,ProcessingQuality::Reference})
    for(unsigned instances:{1u,8u,32u})for(bool rub:{false,true}) {
        HostRateAdapter referenceSeed;ComposedTailHost candidateSeed;EngineSettings settings;
        require(referenceSeed.configure(seedBowls[0],seedMallets[1],settings,48000,quality)
            &&candidateSeed.configure(seedBowls[0],seedMallets[1],settings,48000,quality),"reentry setup");
        seed(referenceSeed);seed(candidateSeed);HostControls empty;
        for(int i=0;i<512;++i) {referenceSeed.process(empty);candidateSeed.process(empty);}
        require(candidateSeed.running(),"reentry seed not composed");
        std::vector<HostRateAdapter> references(instances);
        std::vector<ComposedTailHost> candidates(instances);
        std::vector<double> firstRef,firstCandidate,setup,streamRef,streamCandidate;
        for(int repeat=0;repeat<200;++repeat) {
            std::fill(references.begin(),references.end(),referenceSeed);
            std::fill(candidates.begin(),candidates.end(),candidateSeed);
            HostControls controls;controls.strikeEvent=!rub;controls.rotate=rub;
            // Separately time the new bounded history handoff setup, excluding
            // contact processing. The full onset measurement below includes it.
            auto setupCopy=candidates;
            auto start=std::chrono::steady_clock::now();
            for(auto& h:setupCopy)h.probeBeginHandoff();
            setup.push_back(std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()*1e6);
            countAllocations=true;
            auto runReference=[&]() {double total=0;const auto begin=std::chrono::steady_clock::now();
                for(auto& h:references) {const auto f=h.process(controls);require(!f.fault,"reentry reference fault");total+=f.audio.left;}
                const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count()*1e6;sink=total;return elapsed;};
            auto runCandidate=[&]() {double total=0;const auto begin=std::chrono::steady_clock::now();
                for(auto& h:candidates) {const auto f=h.process(controls);require(!f.fault,"reentry candidate fault");total+=f.audio.left;}
                const double elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-begin).count()*1e6;sink=total;return elapsed;};
            double a,b;
            if(repeat%2) {b=runCandidate();a=runReference();}else {a=runReference();b=runCandidate();}
            countAllocations=false;firstRef.push_back(a);firstCandidate.push_back(b);
            controls.strikeEvent=false;
            for(unsigned frame=1;frame<128;++frame) {
                const bool handoff=candidates.front().handingOff();
                countAllocations=true;
                if(repeat%2) {b=runCandidate();a=runReference();}else {a=runReference();b=runCandidate();}
                countAllocations=false;
                if(handoff) {streamRef.push_back(a);streamCandidate.push_back(b);}
            }
        }
        auto emit=[&](const char* phase,const char* variant,const std::vector<double>& samples) {
            report<<int(quality)<<','<<instances<<','<<(rub?"rub":"strike")<<','<<phase<<','<<variant<<','<<samples.size()<<','
                <<quantile(samples,.5)<<','<<quantile(samples,.99)<<','<<*std::max_element(samples.begin(),samples.end())<<'\n';
        };
        emit("setup_only","candidate",setup);emit("onset","reference",firstRef);emit("onset","candidate",firstCandidate);
        emit("stream","reference",streamRef);emit("stream","candidate",streamCandidate);
    }
    require(allocations==0,"reentry allocation");require(bool(report),"reentry timing write");
}
void writeAudio(const std::string& path,const std::vector<StereoSample>& samples) {
    static_assert(sizeof(StereoSample)==2*sizeof(double),"packed stereo");
    std::ofstream file(path,std::ios::binary);
    file.write(reinterpret_cast<const char*>(samples.data()),samples.size()*sizeof(StereoSample));
    require(bool(file),"audio write");
}
void audition(int bowl,ProcessingQuality quality) {
    if(outputDirectory.empty())return;
    HostRateAdapter reference;ComposedTailHost candidate;EngineSettings s;
    require(reference.configure(seedBowls[bowl],seedMallets[1],s,48000,quality)
        &&candidate.configure(seedBowls[bowl],seedMallets[1],s,48000,quality),"audition setup");
    std::vector<StereoSample> a(12*48000),b(a.size());Difference d;HostControls c;
    for(int i=0;i<int(a.size());++i) {
        c.strikeEvent=i==0;c.rotate=i>=48000&&i<3*48000;
        if(i%48==0) {damping(reference);damping(candidate);}
        countAllocations=true;const auto x=reference.process(c),y=candidate.process(c);countAllocations=false;
        a[i]=x.audio;b[i]=y.audio;d.add(x,y,reference.engine(),candidate.engine());
    }
    const auto name="bowl"+std::to_string(bowl)+"_quality"+std::to_string(int(quality));
    writeAudio(outputDirectory+"/"+name+"_reference.f64",a);
    writeAudio(outputDirectory+"/"+name+"_candidate.f64",b);
    row("audition",bowl,48000,int(quality),s.frequency,d,candidate);
}
}
int main(int argc,char** argv) {
    try {
        bool quick=false;
        for(int i=1;i<argc;++i) {
            if(std::string(argv[i])=="--quick")quick=true;
            else if(std::string(argv[i])=="--output"&&i+1<argc)outputDirectory=argv[++i];
            else throw std::runtime_error("invalid arguments");
        }
        std::cout<<std::setprecision(17)<<"group,bowl,host_hz,quality,pitch,factor,relative_error,peak_error,energy_error,state_error,composed_frames,handoffs,reference_us,candidate_us,max_contact_exit_us,configuration_us,allocations\n";
        for(int bowl:{0,1})for(double rate:{32000.,44100.,48000.,88200.,96000.,176400.,192000.})
        for(auto q:{ProcessingQuality::Economy,ProcessingQuality::Balanced,ProcessingQuality::Reference})
        for(double pitch:{20.,261.625565,880.,2000.})fixed(bowl,rate,q,pitch,quick?.03:.25);
        for(int bowl:{0,1})for(double rate:{32000.,48000.})
        for(auto q:{ProcessingQuality::Balanced,ProcessingQuality::Reference}) {
            fixed(bowl,rate,q,261.625565,quick?.1:60);
            fixed(bowl,rate,q,261.625565,quick?.1:2,true);
            transitions(bowl,rate,q);
        }
        for(int bowl:{0,1})for(double rate:{32000.,48000.})
        for(auto q:{ProcessingQuality::Balanced,ProcessingQuality::Reference})handoffStress(bowl,rate,q);
        reentryTiming();
        for(int bowl:{0,1})for(auto q:{ProcessingQuality::Economy,ProcessingQuality::Balanced,ProcessingQuality::Reference})benchmark(bowl,q);
        for(int bowl:{0,1})for(auto q:{ProcessingQuality::Balanced,ProcessingQuality::Reference}) {
            paired(bowl,q,33);audition(bowl,q);
        }
    } catch(const std::exception& e) {countAllocations=false;std::cerr<<e.what()<<'\n';return 1;}
}

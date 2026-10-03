#include "Tiamat/TiamatBufferEngine.hpp"
#include "Tiamat/TiamatMacro.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace tiamat {
struct BufferTestAccess {
    static BufferChannel& ch(Buffer& b, unsigned c) { return b.channels_[c]; }
    static TransitionState& transition(Buffer& b) { return b.transition_; }
    static float* plane(Buffer& b, unsigned c) { return b.memory_.get() + c*b.capacity_; }
    static void process(Buffer& b, const float* x, float* y, unsigned n, const BufferControls& c, Random& r) { b.processFrames(x,y,n,c,r); }
    static void observe(Buffer& b, void (*fn)(const Buffer&, unsigned, void*), void* data) { b.observer_=fn; b.observerData_=data; }
};
struct RandomTestAccess {
    static void inject(Random& r, const std::uint32_t* sequence, unsigned n) { r.sequence_=sequence; r.sequenceSize_=n; r.sequenceIndex_=0; }
    static unsigned draws(const Random& r) { return r.sequenceIndex_; }
};
}
using namespace tiamat;
static void tag(std::istream& in, const char* expected) { std::string got; in >> got; assert(got==expected); }
static void state(std::istream& in, const Buffer& b, unsigned c) {
    tag(in,"STATE");
    const auto& ch=b.channel(c);
    const double actual[] = {double(ch.writer.captureFrames), double(ch.reader.sliceFrames), double(ch.reader.sliceStart),
        double(ch.reader.sliceStart+ch.reader.sliceFrames), double(ch.audibleFrames), double(ch.candidate),
        double(ch.writer.writeBank), double(ch.reader.anchor), double(ch.subdivisions), double(ch.reader.position),
        double(ch.writer.writePosition), double(ch.reader.rate), double(b.transition().freezeActive),
        double(b.transition().freezeRequested), double(ch.resizePending), double(ch.macro.rate),
        double(ch.macro.extraExponent), double(ch.macro.silence), double(ch.macro.randomPosition)};
    unsigned field=0;
    for (double value:actual) {
        double expected; in >> expected; assert(in);
        if (value != expected) {
            std::cerr << "state ch=" << c << " field=" << field << " got=" << value << " expected=" << expected << '\n'; std::abort();
        }
        ++field;
    }
}
static void macro(std::istream& in) {
    std::string which; float amount, initial; unsigned channel, count, draws; bool shared;
    in >> which >> amount >> channel >> shared >> initial >> count;
    std::vector<std::uint32_t> sequence(count);
    for (auto& x:sequence) in >> x;
    in >> draws;
    std::array<float,5> expected; for(auto& x:expected) in >> x;
    MacroState left, right;
    left.rate=right.rate=0; left.slew=right.slew=0;
    left.extraExponent=7; left.silence=.5f; left.randomPosition=.8f;
    auto& target=channel ? right : left; target.slew=initial;
    Random rng; RandomTestAccess::inject(rng,sequence.data(),count);
    if(which=="bend") chooseBend(target,amount,rng);
    else chooseBreak(target,left,channel && shared,amount,rng);
    const std::array<float,5> actual {{target.rate,target.slew,float(target.extraExponent),target.silence,target.randomPosition}};
    assert(actual==expected && RandomTestAccess::draws(rng)==draws);
}
static void partition(std::istream& in) {
    unsigned n,draws; std::string scenario; bool shared; int first; double maxDifference;
    in >> n >> scenario >> shared >> draws >> first >> maxDifference;
    Buffer b(32768,coefficientRate,1000); Random rng;
    const std::uint32_t sequence[]={5,210,400,99,254,33,180,601,75,0,160};
    RandomTestAccess::inject(rng,sequence,11);
    BufferControls c; c.frequency=coefficientRate/(scenario=="time" ? 2000.f : 1000.f);
    c.window=0; c.freezeRequested=true; c.unique=!shared;
    if(scenario=="macro") { c.macroBend=.9f; c.macroBreak=.8f; }
    BufferTestAccess::transition(b).freezeActive=true;
    for(unsigned ch=0; ch<2; ++ch) {
        BufferTestAccess::ch(b,ch).reader.rate=0;
        if(scenario=="macro") BufferTestAccess::ch(b,ch).writer.writePosition=980;
        for(unsigned i=0;i<32768;++i) BufferTestAccess::plane(b,ch)[i]=float(std::sin(double(i)*.071));
    }
    std::vector<float> x(n*2,0), y(n*2);
    int actualFirst=-1; double actualMax=0;
    for(unsigned block=0;block<1920/n;++block) {
        BufferTestAccess::process(b,x.data(),y.data(),n,c,rng);
        for(unsigned i=0;i<n;++i) {
            const double d=std::abs(double(y[i*2])-double(y[i*2+1]));
            if(d && actualFirst<0) actualFirst=int(block*n+i);
            actualMax=std::max(actualMax,d);
        }
    }
    if(actualFirst!=first || actualMax!=maxDifference || RandomTestAccess::draws(rng)!=draws) {
        std::cerr << "partition " << scenario << " n=" << n << " shared=" << shared << " first=" << actualFirst << "/" << first
            << " max=" << actualMax << "/" << maxDifference << " draws=" << RandomTestAccess::draws(rng) << "/" << draws << '\n'; std::abort();
    }
    for(unsigned ch=0;ch<2;++ch) state(in,b,ch);
}
struct AcceptanceCheck { std::vector<std::pair<unsigned,unsigned>> actual; std::array<bool,2> pending {{true,true}}; };
static void freezeCase(std::istream& in) {
    unsigned n,guard,count; float previous,value; in >> n >> guard >> previous >> value >> count;
    std::vector<std::pair<unsigned,unsigned>> expected(count);
    for(auto& e:expected) { tag(in,"ACCEPT"); in >> e.first >> e.second; }
    Buffer b(32768,coefficientRate,64); Random rng;
    BufferControls c; c.frequency=coefficientRate/64.f; c.window=0; c.freezeRequested=true; c.unique=false;
    auto& transition=BufferTestAccess::transition(b); transition.pendingAcceptance={{true,true}}; transition.guard=guard;
    for(unsigned ch=0;ch<2;++ch) {
        auto& s=BufferTestAccess::ch(b,ch); s.reader.rate=0; s.reader.previousSample=s.priorSample=previous;
        std::fill(BufferTestAccess::plane(b,ch),BufferTestAccess::plane(b,ch)+32768,value);
    }
    AcceptanceCheck check;
    BufferTestAccess::observe(b,[](const Buffer& engine,unsigned ch,void* ptr) {
        auto& check=*static_cast<AcceptanceCheck*>(ptr);
        if(check.pending[ch] && !engine.transition().pendingAcceptance[ch]) {
            check.actual.emplace_back(ch,engine.channel(ch).sampleCounter-1); check.pending[ch]=false;
        }
    },&check);
    std::vector<float> x(n*2,value),y(n*2);
    for(unsigned i=0;i<1920/n;++i) BufferTestAccess::process(b,x.data(),y.data(),n,c,rng);
    assert(check.actual==expected && b.transition().guard==0);
    for(unsigned ch=0;ch<2;++ch) state(in,b,ch);
}
static void fixtures() {
    std::ifstream file("tests/fixtures/tiamat/events_v1.txt"); assert(file);
    std::string line; std::ostringstream clean;
    while(std::getline(file,line)) if(!line.empty() && line[0]!='#') clean << line << '\n';
    std::istringstream in(clean.str()); std::string kind; unsigned macros=0,partitions=0,freezes=0;
    while(in>>kind) {
        if(kind=="MACRO") { macro(in); ++macros; }
        else if(kind=="PARTITION") { partition(in); ++partitions; }
        else if(kind=="FREEZE") { freezeCase(in); ++freezes; }
        else assert(false);
    }
    assert(macros==68 && partitions==20 && freezes==20);
    std::cout << "Exact events: 68 Macro helpers, 20 block partitions, 20 Freeze acceptance cases\n";
}
static void clockTests() {
    Clock clock;
    const float twoHz=float(std::log(32.)/7.15461540222168);
    clock.configure(twoHz,ClockSource::Internal);
    std::uint64_t count=0;
    for(unsigned i=1;i<=480000;++i) count+=clock.advance(double(i)/48000).boundaries;
    assert(count==20);
    assert(std::abs(clock.requestedPeriod()-.5)<1e-6);
    clock.resetPhaseAt(10);
    assert(clock.advance(10.25).boundaries==0);
    assert(clock.advance(10.500001).boundaries==1);
    for(unsigned index=0;index<9;++index) {
        Clock ext; const float time=(float(index)+.25f)/8.25f;
        ext.configure(time,ClockSource::External);
        const float ratio=clockRatios[index];
        unsigned edges=0; std::uint64_t total=0;
        for(unsigned frame=0;frame<=16*24000;++frame) {
            const bool edge=frame%24000==0;
            edges+=edge;
            total+=ext.advance(double(frame)/48000,edge).boundaries;
        }
        const auto expected=ratio>=1 ? std::uint64_t(16*ratio+1) : std::uint64_t(edges/unsigned(1/ratio));
        if(total!=expected) { std::cerr << "ratio " << ratio << " total=" << total << " expected=" << expected << '\n'; std::abort(); }
        assert(ext.estimatedInputPeriod()==.5);
    }
    Clock median; median.configure(.5f,ClockSource::External); median.advance(0,true);
    double t=0;
    const double intervals[]={.5,1,.501,.502,.503}, expected[]={.5,.5,.501,.502,.502};
    for(unsigned i=0;i<5;++i) { t+=intervals[i]; median.advance(t,true); assert(std::abs(median.estimatedInputPeriod()-expected[i])<1e-12); }
    const double prior=median.estimatedInputPeriod();
    median.advance(t+.000001,true); assert(median.estimatedInputPeriod()==prior);
    median.advance(std::numeric_limits<double>::quiet_NaN(),true); assert(median.estimatedInputPeriod()==prior);
    auto lost=median.advance(t+2.1); assert(lost.lost && lost.boundaries>0);
    assert(!median.advance(t+2.2,true).lost);
    Clock divided; divided.configure(0,ClockSource::External); divided.advance(0,true);
    divided.resetPhase(); assert(divided.advance(.5,true).boundaries==1);
    assert(divided.advance(.5,true).boundaries==0);
    Clock narrow; narrow.configure(.5f,ClockSource::External);
    std::uint64_t narrowCount=0;
    for(unsigned i=0;i<96;++i) narrowCount+=narrow.advance(double(i)/48000,true).boundaries;
    assert(narrowCount==96); // exact one-core-frame intervals survive subtraction rounding
    Clock divideNarrow; divideNarrow.configure(.4f,ClockSource::External); // /2
    std::uint64_t dividedCount=0;
    for(unsigned i=0;i<96;++i) dividedCount+=divideNarrow.advance(double(i)/48000,true).boundaries;
    assert(dividedCount==48); // edge counting precedes block request coalescing
    Clock capacity; capacity.configure(0,ClockSource::External); capacity.advance(0,true);
    capacity.advance(3,true); auto limited=capacity.advance(6,true);
    assert(limited.capacityLimited && std::abs(capacity.requestedPeriod()-48)<1e-9);
    assert(limited.bufferFrequency==coefficientRate/float(maxCaptureFrames));
    // Huge scheduling jump is arithmetic, never an unbounded catch-up loop.
    Clock backlog; assert(backlog.advance(1e12).boundaries>1000000000);
    std::cout << "Clock: real-time cadence, all nine ratios, median/outliers, reset, loss, capacity and bounded backlog passed\n";
}
static ControlEvent event(EventKind kind,Action action,bool high=true,bool rising=false,unsigned frame=0) {
    ControlEvent e; e.kind=kind; e.action=action; e.high=high; e.rising=rising; e.frame=frame; return e;
}
static void eventTests() {
    GateDetector detector; detector.seed(5); assert(!detector.process(5) && detector.high());
    assert(!detector.process(.2f) && detector.high());
    assert(!detector.process(.1f) && !detector.high());
    assert(detector.process(1) && !detector.process(1));
    EventState e;
    e.apply(event(EventKind::Button,Action::Bend)); assert(e.controls.macroBend);
    e.apply(event(EventKind::Button,Action::Mode)); assert(e.controls.mode==Mode::Micro && !e.effective().bend);
    e.apply(event(EventKind::Button,Action::Bend)); assert(e.controls.microReverse);
    e.apply(event(EventKind::Button,Action::Mode)); assert(e.controls.macroBend && e.controls.microReverse);
    e.settings.gates=GateBehavior::Level;
    e.apply(event(EventKind::Gate,Action::Freeze,true,false));
    assert(e.effective().freeze && !e.controls.buttonFreeze);
    e.apply(event(EventKind::Button,Action::Freeze)); e.apply(event(EventKind::Button,Action::Freeze));
    assert(e.effective().freeze && !e.controls.buttonFreeze);
    e.settings.freezeButton=FreezeButton::Momentary;
    e.apply(event(EventKind::Button,Action::Freeze)); assert(e.effective().freeze);
    e.apply(event(EventKind::Button,Action::Freeze,false)); assert(!e.controls.buttonFreeze && e.effective().freeze);
    e.settings.gates=GateBehavior::Latching;
    e.apply(event(EventKind::Button,Action::Freeze));
    e.apply(event(EventKind::Gate,Action::Freeze,true,true));
    e.apply(event(EventKind::Button,Action::Freeze,false));
    assert(e.effective().freeze); // releasing momentary button preserves gate latch
    e.apply(event(EventKind::Gate,Action::Freeze,true,true));
    assert(!e.effective().freeze);
    e.apply(event(EventKind::Button,Action::Freeze));
    e.apply(event(EventKind::Gate,Action::Freeze,true,true));
    e.apply(event(EventKind::Gate,Action::Freeze,true,true));
    assert(e.effective().freeze); // latching gate cannot cancel a held momentary button
    e.apply(event(EventKind::Button,Action::Freeze,false));
    e.settings.effectSet=EffectSet::OriginalThree; e.controls.effect=Effect::Vinyl; e.normalizeSelection();
    assert(e.controls.effect==Effect::Decimate);
    for(unsigned i=0;i<3;++i) e.apply(event(EventKind::Gate,Action::Corrupt,true,true));
    assert(e.controls.effect==Effect::Decimate);
    e.settings.corruptGate=CorruptGate::ClockReset;
    assert(e.apply(event(EventKind::Gate,Action::Corrupt,true,true)));
    e.controls.seed=123; e.controls.clockSource=ClockSource::External;
    e.settings.window=.9f; e.settings.separation=0; e.settings.bendDepth=e.settings.breakDepth=e.settings.corruptDepth=0;
    e.settings.unique=false; e.controls.macroBreak=e.controls.microSilence=true;
    e.restoreSecondaryDefaults();
    assert(e.controls.seed==123 && e.controls.clockSource==ClockSource::External);
    assert(e.controls.mode==Mode::Macro && !e.controls.macroBend && !e.controls.macroBreak && !e.controls.microReverse && !e.controls.microSilence && !e.controls.buttonFreeze);
    assert(e.settings.window==SecondarySettings{}.window && e.settings.separation==1);
    assert(e.settings.bendDepth==1 && e.settings.breakDepth==1 && e.settings.corruptDepth==1 && e.settings.unique);
    assert(e.settings.gates==GateBehavior::Latching && e.settings.freezeButton==FreezeButton::Latching
        && e.settings.corruptGate==CorruptGate::Advance && e.settings.effectSet==EffectSet::AllFive && e.controls.effect==Effect::Decimate);
    BlockEvents queue;
    queue.append(event(EventKind::Gate,Action::Bend,true,true,0));
    queue.append(event(EventKind::Button,Action::Mode,true,false,20));
    queue.append(event(EventKind::Button,Action::Freeze,true,false,5));
    assert(queue.values[0].action==Action::Freeze && queue.values[1].action==Action::Mode && queue.values[2].kind==EventKind::Gate);
    while(queue.count<queue.values.size()) assert(queue.append(event(EventKind::Button,Action::Bend)));
    assert(!queue.append(event(EventKind::Button,Action::Bend)));
    Random rng, reference; CorruptRoutingState routing;
    for(float amount:{0.f,1.f/512,.5f,1.f}) {
        retainedCorruptDecision(routing,amount,rng); reference.next(); reference.next();
        assert(rng.state()==reference.state());
        assert(int(routing.retained)>=1 && int(routing.retained)<=3);
        if(amount<1.f/256) assert(routing.retainedAmount==0);
    }
    // Exhaustive full-amount Bend weighting: +1.5 owns 85/255 residues.
    unsigned weighted=0; MacroState m;
    for(std::uint32_t i=0;i<255;++i) {
        const std::uint32_t draws[]={i,0}; Random r; RandomTestAccess::inject(r,draws,2);
        chooseBend(m,1,r); weighted+=m.rate==1.5f;
    }
    assert(weighted==85);
}
static void integration() {
    BufferEngine engine;
    BufferBlockInput c; c.primary.time=float(std::log(32.)/7.15461540222168);
    std::array<float,192> x,y; x.fill(.4f);
    for(unsigned i=0;i<5000;++i) engine.processBlock(x.data(),y.data(),c);
    assert(engine.clockBoundaryCount()==20);
    const unsigned n=engine.buffer().channel(0).writer.captureFrames;
    assert(n>=23998 && n<=24001);
    auto& e=engine.eventState(); e.settings.freezeButton=FreezeButton::Momentary;
    c.events.append(event(EventKind::Button,Action::Freeze));
    engine.processBlock(x.data(),y.data(),c); assert(engine.buffer().transition().freezeActive);
    c.events=BlockEvents{}; c.events.append(event(EventKind::Button,Action::Freeze,false));
    engine.processBlock(x.data(),y.data(),c); assert(!engine.buffer().transition().freezeActive);
    e.settings.gates=GateBehavior::Level;
    c.events=BlockEvents{}; c.events.append(event(EventKind::Gate,Action::Freeze,true,false));
    engine.processBlock(x.data(),y.data(),c); assert(engine.buffer().transition().freezeActive);
    c.events=BlockEvents{}; c.restoreSecondary=true;
    engine.processBlock(x.data(),y.data(),c);
    assert(!engine.buffer().transition().freezeRequested); // defaults return gates to Latching
    // A request alone waits for a qualifying pending path; replacing the
    // request twice before acceptance does not create a second future toggle.
    Buffer isolated(32768,coefficientRate,1000); Random rng;
    BufferControls controls; controls.frequency=coefficientRate/1000; controls.window=0;
    controls.freezeRequested=true;
    isolated.processBlock(x.data(),y.data(),controls,rng);
    assert(!isolated.transition().freezeActive);
    controls.freezeRequested=false; controls.clockRequest=true;
    isolated.processBlock(x.data(),y.data(),controls,rng);
    assert(!isolated.transition().freezeActive);
    controls.freezeRequested=true;
    for(unsigned i=0;i<12;++i) {
        isolated.processBlock(x.data(),y.data(),controls,rng); controls.clockRequest=false;
    }
    assert(isolated.transition().freezeActive);
    controls.freezeRequested=false; controls.clockRequest=true;
    for(unsigned i=0;i<12;++i) {
        isolated.processBlock(x.data(),y.data(),controls,rng); controls.clockRequest=false;
    }
    assert(!isolated.transition().freezeActive);
    std::cout << "Headless engine: normalized 500 ms capture, clock/mapping composition, momentary Freeze and defaults passed\n";
}
int main() { fixtures(); clockTests(); eventTests(); integration(); std::cout << "Tiamat event tests passed\n"; }

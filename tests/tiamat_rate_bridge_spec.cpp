#include "Tiamat/TiamatTransport.hpp"
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <limits>
#include <thread>
#include <vector>

static thread_local bool audioThread=false;
static thread_local unsigned allocations=0,deletions=0;
__attribute__((noinline)) void* operator new(std::size_t n) { if(audioThread) ++allocations; if(void* p=std::malloc(n?n:1)) return p; throw std::bad_alloc(); }
__attribute__((noinline)) void* operator new[](std::size_t n) { return ::operator new(n); }
__attribute__((noinline)) void operator delete(void* p) noexcept { if(audioThread && p) ++deletions; std::free(p); }
__attribute__((noinline)) void operator delete[](void* p) noexcept { ::operator delete(p); }
#ifdef TIAMAT_WRAP_C_ALLOCATION
extern "C" {
void* __real_malloc(std::size_t); void* __real_calloc(std::size_t,std::size_t);
void* __real_realloc(void*,std::size_t); void __real_free(void*);
void* __wrap_malloc(std::size_t n) { if(audioThread) ++allocations; return __real_malloc(n); }
void* __wrap_calloc(std::size_t n,std::size_t size) { if(audioThread) ++allocations; return __real_calloc(n,size); }
void* __wrap_realloc(void* p,std::size_t n) { if(audioThread) ++allocations; return __real_realloc(p,n); }
void __wrap_free(void* p) { if(audioThread && p) ++deletions; __real_free(p); }
}
#endif
namespace tiamat {
struct BufferTestAccess {
    static const float* plane(const Buffer& b,unsigned ch) { return b.memory_.get()+ch*b.capacity_; }
};
}
using namespace tiamat;
static void require(bool condition,const char* message) { if(!condition) { std::fprintf(stderr,"FAIL: %s\n",message); std::abort(); } }
static void copy(const float* x,float* y,const BufferBlockInput&) { std::copy(x,x+192,y); }

static void rates() {
    for(unsigned rate : {8000u,44100u,48000u,88200u,96000u,192000u,768000u}) {
        RateBridge b(rate); require(b.valid(),"bridge prepares");
        HostFrame h; h.leftConnected=true;
        const unsigned impulse=rate/20;
        float peak=0; unsigned peakFrame=0,clock=0,freeze=0,settings=0,restarts=0;
        unsigned highWater=0,lowWater=4096;
        audioThread=true;
        for(unsigned frame=0;frame<rate*10;++frame) {
            h.leftVolts=frame==impulse ? 5.f : 0.f;
            h.gateVolts[0]=h.gateVolts[1]=frame==impulse ? 10.f : 0.f;
            h.controls.cv.time=float(frame);
            h.commandCount=0;
            if(frame==impulse) {
                h.commandCount=2; h.commands[0].kind=CommandKind::Settings; h.commands[0].settings.separation=.25f;
                h.commands[1].kind=CommandKind::RestartRandom; h.commands[1].seed=123;
            }
            const auto out=b.step(h,[&](const float* x,float* y,const BufferBlockInput& block) {
                const auto first=b.coreFrames()-96;
                const auto source=first<b.inputLatencyCore() ? 0 : (first-b.inputLatencyCore())*rate/48000;
                require(block.cv.time==float(std::min<std::uint64_t>(source,frame)),"controls follow delayed input block start");
                for(unsigned i=0;i<96;++i) if(block.clockRises[i]) {
                    ++clock;
                    require(first+i==(std::uint64_t(impulse)*48000+rate-1)/rate+b.inputLatencyCore(),"narrow clock timestamp includes input latency");
                }
                for(unsigned i=0;i<block.events.count;++i) if(block.events.values[i].action==Action::Freeze && block.events.values[i].rising) ++freeze;
                settings+=block.updateSettings; restarts+=block.restartRandom;
                if(block.updateSettings) require(block.settings.separation==.25f,"settings payload survives transport");
                if(block.restartRandom) require(block.seed==123,"restart payload survives transport");
                copy(x,y,block);
            });
            require(!b.failed(),"bounded transport has no underrun or overflow");
            require(out.left==out.right,"unpatched right normals to left with aligned SRC");
            if(std::abs(out.left)>peak) { peak=std::abs(out.left); peakFrame=frame; }
            if(frame>rate/10) { highWater=std::max(highWater,b.pendingOutput()); lowWater=std::min(lowWater,b.pendingOutput()); }
        }
        audioThread=false;
        require(allocations==0 && deletions==0,"host stepping allocates/deletes nothing");
        require(clock==1 && freeze==1 && settings==1 && restarts==1,"single-host-sample gates/commands delivered exactly once");
        require(b.underruns()==0 && b.pendingCommands()==0,"no FIFO drift or stuck commands");
        require(std::abs(double(peakFrame)-impulse-b.latencyHostFrames())<=2.0,"measured impulse peak matches reported latency");
        require(std::abs(double(b.coreFrames())-480000)<=2,"ten seconds advances exactly fixed-rate time");
        if(rate==48000) require(peakFrame==impulse+96 && peak==5.f,"48k exact 96-frame delay and unity scale");
        std::printf("rate %u: peak delay %u, predicted %.3f host frames, FIFO %u..%u, events once\n",rate,peakFrame-impulse,b.latencyHostFrames(),lowWater,highWater);
    }
}

static void seedingAndBounds() {
    RateBridge b(192000); HostFrame h;
    for(auto& v:h.gateVolts) v=10;
    unsigned rises=0; bool freezeHigh=false;
    auto render=[&](const float* x,float* y,const BufferBlockInput& block) {
        for(auto rise:block.clockRises) rises+=rise;
        for(unsigned i=0;i<block.events.count;++i) { rises+=block.events.values[i].rising; if(block.events.values[i].action==Action::Freeze) freezeHigh=block.events.values[i].high; }
        copy(x,y,block);
    };
    for(unsigned i=0;i<2000;++i) b.step(h,render);
    require(!rises && freezeHigh,"held load gates seed levels without invented edges");
    RateBridge resumed(192000); resumed.resumeGates(b.gateHigh());
    for(auto& v:h.gateVolts) v=.5f;
    freezeHigh=false;
    for(unsigned i=0;i<2000;++i) resumed.step(h,render);
    require(!rises && freezeHigh,"replacement preserves hysteresis band baseline");
    for(auto& v:h.gateVolts) v=0;
    for(unsigned i=0;i<1000;++i) resumed.step(h,render);
    for(auto& v:h.gateVolts) v=10;
    for(unsigned i=0;i<1000;++i) resumed.step(h,render);
    require(rises==5,"real subsequent edges delivered once");
    RateBridge fast(192000); h=HostFrame{}; unsigned clocks=0;
    for(unsigned i=0;i<2000;++i) {
        h.gateVolts[0]=i>=100 && i<108 && i%2==0 ? 10.f : 0.f;
        fast.step(h,[&](const float*x,float*y,const BufferBlockInput& block) { for(auto r:block.clockRises) clocks+=r; copy(x,y,block); });
    }
    require(clocks==2,"sub-core-frame clock edges rejected before coalescing");
    RateBridge overloaded(48000); h=HostFrame{}; h.commandCount=8;
    for(unsigned i=0;i<20;++i) overloaded.step(h,copy);
    require(overloaded.failed(),"command exhaustion exposes bounded failure");
    RateBridge invalid(0); require(!invalid.valid(),"invalid host rate rejects preparation");
    require(!RateBridge::supported(std::numeric_limits<float>::infinity()) && !RateBridge::supported(44100.5f),"invalid rates do not reach SRC");
    for(unsigned rate : {44100u,48000u,96000u,192000u}) {
        RateBridge plain(rate),faded(rate); faded.fadeIn();
        HostFrame constant; constant.leftConnected=true; constant.leftVolts=2;
        const unsigned duration=(240u*rate+47999)/48000;
        for(unsigned frame=0;frame<rate/20;++frame) {
            const auto a=plain.step(constant,copy),b=faded.step(constant,copy);
            const float gain=float(std::max(0.0,std::min(1.0,(double(frame)-faded.latencyHostFrames())/duration)));
            require(b.left==a.left*gain && b.right==a.right*gain,"replacement fade spans 240 core frames after transport prime");
        }
    }
}

static void replacement() {
    Transport t; HostFrame h; h.leftConnected=true; h.controls.primary.time=1;
    t.core().eventState().controls.mode=Mode::Micro;
    for(unsigned i=0;i<15000;++i) { h.leftVolts=std::sin(float(i)*.03f); t.step(h,48000); }
    t.core().eventState().settings.freezeButton=FreezeButton::Momentary;
    h.commandCount=1; h.commands[0].control.action=Action::Freeze; h.commands[0].control.high=true;
    t.step(h,48000); h.commandCount=0;
    for(unsigned i=0;i<200;++i) t.step(h,48000);
    require(t.core().bufferEngine().buffer().transition().freezeActive,"frozen audio before rate change");
    const auto* memory=&t.core().bufferEngine().buffer();
    const auto anchor=memory->channel(0).reader.anchor;
    const auto random=t.core().bufferEngine().randomState();
    const unsigned capture=memory->channel(0).writer.captureFrames;
    const float* captured=BufferTestAccess::plane(*memory,0)+anchor;
    const std::vector<float> original(captured,captured+capture);
    double capturedEnergy=0; for(float x:original) capturedEnergy+=x*x;
    require(capturedEnergy>1e-3,"test froze actual captured audio");
    // Queue an action in the old incomplete block, then a restart while waiting.
    h.commandCount=1; h.commands[0].control.action=Action::Bend;
    t.step(h,48000); h.commandCount=0;
    auto silent=t.step(h,96000); require(silent.left==0 && silent.right==0,"unprepared rate emits silence");
    h.commandCount=1; h.commands[0].kind=CommandKind::RestartRandom; h.commands[0].seed=99;
    t.step(h,96000); h.commandCount=0;
    require(t.core().bufferEngine().randomState()==random,"waiting does not advance fixed-rate core");
    t.prepare();
    audioThread=true;
    for(unsigned i=0;i<2000;++i) t.step(h,96000);
    audioThread=false;
    require(allocations==0 && deletions==0,"adoption and retirement allocate/delete nothing on audio");
    require(!t.failed() && t.bridge()->rate()==96000,"replacement active");
    require(memory==&t.core().bufferEngine().buffer() && memory->transition().freezeActive && memory->channel(0).reader.anchor==anchor,"replacement retains fixed core and protected capture anchor");
    require(std::memcmp(original.data(),captured,original.size()*sizeof(float))==0,"frozen captured samples survive rate replacement bit-for-bit");
    require(t.core().eventState().controls.microReverse && t.core().eventState().controls.seed==99,"pending old-block and preparation-time commands both survive");
    t.prepare(); // retire only off audio
    // Real concurrent preparation while audio changes host rates repeatedly.
    std::atomic<bool> stop{false};
    std::thread worker([&] { while(!stop.load(std::memory_order_acquire)) { t.prepare(); std::this_thread::yield(); } });
    audioThread=true;
    for(unsigned i=0;i<100000;++i) {
        const float rate=(i/4096)%3==0 ? 44100.f : (i/4096)%3==1 ? 192000.f : 48000.f;
        t.step(h,rate);
        if(i%100==0) std::this_thread::yield();
    }
    audioThread=false;
    stop.store(true,std::memory_order_release); worker.join(); t.prepare();
    require(!t.failed() && allocations==0 && deletions==0,"concurrent preparation/replacement ownership remains realtime-safe");
}
int main() {
    if(!std::getenv("TIAMAT_TRANSPORT_CONCURRENCY_ONLY")) { rates(); seedingAndBounds(); }
    replacement(); std::puts("Tiamat host transport passed");
}

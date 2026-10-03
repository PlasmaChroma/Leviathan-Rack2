#include "Tiamat/TiamatRuntime.hpp"
#include <cstdio>
#include <cstdlib>

namespace tiamat {
struct BufferTestAccess {
    static const float* plane(const Buffer& b, unsigned ch) { return b.memory_.get()+ch*b.capacity_; }
    static unsigned size(const Buffer& b) { return b.capacity_; }
};
}
static void check(bool ok,const char* text) { if(!ok) { std::fprintf(stderr,"FAIL: %s\n",text); std::abort(); } }
int main() {
    using namespace tiamat;
    Runtime runtime;
    HostFrame h; h.leftConnected=true; h.leftVolts=5;
    for(unsigned i=0;i<12000;++i) runtime.step(h,48000);
    const auto& recorded=runtime.audioTransport().core().bufferEngine().buffer();
    bool nonzero=false;
    for(unsigned i=0;i<BufferTestAccess::size(recorded);++i) nonzero |= BufferTestAccess::plane(recorded,0)[i]!=0;
    check(nonzero,"input captured before reset");
    runtime.reset(); h.leftVolts=0;
    auto end=std::chrono::steady_clock::now()+std::chrono::seconds(5);
    while(runtime.audioSnapshot().generation!=runtime.generation()) {
        runtime.step(h,48000);
        check(std::chrono::steady_clock::now()<end,"reset adoption deadline");
        std::this_thread::yield();
    }
    const auto& cleared=runtime.audioTransport().core().bufferEngine().buffer();
    for(unsigned ch=0;ch<2;++ch) for(unsigned i=0;i<BufferTestAccess::size(cleared);++i)
        check(BufferTestAccess::plane(cleared,ch)[i]==0,"both memory planes cleared by prepared reset");
    std::atomic<bool> finished{false};
    std::thread control([&] {
        for(unsigned i=0;i<80;++i) {
            Preset p; p.controls.seed=500+i; runtime.reset(p);
            runtime.editSettings([&](SecondarySettings& s) { s.separation=float(i%10)/10; });
            (void)runtime.snapshot();
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }
        finished.store(true,std::memory_order_release);
    });
    unsigned frames=0;
    while(!finished.load(std::memory_order_acquire)) {
        runtime.step(h,(frames++/2048)%2 ? 44100 : 96000);
        if(frames%384==0) std::this_thread::yield();
    }
    control.join();
    end=std::chrono::steady_clock::now()+std::chrono::seconds(5);
    do {
        for(unsigned i=0;i<384;++i) runtime.step(h,48000);
        check(std::chrono::steady_clock::now()<end,"final generation adoption deadline");
        std::this_thread::yield();
    } while(runtime.audioSnapshot().generation!=runtime.generation() || runtime.audioTransport().bridge()->rate()!=48000);
    check(runtime.snapshot().preset.controls.seed==579 && !runtime.snapshot().fault,"latest generation healthy after concurrent replacement");
    std::puts("Tiamat runtime: cleared memory and concurrent reset/settings/rate replacement PASS");
}

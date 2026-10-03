#include "Tiamat/TiamatModule.hpp"
#include "Tiamat/TiamatPersistence.hpp"
#include <cstdio>
#include <cstdlib>
#include <limits>

static thread_local bool audioThread = false;
static thread_local unsigned allocations = 0, deletions = 0;
__attribute__((noinline)) void* operator new(std::size_t n) {
    if (audioThread) ++allocations;
    if (void* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
__attribute__((noinline)) void* operator new[](std::size_t n) { return ::operator new(n); }
__attribute__((noinline)) void operator delete(void* p) noexcept { if (audioThread && p) ++deletions; std::free(p); }
__attribute__((noinline)) void operator delete[](void* p) noexcept { ::operator delete(p); }
static void check(bool ok, const char* label) { if (!ok) { std::fprintf(stderr,"FAIL: %s\n",label); std::abort(); } }
static void process(Tiamat& m, unsigned frames=384, float rate=48000) {
    rack::engine::Module::ProcessArgs args; args.sampleRate=rate; args.sampleTime=1/rate;
    audioThread=true;
    for (unsigned i=0;i<frames;++i) { args.frame=i; m.process(args); }
    audioThread=false;
}
static void adopted(Tiamat& m, float rate=48000) {
    const auto end=std::chrono::steady_clock::now()+std::chrono::seconds(5);
    do {
        process(m,384,rate);
        if (m.runtime.audioSnapshot().generation==m.runtime.generation() && !m.runtime.audioSnapshot().busy) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    } while(std::chrono::steady_clock::now()<end);
    check(false,"prepared reset adoption deadline");
}
static void press(Tiamat& m,int param) {
    m.params[param].setValue(1); process(m);
    m.params[param].setValue(0); process(m);
}
static void persistence() {
    using namespace tiamat;
    std::uint64_t seed=17;
    check(parseSeed("18446744073709551615",seed) && seed==UINT64_MAX,"full decimal seed");
    check(parseSeed("0xffffffffffffffff",seed) && seed==UINT64_MAX,"full hex seed");
    for(const char* s : {"", "-1", "+1", "0x", "0x10000000000000000", "18446744073709551616", "1.2", " 1"})
        check(!parseSeed(s,seed),"malformed seed rejected");
    Preset p; p.controls.seed=UINT64_MAX; p.controls.mode=Mode::Micro;
    p.controls.clockSource=ClockSource::External; p.controls.effect=Effect::Vinyl;
    p.controls.macroBend=p.controls.macroBreak=p.controls.microReverse=p.controls.microSilence=p.controls.buttonFreeze=true;
    p.settings.window=.7f; p.settings.separation=.2f; p.settings.unique=false;
    p.settings.gates=GateBehavior::Level; p.settings.freezeButton=FreezeButton::Momentary;
    json_t* j=presetToJson(p); const auto q=presetFromJson(j);
    check(q.controls.seed==UINT64_MAX && q.controls.mode==Mode::Micro && q.controls.effect==Effect::Vinyl,"control roundtrip");
    check(q.controls.macroBend && q.controls.macroBreak && q.controls.microReverse && q.controls.microSilence,"independent flags roundtrip");
    check(!q.controls.buttonFreeze && !json_object_get(j,"buttonFreeze"),"Freeze excluded from patch state");
    check(q.settings.window==.7f && q.settings.separation==.2f && !q.settings.unique && q.settings.gates==GateBehavior::Level,"settings roundtrip");
    json_object_set_new(j,"window",json_real(1e100)); json_object_set_new(j,"separation",json_real(-1e100));
    json_object_set_new(j,"effectSet",json_integer(int(EffectSet::OriginalThree)));
    const auto bounded=presetFromJson(j);
    check(bounded.settings.window==1 && bounded.settings.separation==0 && bounded.controls.effect==Effect::Decimate,"malformed ranges bounded before narrowing");
    json_object_set_new(j,"schema",json_integer(999));
    check(presetFromJson(j).controls.seed==1,"unknown schema defaults safely");
    json_decref(j);
}
static void module() {
    Tiamat m; m.outputs[0].channels=1; m.outputs[1].channels=1; process(m);
    check(m.params.size()==12 && m.inputs.size()==13 && m.outputs.size()==2,"Rack control counts");
    press(m,Tiamat::MODE_BUTTON); press(m,Tiamat::BEND_BUTTON);
    check(m.runtime.snapshot().preset.controls.microReverse,"Micro reverse button");
    press(m,Tiamat::MODE_BUTTON); press(m,Tiamat::BREAK_BUTTON);
    check(m.runtime.snapshot().preset.controls.macroBreak && m.runtime.snapshot().preset.controls.microReverse,"mode flags remain independent");
    press(m,Tiamat::FREEZE_BUTTON);
    check(m.runtime.snapshot().status.freezeRequested,"Freeze button request");
    json_t* saved=m.dataToJson(); m.dataFromJson(saved); adopted(m); json_decref(saved);
    check(!m.runtime.snapshot().status.freezeRequested && m.runtime.snapshot().preset.controls.macroBreak,"load clears Freeze and preserves flags");
    m.runtime.editSettings([](tiamat::SecondarySettings& s) { s.gates=tiamat::GateBehavior::Level; s.freezeButton=tiamat::FreezeButton::Momentary; });
    process(m);
    m.params[Tiamat::FREEZE_BUTTON].setValue(1); process(m);
    check(m.runtime.snapshot().status.freezeRequested,"momentary press");
    m.params[Tiamat::FREEZE_BUTTON].setValue(0); process(m);
    check(!m.runtime.snapshot().status.freezeRequested,"momentary release");
    m.inputs[Tiamat::BEND_GATE_INPUT].channels=1; m.inputs[Tiamat::BEND_GATE_INPUT].setVoltage(10); process(m);
    check(m.lights[Tiamat::BEND_LIGHT].getBrightness()==1 && !m.runtime.snapshot().preset.controls.macroBend,"level gate lights effective flag without latching");
    m.inputs[Tiamat::BEND_GATE_INPUT].setVoltage(0); process(m);
    check(m.lights[Tiamat::BEND_LIGHT].getBrightness()==0,"level gate release");
    m.runtime.restartRandom(UINT64_MAX); m.runtime.restoreSecondary(); process(m);
    check(m.runtime.snapshot().preset.controls.seed==UINT64_MAX && !m.runtime.snapshot().preset.controls.microReverse,"secondary defaults preserve seed and clear flags");
    m.inputs[Tiamat::LEFT_INPUT].channels=2; m.inputs[Tiamat::LEFT_INPUT].setVoltage(3,0); m.inputs[Tiamat::LEFT_INPUT].setVoltage(9,1);
    rack::engine::Module::ProcessArgs args{}; m.processBypass(args);
    check(m.outputs[0].getVoltage()==3 && m.outputs[1].getVoltage()==3 && m.outputs[0].getChannels()==1,"bypass mono and right normaling");
    m.inputs[Tiamat::RIGHT_INPUT].channels=1; m.inputs[Tiamat::RIGHT_INPUT].setVoltage(-2); m.processBypass(args);
    check(m.outputs[1].getVoltage()==-2,"patched right bypass");
    m.inputs[Tiamat::LEFT_INPUT].setVoltage(std::numeric_limits<float>::quiet_NaN()); m.processBypass(args);
    check(m.outputs[0].getVoltage()==0,"bypass sanitizes nonfinite");
    m.inputs[Tiamat::LEFT_INPUT].setVoltage(5); process(m,2000);
    m.runtime.action(tiamat::Action::Mode); // stale command must not survive reset
    m.runtime.reset(); adopted(m);
    check(m.runtime.snapshot().preset.controls.mode==tiamat::Mode::Macro && m.runtime.snapshot().preset.controls.seed==1,"reset discards old command generation");
    // Reset an incomplete block during a rate transition; cannot wait forever
    // for an abandoned old-rate block to finish.
    process(m,1); m.runtime.reset(); adopted(m,96000);
    check(!m.runtime.snapshot().fault,"reset during rate transition");
    for(unsigned i=0;i<64;++i) check(m.runtime.action(tiamat::Action::Mode),"bounded queue accepts capacity");
    check(!m.runtime.action(tiamat::Action::Mode) && m.runtime.snapshot().fault,"queue overflow explicitly reported");
    m.runtime.reset(); adopted(m);
    check(!m.runtime.snapshot().fault,"reset recovers command overflow");
    // Concurrent menu, reset, rate replacement and audioThread ownership stress.
    std::thread control([&] {
        for(unsigned i=0;i<30;++i) {
            tiamat::Preset p; p.controls.seed=100+i; m.runtime.reset(p);
            m.runtime.editSettings([&](tiamat::SecondarySettings& s) { s.separation=float(i%10)/10; });
            (void)m.runtime.snapshot();
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
        }
    });
    for(unsigned i=0;i<150;++i) { process(m,384,i%2 ? 44100 : 48000); std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
    control.join(); adopted(m);
    check(m.runtime.snapshot().preset.controls.seed==129,"latest reset survives concurrent handoffs");
    check(!m.runtime.snapshot().fault,"concurrent handoffs remain healthy");
}
int main() {
    persistence(); module();
    check(!allocations && !deletions,"no C++ allocation or deletion in module process");
    std::puts("Tiamat module: persistence, controls, prepared reset, concurrency and audioThread allocation guards PASS");
}

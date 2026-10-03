#include "TiamatModule.hpp"
#include "TiamatPersistence.hpp"

namespace {
struct TiamatQuantity final : ParamQuantity {
    std::string getDisplayValueString() override {
        auto* tiamat = dynamic_cast<Tiamat*>(module);
        if (!tiamat) return ParamQuantity::getDisplayValueString();
        const auto s = tiamat->runtime.snapshot();
        const auto& m = s.mapped;
        switch (paramId) {
        case Tiamat::TIME_PARAM: return s.preset.controls.clockSource == tiamat::ClockSource::External
            ? string::f("x%.4g",tiamat::externalRatio(m.time)) : string::f("%.3g Hz",tiamat::internalFrequency(m.time));
        case Tiamat::REPEATS_PARAM: return string::f("%u divisions",1u << m.repeatsExponent);
        case Tiamat::MIX_PARAM: return string::f("%.1f%% wet target",m.mixTarget*100);
        case Tiamat::BEND_PARAM: return s.preset.controls.mode == tiamat::Mode::Micro ? string::f("%+.3gx speed",m.baseRate) : string::f("%.1f%% Macro",m.macroBend*100);
        case Tiamat::BREAK_PARAM: return s.preset.controls.mode == tiamat::Mode::Micro
            ? string::f("%.1f%% %s",100*(s.preset.controls.microSilence ? m.manualSilence : m.traverse),s.preset.controls.microSilence ? "silence" : "traverse") : string::f("%.1f%% Macro",m.macroBreak*100);
        case Tiamat::CORRUPT_PARAM: return string::f("%.1f%% effective",m.corrupt*100);
        default: return ParamQuantity::getDisplayValueString();
        }
    }
};
}
Tiamat::Tiamat() {
    config(PARAMS_LEN,INPUTS_LEN,OUTPUTS_LEN,LIGHTS_LEN);
    const char* names[] = {"Time","Repeats","Mix","Bend","Break","Corrupt"};
    const float defaults[] = {.5f,0,1,.5f,0,0};
    for (int i=0;i<6;++i) configParam<TiamatQuantity>(i,0,1,defaults[i],names[i]);
    const char* actions[] = {"Internal / external clock","Macro / Micro mode","Freeze","Bend / reverse","Break / silence","Advance Corrupt effect"};
    for (int i=0;i<6;++i) configButton(CLOCK_BUTTON+i,actions[i]);
    configInput(LEFT_INPUT,"Audio L"); configInput(RIGHT_INPUT,"Audio R (normalled to L)");
    for (int i=0;i<6;++i) configInput(TIME_INPUT+i,std::string(names[i])+" CV");
    const char* gates[] = {"Clock","Freeze gate","Bend gate","Break gate","Corrupt gate"};
    for (int i=0;i<5;++i) configInput(CLOCK_INPUT+i,gates[i]);
    configOutput(LEFT_OUTPUT,"Audio L"); configOutput(RIGHT_OUTPUT,"Audio R");
}
void Tiamat::process(const ProcessArgs& args) {
    tiamat::HostFrame host;
    host.leftConnected=inputs[LEFT_INPUT].isConnected(); host.rightConnected=inputs[RIGHT_INPUT].isConnected();
    host.leftVolts=inputs[LEFT_INPUT].getVoltage(0); host.rightVolts=inputs[RIGHT_INPUT].getVoltage(0);
    auto& p=host.controls.primary; p.time=params[TIME_PARAM].getValue(); p.repeats=params[REPEATS_PARAM].getValue(); p.mix=params[MIX_PARAM].getValue();
    p.bend=params[BEND_PARAM].getValue(); p.brk=params[BREAK_PARAM].getValue(); p.corrupt=params[CORRUPT_PARAM].getValue();
    auto& cv=host.controls.cv; cv.time=inputs[TIME_INPUT].getVoltage(0); cv.repeats=inputs[REPEATS_INPUT].getVoltage(0); cv.mix=inputs[MIX_INPUT].getVoltage(0);
    cv.bend=inputs[BEND_INPUT].getVoltage(0); cv.brk=inputs[BREAK_INPUT].getVoltage(0); cv.corrupt=inputs[CORRUPT_INPUT].getVoltage(0);
    for(unsigned i=0;i<5;++i) host.gateVolts[i]=inputs[CLOCK_INPUT+i].getVoltage(0);
    const auto generation=runtime.generation();
    const tiamat::Action actions[]={tiamat::Action::ClockSource,tiamat::Action::Mode,tiamat::Action::Freeze,tiamat::Action::Bend,tiamat::Action::Break,tiamat::Action::Corrupt};
    for(unsigned i=0;i<6;++i) {
        const bool high=params[CLOCK_BUTTON+i].getValue()>=.5f;
        if(buttonGeneration_==generation && high!=buttonHigh_[i]) {
            auto& command=host.commands[host.commandCount++]; command.control.action=actions[i]; command.control.high=high;
        }
        buttonHigh_[i]=high;
    }
    buttonGeneration_=generation;
    const auto output=runtime.step(host,args.sampleRate);
    outputs[LEFT_OUTPUT].setChannels(1); outputs[RIGHT_OUTPUT].setChannels(1);
    outputs[LEFT_OUTPUT].setVoltage(output.left); outputs[RIGHT_OUTPUT].setVoltage(output.right);
    const auto& s=runtime.audioSnapshot(); const auto& c=s.preset.controls;
    lights[EXTERNAL_LIGHT].setBrightness(c.clockSource==tiamat::ClockSource::External);
    lights[MICRO_LIGHT].setBrightness(c.mode==tiamat::Mode::Micro);
    lights[FREEZE_REQUEST_LIGHT].setBrightness(s.status.freezeRequested); lights[FREEZE_ACTIVE_LIGHT].setBrightness(s.status.freezeActive);
    lights[BEND_LIGHT].setBrightness(s.effective.bend);
    lights[BREAK_LIGHT].setBrightness(s.effective.brk);
    for(int i=0;i<5;++i) lights[DECIMATE_LIGHT+i].setBrightness(int(c.effect)==i+1);
    lights[CLOCK_LOST_LIGHT].setBrightness(s.status.clockLost); lights[CAPACITY_LIGHT].setBrightness(s.status.capacityLimited);
    lights[FAULT_LIGHT].setBrightness(s.fault || s.busy);
}
void Tiamat::processBypass(const ProcessArgs&) {
    const float l=inputs[LEFT_INPUT].isConnected() ? tiamat::finiteOrZero(inputs[LEFT_INPUT].getVoltage(0)) : 0.f;
    const float r=inputs[RIGHT_INPUT].isConnected() ? tiamat::finiteOrZero(inputs[RIGHT_INPUT].getVoltage(0)) : l;
    outputs[LEFT_OUTPUT].setChannels(1); outputs[RIGHT_OUTPUT].setChannels(1);
    outputs[LEFT_OUTPUT].setVoltage(l); outputs[RIGHT_OUTPUT].setVoltage(r);
}
void Tiamat::onReset(const ResetEvent& e) { Module::onReset(e); runtime.reset(); }
json_t* Tiamat::dataToJson() { return tiamat::presetToJson(runtime.snapshot().preset); }
void Tiamat::dataFromJson(json_t* root) { runtime.reset(tiamat::presetFromJson(root)); }

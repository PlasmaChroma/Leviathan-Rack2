#include "TiamatModule.hpp"
#include "TiamatPersistence.hpp"
#include "../PanelSvgUtils.hpp"
#include "../visual/VisualAssets.hpp"

namespace {
struct SettingSlider final : ui::Slider {
    struct SettingQuantity final : Quantity {
        Tiamat* module; float tiamat::SecondarySettings::*field; std::string label; float initial;
        SettingQuantity(Tiamat* m,float tiamat::SecondarySettings::*f,std::string name,float d) : module(m),field(f),label(name),initial(d) {}
        float getValue() override { return module->runtime.snapshot().preset.settings.*field; }
        void setValue(float v) override { module->runtime.editSettings([&](tiamat::SecondarySettings& s) { s.*field=tiamat::normalized(v); }); }
        float getMinValue() override { return 0; } float getMaxValue() override { return 1; }
        float getDefaultValue() override { return initial; }
        std::string getLabel() override { return label; }
        float getDisplayValue() override { return getValue()*100; }
        void setDisplayValue(float v) override { setValue(v/100); }
        std::string getUnit() override { return "%"; }
    } value;
    SettingSlider(Tiamat* m,float tiamat::SecondarySettings::*field,const char* label,float initial) : value(m,field,label,initial) {
        quantity=&value; box.size.x=240;
    }
};
struct SeedField final : ui::TextField {
    Tiamat* module;
    explicit SeedField(Tiamat* m) : module(m) { box.size.x=240; setText(std::to_string(m->runtime.snapshot().preset.controls.seed)); placeholder="Decimal or 0x hexadecimal seed"; }
    void onSelectKey(const event::SelectKey& e) override {
        if(e.action==GLFW_PRESS && (e.key==GLFW_KEY_ENTER || e.key==GLFW_KEY_KP_ENTER)) {
            std::uint64_t seed;
            if(tiamat::parseSeed(getText(),seed) && module->runtime.restartRandom(seed)) setText(std::to_string(seed));
            else { setText(""); placeholder="Invalid seed (unsigned 64-bit)"; }
            e.consume(this); return;
        }
        ui::TextField::onSelectKey(e);
    }
};
}
struct TiamatWidget final : ModuleWidget {
    explicit TiamatWidget(Tiamat* module) {
        setModule(module);
        visual_assets::SplitPanelRenderer panel(this,"res/Tiamat.panel.svg");
        panel.addThemedLabels("res/Tiamat.labels.svg", "res/Tiamat.theme-text-input.svg", "res/Tiamat.theme-text-output.svg");
        const auto path=panel.panelPath();
        auto point=[&](const char* id,float x,float y) { Vec value(x,y); panel_svg::loadPointFromSvgMm(path,id,&value); return mm2px(value); };
        const char* knobs[]={"TIME_PARAM","REPEATS_PARAM","MIX_PARAM","BEND_PARAM","BREAK_PARAM","CORRUPT_PARAM"};
        const char* cv[]={"TIME_INPUT","REPEATS_INPUT","MIX_INPUT","BEND_INPUT","BREAK_INPUT","CORRUPT_INPUT"};
        for(int i=0;i<6;++i) {
            addParam(createParamCentered<Eclipse2Knob>(point(knobs[i],15.24f+30.48f*(i%3),25+33*(i/3)),module,i));
            addInput(createInputCentered<PJ301MPort>(point(cv[i],15.24f+30.48f*(i%3),38+33*(i/3)),module,Tiamat::TIME_INPUT+i));
        }
        const char* buttons[]={"CLOCK_BUTTON","MODE_BUTTON","FREEZE_BUTTON","BEND_BUTTON","BREAK_BUTTON","CORRUPT_BUTTON"};
        for(int i=0;i<6;++i) addParam(createParamCentered<LEDButton>(point(buttons[i],8+15*i,84),module,Tiamat::CLOCK_BUTTON+i));
        const char* gates[]={"FREEZE_INPUT","BEND_GATE_INPUT","BREAK_GATE_INPUT","CORRUPT_GATE_INPUT"};
        for(int i=0;i<4;++i) addInput(createInputCentered<PJ301MPort>(point(gates[i],17+19*i,101),module,Tiamat::FREEZE_INPUT+i));
        addInput(createInputCentered<PJ301MPort>(point("LEFT_INPUT",12,115),module,Tiamat::LEFT_INPUT));
        addInput(createInputCentered<PJ301MPort>(point("RIGHT_INPUT",27,115),module,Tiamat::RIGHT_INPUT));
        addInput(createInputCentered<PJ301MPort>(point("CLOCK_INPUT",46,115),module,Tiamat::CLOCK_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(point("LEFT_OUTPUT",66,115),module,Tiamat::LEFT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(point("RIGHT_OUTPUT",81,115),module,Tiamat::RIGHT_OUTPUT));
        const char* lights[]={"EXTERNAL_LIGHT","MICRO_LIGHT","FREEZE_REQUEST_LIGHT","FREEZE_ACTIVE_LIGHT","BEND_LIGHT","BREAK_LIGHT", "DECIMATE_LIGHT","DROPOUT_LIGHT","DESTROY_LIGHT","DJ_LIGHT","VINYL_LIGHT","CLOCK_LOST_LIGHT","CAPACITY_LIGHT","FAULT_LIGHT"};
        const float xs[]={8,23,36,40,53,68,62,67,72,77,82,70,78,86};
        for(int i=0;i<Tiamat::LIGHTS_LEN;++i)
            addChild(createLightCentered<SmallLight<GreenLight>>(point(lights[i],xs[i],i>=11 ? 7 : i>=6 ? 92 : 89),module,i));
    }
    void appendContextMenu(Menu* menu) override {
        ModuleWidget::appendContextMenu(menu);
        auto* m=dynamic_cast<Tiamat*>(module); if(!m) return;
        menu->addChild(new MenuSeparator);
        const char* effects[]={"Decimate","Dropout","Destroy","DJ Filter","Vinyl"};
        auto state=m->runtime.snapshot();
        menu->addChild(createMenuLabel(std::string("Corrupt: ")+effects[std::max(0,std::min(4,int(state.preset.controls.effect)-1))]));
        menu->addChild(new SettingSlider(m,&tiamat::SecondarySettings::window,"Window",.141421356f));
        menu->addChild(new SettingSlider(m,&tiamat::SecondarySettings::separation,"Stereo separation",1));
        menu->addChild(new SettingSlider(m,&tiamat::SecondarySettings::bendDepth,"Bend CV depth",1));
        menu->addChild(new SettingSlider(m,&tiamat::SecondarySettings::breakDepth,"Break CV depth",1));
        menu->addChild(new SettingSlider(m,&tiamat::SecondarySettings::corruptDepth,"Corrupt CV depth",1));
        menu->addChild(createCheckMenuItem("Shared Macro decisions","",[m] { return !m->runtime.snapshot().preset.settings.unique; },[m] { m->runtime.editSettings([](tiamat::SecondarySettings& s) { s.unique=!s.unique; }); }));
        menu->addChild(createCheckMenuItem("Level gates","",[m] { return m->runtime.snapshot().preset.settings.gates==tiamat::GateBehavior::Level; },[m] { m->runtime.editSettings([](tiamat::SecondarySettings& s) { s.gates=s.gates==tiamat::GateBehavior::Level ? tiamat::GateBehavior::Latching : tiamat::GateBehavior::Level; }); }));
        menu->addChild(createCheckMenuItem("Momentary Freeze button","",[m] { return m->runtime.snapshot().preset.settings.freezeButton==tiamat::FreezeButton::Momentary; },[m] { m->runtime.editSettings([](tiamat::SecondarySettings& s) { s.freezeButton=s.freezeButton==tiamat::FreezeButton::Momentary ? tiamat::FreezeButton::Latching : tiamat::FreezeButton::Momentary; }); }));
        menu->addChild(createCheckMenuItem("Corrupt gate resets clock","",[m] { return m->runtime.snapshot().preset.settings.corruptGate==tiamat::CorruptGate::ClockReset; },[m] { m->runtime.editSettings([](tiamat::SecondarySettings& s) { s.corruptGate=s.corruptGate==tiamat::CorruptGate::ClockReset ? tiamat::CorruptGate::Advance : tiamat::CorruptGate::ClockReset; }); }));
        menu->addChild(createCheckMenuItem("Original three effects only","",[m] { return m->runtime.snapshot().preset.settings.effectSet==tiamat::EffectSet::OriginalThree; },[m] { m->runtime.editSettings([](tiamat::SecondarySettings& s) { s.effectSet=s.effectSet==tiamat::EffectSet::OriginalThree ? tiamat::EffectSet::AllFive : tiamat::EffectSet::OriginalThree; }); }));
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("Seed (Enter applies and restarts)")); menu->addChild(new SeedField(m));
        menu->addChild(createMenuItem("Restart random sequence","",[m] { m->runtime.restartRandom(m->runtime.snapshot().preset.controls.seed); }));
        menu->addChild(createMenuItem("Restore secondary defaults","",[m] { m->runtime.restoreSecondary(); }));
        menu->addChild(createMenuLabel("Mix=0 retains delay, Tone and width."));
        menu->addChild(createMenuLabel("Bypass is direct; right input normals to left."));
        if(state.fault) menu->addChild(createMenuItem("Recover with cleared engine","",[m] { m->runtime.reset(m->runtime.snapshot().preset); }));
    }
};
Model* modelTiamat=createModel<Tiamat,TiamatWidget>("Tiamat");

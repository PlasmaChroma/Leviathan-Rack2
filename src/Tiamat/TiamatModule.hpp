#pragma once
#include "../plugin.hpp"
#include "TiamatRuntime.hpp"

struct Tiamat final : Module {
    enum ParamId { TIME_PARAM, REPEATS_PARAM, MIX_PARAM, BEND_PARAM, BREAK_PARAM, CORRUPT_PARAM,
        CLOCK_BUTTON, MODE_BUTTON, FREEZE_BUTTON, BEND_BUTTON, BREAK_BUTTON, CORRUPT_BUTTON, PARAMS_LEN };
    enum InputId { LEFT_INPUT, RIGHT_INPUT, TIME_INPUT, REPEATS_INPUT, MIX_INPUT, BEND_INPUT, BREAK_INPUT,
        CORRUPT_INPUT, CLOCK_INPUT, FREEZE_INPUT, BEND_GATE_INPUT, BREAK_GATE_INPUT, CORRUPT_GATE_INPUT, INPUTS_LEN };
    enum OutputId { LEFT_OUTPUT, RIGHT_OUTPUT, OUTPUTS_LEN };
    enum LightId { EXTERNAL_LIGHT, MICRO_LIGHT, FREEZE_REQUEST_LIGHT, FREEZE_ACTIVE_LIGHT, BEND_LIGHT, BREAK_LIGHT,
        DECIMATE_LIGHT, DROPOUT_LIGHT, DESTROY_LIGHT, DJ_LIGHT, VINYL_LIGHT, CLOCK_LOST_LIGHT, CAPACITY_LIGHT, FAULT_LIGHT, LIGHTS_LEN };
    tiamat::Runtime runtime;
    Tiamat();
    void process(const ProcessArgs&) override;
    void processBypass(const ProcessArgs&) override;
    void onReset(const ResetEvent&) override;
    json_t* dataToJson() override;
    void dataFromJson(json_t*) override;
private:
    std::array<bool,6> buttonHigh_ {};
    std::uint64_t buttonGeneration_ = 0;
};
extern Model* modelTiamat;

#pragma once

#include "plugin.hpp"

struct VTune final : Module {
    enum ParamId {
        VELOCITY_PARAM,
        SPEED_PARAM,
        PRESSURE_PARAM,
        SUSTAIN_PARAM,
        IMPERFECTION_PARAM,
        WIDTH_PARAM,
        LEVEL_PARAM,
        PARAMS_LEN
    };
    enum InputId { INPUTS_LEN };
    enum OutputId { OUTPUTS_LEN };
    enum LightId { LIGHTS_LEN };

    VTune();
    void process(const ProcessArgs& args) override;
};

extern Model* modelVTune;

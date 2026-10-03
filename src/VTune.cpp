#include "VTune.hpp"
#include "VesselExpanderProtocol.hpp"
#include "vessel/PitchColorMap.hpp"

#include <algorithm>
#include <cmath>

namespace {
float finiteBound(float value, float low, float high, float fallback) {
    return std::isfinite(value) ? std::max(low, std::min(high, value)) : fallback;
}

bool isVessel(const Module* module) {
    return module && module->model == modelVessel;
}
}

VTune::VTune() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
    for (int i = 0; i < vessel_pitch_color::count; ++i)
        configLight(CHAKRA_ROOT_LIGHT + i, vessel_pitch_color::names[i]);
    configParam(VELOCITY_PARAM, 0.f, 1.f, .5f, "Strike velocity", "%", 0.f, 100.f);
    configParam(SPEED_PARAM, 0.f, 2.f, .4f, "Rubbing speed", " rev/s");
    configParam(PRESSURE_PARAM, 0.f, 15.f, 2.5f, "Contact pressure", " N");
    configParam(SUSTAIN_PARAM, -2.f, 2.f, 0.f, "Sustain", "x", 2.f);
    configParam(IMPERFECTION_PARAM, 0.f, 2.f, 1.f, "Mode-pair imperfection");
    configParam(WIDTH_PARAM, 0.f, 1.f, .7f, "Stereo width", "%", 0.f, 100.f);
    configParam(LEVEL_PARAM, 0.f, 2.f, 1.f, "Output level", "%", 0.f, 100.f);
}

void VTune::process(const ProcessArgs& args) {
    updateBodyMapFrequency(args.sampleTime);
    Module* vessel = leftExpander.module;
    const bool linked = isVessel(vessel) && vessel->rightExpander.module == this;
    const bool ready = linked && vessel->rightExpander.producerMessage;
    lights[VESSEL_LINK_LIGHT].setBrightness(linked && !ready ? 1.f : 0.f);
    lights[VESSEL_READY_LIGHT].setBrightness(ready ? 1.f : 0.f);
    if (!ready) {
        return;
    }

    auto* message = reinterpret_cast<vessel_expander::TuneMessage*>(vessel->rightExpander.producerMessage);
    message->magic = vessel_expander::kMagic;
    message->version = vessel_expander::kVersion;
    message->velocity = finiteBound(params[VELOCITY_PARAM].getValue(), 0.f, 1.f, .5f);
    message->speed = finiteBound(params[SPEED_PARAM].getValue(), 0.f, 2.f, .4f);
    message->pressure = finiteBound(params[PRESSURE_PARAM].getValue(), 0.f, 15.f, 2.5f);
    message->sustain = finiteBound(params[SUSTAIN_PARAM].getValue(), -2.f, 2.f, 0.f);
    message->imperfection = finiteBound(params[IMPERFECTION_PARAM].getValue(), 0.f, 2.f, 1.f);
    message->width = finiteBound(params[WIDTH_PARAM].getValue(), 0.f, 1.f, .7f);
    message->level = 1.f; // Reserved legacy field; output level now lives on Vessel.
    vessel->rightExpander.messageFlipRequested = true;
}

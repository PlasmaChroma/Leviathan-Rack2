#pragma once

#include "plugin.hpp"
#include "vtune/BodyMapData.hpp"
#include <atomic>

struct VTune final : Module {
    enum ParamId {
        VELOCITY_PARAM,
        SPEED_PARAM,
        PRESSURE_PARAM,
        SUSTAIN_PARAM,
        IMPERFECTION_PARAM,
        WIDTH_PARAM,
        LEVEL_PARAM, // Retired; retain the saved parameter ID.
        PARAMS_LEN
    };
    enum InputId { INPUTS_LEN };
    enum OutputId { OUTPUTS_LEN };
    enum LightId { VESSEL_LINK_LIGHT, VESSEL_READY_LIGHT,
        CHAKRA_ROOT_LIGHT, CHAKRA_SACRAL_LIGHT, CHAKRA_SOLAR_LIGHT,
        CHAKRA_HEART_LIGHT, CHAKRA_THROAT_LIGHT, CHAKRA_THIRD_EYE_LIGHT,
        CHAKRA_CROWN_LIGHT, LIGHTS_LEN };
    std::atomic<float> bodyFrequencyHz {0.f};
    std::atomic<int> bodyMapMode {0};
    std::atomic<float> bodyMapOpacity {vtune_body::kDefaultOpacity};
    float bodyMapPublishElapsed = 1.f;
    Module* adoptedVessel = nullptr;

    VTune();
    void process(const ProcessArgs& args) override;
    void adoptVesselSettings();
    void updateBodyMapFrequency(float sampleTime);
    void processBypass(const ProcessArgs& args) override;
    void onExpanderChange(const ExpanderChangeEvent& e) override;
    void onReset(const ResetEvent& e) override;
    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
};

extern Model* modelVTune;

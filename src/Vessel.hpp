#pragma once

#include "plugin.hpp"
#include "DebugTerminalMetrics.hpp"
#include "VesselExpanderProtocol.hpp"
#include "vessel/DualBowlAdapter.hpp"
#include "vessel/RubIntensity.hpp"
#include <atomic>

struct Vessel final : Module {
    static constexpr float strikeApproachSeconds = .035f;
    static constexpr float strikeReboundSeconds = .50f;
    // Stable IDs: additions append, even while the module is a prototype.
    enum ParamId { PITCH_PARAM, FINE_PARAM, VELOCITY_PARAM, STRIKE_PARAM,
        ROTATE_PARAM, SPEED_PARAM, PRESSURE_PARAM, BOWL_PARAM, MALLET_PARAM,
        DECAY_PARAM, IMPERFECTION_PARAM, WIDTH_PARAM, LEVEL_PARAM, BINAURAL_PARAM, PARAMS_LEN };
    enum InputId { VOCT_INPUT, STRIKE_INPUT, VELOCITY_INPUT, ROTATE_INPUT,
        SPEED_INPUT, PRESSURE_INPUT, INTENSITY_INPUT, INPUTS_LEN }; // Speed/pressure inputs are retired; retain IDs.
    enum OutputId { LEFT_OUTPUT, RIGHT_OUTPUT, OUTPUTS_LEN };
    enum LightId {
        STRIKE_LIGHT,
        ROTATE_LIGHT,
        FAULT_LIGHT,
        VTUNE_LINK_LIGHT,
        VTUNE_READY_LIGHT,
        LIGHTS_LEN
    };

    vessel::DualBowlAdapter audio;
    debug_terminal::BaselineModuleMetrics debugMetrics;
    std::atomic<float> visualEnergy {0.f}, visualFrequency {261.625565f};
    std::atomic<float> visualRotationAngle {0.f};
    std::atomic<float> visualStrikeAftermath {0.f};
    std::atomic<float> rawEnergy {0.f};
    std::atomic<float> visualSeparation {0.f}, leftEnergy {0.f}, rightEnergy {0.f};
    std::atomic<bool> visualFault {false}, visualSleeping {true}, visualRubbing {false};
    std::atomic<bool> pendingReset {true};
    std::atomic<int> requestedQuality {int(vessel::ProcessingQuality::Balanced)};
    std::atomic<float> visualInternalRate {0.f};
    std::atomic<bool> visualRateFallback {false};
    std::atomic<float> manualStrikeVelocity {1.f};
    std::atomic<float> manualRubIntensity {1.f}; // Normalized coordinated pad gesture.
    vessel_expander::TuneMessage tuneMessages[2];

    Vessel();
    void process(const ProcessArgs& args) override;
    void onReset(const ResetEvent& event) override;
    void onExpanderChange(const ExpanderChangeEvent& event) override;
    void processBypass(const ProcessArgs& args) override;
    json_t* dataToJson() override;
    void dataFromJson(json_t* root) override;
    json_t* paramsToJson() override;
    void paramsFromJson(json_t* root) override;
private:
    vessel::BowlDescriptor currentBowl {}, startBowl {};
    vessel::MalletDescriptor currentMallet {}, startMallet {};
    vessel::EngineSettings applied;
    int selectedBowl = 0, selectedMallet = 1;
    double bowlBlend = 1.0, malletBlend = 1.0;
    double pitchOctaves = 0.0, controlElapsed = 0.001, visualElapsed = 0.0;
    double separationHz = 0.0;
    double intensityMaximumSpeed = 0.0;
    double intensitySlipScale = 1.2, startIntensitySlipScale = 1.2;
    vessel::RubIntensityPlayer intensityPlayer;
    double intensityStartSpeed = 0, intensityEnergy = 0;
    double idleElapsed = 0.0;
    float meter = 0.f, strikeFlash = 0.f;
    float strikeAnimationRemaining = 0.f;
    bool strikeHigh = false, manualHigh = false, rotateHigh = false;
    bool sleeping = true, configured = false, needsConfigure = true;
    double hostRate = 0.0;
    vessel::ProcessingQuality activeQuality = vessel::ProcessingQuality::Balanced;
    void resetRuntime();
    vessel_expander::TuneMessage tuneControls();
    void updateControls(double elapsed, const vessel_expander::TuneMessage& tune);
    bool configureAudio(double rate);
};

extern Model* modelVessel;

#pragma once

#include "plugin.hpp"
#include <memory>

struct CartridgeVisualStyle {
  NVGcolor shellFill;
  NVGcolor shellStroke;
  NVGcolor holeFill;
};

struct TemporalDeck final : Module {
  static constexpr int CARTRIDGE_CLEAN = 0;
  static constexpr int CARTRIDGE_M44_7 = 1;
  static constexpr int CARTRIDGE_ORTOFON_SCRATCH = 2;
  static constexpr int CARTRIDGE_STANTON_680HP = 3;
  static constexpr int CARTRIDGE_LOFI = 4;
  static constexpr int CARTRIDGE_COUNT = 5;

  static constexpr int SCRATCH_MODEL_LEGACY = 0;
  static constexpr int SCRATCH_MODEL_HYBRID = 1;
  static constexpr int SCRATCH_MODEL_SCRATCH3 = 2;
  static constexpr int SCRATCH_MODEL_COUNT = 3;

  static constexpr int BUFFER_DURATION_8S = 0;
  static constexpr int BUFFER_DURATION_16S = 1;
  static constexpr int BUFFER_DURATION_8M = 2;
  static constexpr int BUFFER_DURATION_COUNT = 3;

  static constexpr float kNominalPlatterRpm = 33.3333f;
  static constexpr float kMouseScratchTravelScale = 1.25f;
  static constexpr float kWheelScratchTravelScale = 1.10f;

  static constexpr float kUiPublishRateHz = 120.f;
  static constexpr float kUiPublishIntervalSec = 1.f / kUiPublishRateHz;
  static constexpr int kArcLightCount = 31;

  enum ParamId {
    BUFFER_PARAM,
    RATE_PARAM,
    SCRATCH_SENSITIVITY_PARAM,
    MIX_PARAM,
    FEEDBACK_PARAM,
    FREEZE_PARAM,
    REVERSE_PARAM,
    SLIP_PARAM,
    CARTRIDGE_CYCLE_PARAM,
    PARAMS_LEN
  };
  enum InputId {
    POSITION_CV_INPUT,
    RATE_CV_INPUT,
    INPUT_L_INPUT,
    INPUT_R_INPUT,
    SCRATCH_GATE_INPUT,
    FREEZE_GATE_INPUT,
    INPUTS_LEN
  };
  enum OutputId { OUTPUT_L_OUTPUT, OUTPUT_R_OUTPUT, OUTPUTS_LEN };
  enum LightId {
    FREEZE_LIGHT,
    REVERSE_LIGHT,
    SLIP_LIGHT,
    ARC_LIGHT_START,
    ARC_MAX_LIGHT_START = ARC_LIGHT_START + kArcLightCount,
    LIGHTS_LEN = ARC_MAX_LIGHT_START + kArcLightCount
  };

  TemporalDeck();
  ~TemporalDeck() override;

  static const char *cartridgeLabelFor(int index);
  static CartridgeVisualStyle cartridgeVisualStyleFor(int index);
  static const char *scratchModelLabelFor(int index);
  static const char *bufferDurationLabelFor(int index);

  void onSampleRateChange() override;
  json_t *dataToJson() override;
  void dataFromJson(json_t *root) override;
  void process(const ProcessArgs &args) override;

  float scratchSensitivity();
  void applyBufferDurationMode(int mode);

  void setPlatterScratch(bool touched, float lagSamples, float velocitySamples, int holdSamples = 0);
  void setPlatterMotionFreshSamples(int motionFreshSamples);
  void addPlatterWheelDelta(float delta, int holdSamples);

  double getUiLagSamples() const;
  double getUiAccessibleLagSamples() const;
  float getUiSampleRate() const;
  float getUiPlatterAngle() const;

  bool isSlipLatched() const;
  int getCartridgeCharacter() const;

  int getScratchModel() const;
  void setScratchModel(int mode);

  int getBufferDurationMode() const;

  bool isPlatterCursorLockEnabled() const;
  void setPlatterCursorLockEnabled(bool enabled);

  bool isHighQualityScratchInterpolationEnabled() const;
  void setHighQualityScratchInterpolationEnabled(bool enabled);

private:
  struct Impl;
  std::unique_ptr<Impl> impl;
};

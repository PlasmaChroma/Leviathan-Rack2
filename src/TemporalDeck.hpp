#pragma once

#include "TemporalDeckTest.hpp"
#include "plugin.hpp"
#include <memory>
#include <string>

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
  static constexpr int CARTRIDGE_QBERT = 4;
  static constexpr int CARTRIDGE_LOFI = 5;
  static constexpr int CARTRIDGE_COUNT = 6;

  static constexpr int SCRATCH_INTERP_CUBIC = 0;
  static constexpr int SCRATCH_INTERP_LAGRANGE6 = 1;
  static constexpr int SCRATCH_INTERP_SINC = 2;
  static constexpr int SCRATCH_INTERP_COUNT = 3;

  static constexpr int SLIP_RETURN_SLOW = 0;
  static constexpr int SLIP_RETURN_NORMAL = 1;
  static constexpr int SLIP_RETURN_INSTANT = 2;
  static constexpr int SLIP_RETURN_COUNT = 3;

  static constexpr int BUFFER_DURATION_10S = 0;
  static constexpr int BUFFER_DURATION_20S = 1;
  static constexpr int BUFFER_DURATION_10M_STEREO = 2;
  static constexpr int BUFFER_DURATION_10M_MONO = 3;
  static constexpr int BUFFER_DURATION_COUNT = 4;

  static constexpr int SAMPLE_SOURCE_LIVE = 0;
  static constexpr int SAMPLE_SOURCE_FILE = 1;

  static constexpr float kNominalPlatterRpm = 33.3333f;
  static constexpr float kMouseScratchTravelScale = 1.00f;
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
    SLIP_SLOW_LIGHT,
    SLIP_LIGHT,
    SLIP_FAST_LIGHT,
    ARC_LIGHT_START,
    ARC_MAX_LIGHT_START = ARC_LIGHT_START + kArcLightCount,
    LIGHTS_LEN = ARC_MAX_LIGHT_START + kArcLightCount
  };

  TemporalDeck();
  ~TemporalDeck() override;

  static const char *cartridgeLabelFor(int index);
  static CartridgeVisualStyle cartridgeVisualStyleFor(int index);
  static const char *scratchInterpolationLabelFor(int index);
  static const char *slipReturnLabelFor(int index);
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
  void triggerQuickSlipReturn();

  double getUiLagSamples() const;
  double getUiAccessibleLagSamples() const;
  float getUiSampleRate() const;
  float getUiPlatterAngle() const;
  bool isUiFreezeLatched() const;
  bool isSampleModeEnabled() const;
  bool hasLoadedSample() const;
  bool isSampleAutoPlayOnLoadEnabled() const;
  void setSampleAutoPlayOnLoadEnabled(bool enabled);
  void setSampleModeEnabled(bool enabled);
  bool isSampleTransportPlaying() const;
  void setSampleTransportPlaying(bool enabled);
  bool isSampleLoopEnabled() const;
  void setSampleLoopEnabled(bool enabled);
  void stopSampleTransport();
  void clearLoadedSample();
  bool loadSampleFromPath(const std::string &path, std::string *errorOut = nullptr);
  void seekSampleByNormalizedPosition(double normalized);
  double getUiSamplePlayheadSeconds() const;
  double getUiSampleDurationSeconds() const;
  double getUiSampleProgress() const;
  std::string getLoadedSampleDisplayName() const;
  bool wasLoadedSampleTruncated() const;

  bool isSlipLatched() const;
  int getCartridgeCharacter() const;

  int getBufferDurationMode() const;
  bool isBufferModeMono() const;

  bool isPlatterCursorLockEnabled() const;
  void setPlatterCursorLockEnabled(bool enabled);
  bool isFreezeTraceLoggingEnabled() const;
  void setFreezeTraceLoggingEnabled(bool enabled);

  bool isHighQualityScratchInterpolationEnabled() const;
  void setHighQualityScratchInterpolationEnabled(bool enabled);
  int getScratchInterpolationMode() const;
  void setScratchInterpolationMode(int mode);
  void setSlipLatched(bool enabled);
  int getSlipReturnMode() const;
  void setSlipReturnMode(int mode);

private:
  void applySampleRateChange(float sampleRate);

  struct Impl;
  std::unique_ptr<Impl> impl;
};

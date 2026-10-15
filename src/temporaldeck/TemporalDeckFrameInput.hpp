#pragma once

#include "TemporalDeckEngine.hpp"
#include "TemporalDeckPlatterInput.hpp"

namespace temporaldeck_frameinput {

struct SignalInputs {
  float inL = 0.f;
  float inR = 0.f;
  float positionCv = 0.f;
  float rateCv = 0.f;
  bool rateCvConnected = false;
  bool freezeGateHigh = false;
  bool scratchGateHigh = false;
  bool scratchGateConnected = false;
  bool positionConnected = false;
};

struct FrameInputControls {
  float dt = 0.f;
  float bufferKnob = 0.f;
  float rateKnob = 0.f;
  float mixKnob = 0.f;
  float feedbackKnob = 0.f;
  bool freezeButton = false;
  bool reverseButton = false;
  bool slipButton = false;
};

temporaldeck::TemporalDeckEngine::FrameInput buildFrameInput(const SignalInputs &signals,
                                                             const FrameInputControls &controls,
                                                             const temporaldeck::PlatterInputSnapshot &platterInput);

} // namespace temporaldeck_frameinput

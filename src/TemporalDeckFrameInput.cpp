#include "TemporalDeckFrameInput.hpp"

namespace temporaldeck_frameinput {

temporaldeck::TemporalDeckEngine::FrameInput buildFrameInput(const SignalInputs &signals,
                                                             const FrameInputControls &controls,
                                                             const temporaldeck::PlatterInputSnapshot &platterInput) {
  temporaldeck::TemporalDeckEngine::FrameInput frameInput;
  frameInput.dt = controls.dt;
  frameInput.inL = signals.inL;
  frameInput.inR = signals.inR;
  frameInput.bufferKnob = controls.bufferKnob;
  frameInput.rateKnob = controls.rateKnob;
  frameInput.mixKnob = controls.mixKnob;
  frameInput.feedbackKnob = controls.feedbackKnob;
  frameInput.freezeButton = controls.freezeButton;
  frameInput.reverseButton = controls.reverseButton;
  frameInput.slipButton = controls.slipButton;
  frameInput.quickSlipTrigger = platterInput.quickSlipTrigger;
  frameInput.freezeGate = signals.freezeGateHigh;
  frameInput.scratchGate = signals.scratchGateHigh;
  frameInput.scratchGateConnected = signals.scratchGateConnected;
  frameInput.positionConnected = signals.positionConnected;
  frameInput.positionCv = signals.positionCv;
  frameInput.rateCv = signals.rateCv;
  frameInput.rateCvConnected = signals.rateCvConnected;
  frameInput.platterTouched = platterInput.platterTouched;
  frameInput.platterTouchHoldDirect = platterInput.platterTouchHoldDirect;
  frameInput.wheelScratchHeld = platterInput.wheelScratchHeld;
  frameInput.platterMotionActive = platterInput.platterMotionActive;
  frameInput.platterGestureRevision = platterInput.platterGestureRevision;
  frameInput.platterLagTarget = platterInput.platterLagTarget;
  frameInput.platterGestureVelocity = platterInput.platterGestureVelocity;
  frameInput.wheelDelta = platterInput.wheelDelta;
  return frameInput;
}

} // namespace temporaldeck_frameinput

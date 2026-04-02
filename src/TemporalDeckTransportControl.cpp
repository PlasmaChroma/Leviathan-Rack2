#include "TemporalDeckTransportControl.hpp"

#include <algorithm>

namespace {

double clampd(double x, double a, double b) {
  return std::max(a, std::min(x, b));
}

} // namespace

namespace temporaldeck_transport {

TransportButtonResult applyTransportButtonEvents(TransportControlState &state, const TransportButtonEvents &events,
                                                 bool sampleModeEnabled, bool sampleLoaded) {
  TransportButtonResult result;

  if (events.freezePressed) {
    bool next = !state.freezeLatched;
    state.freezeLatched = next;
    state.freezeLatchedByButton = next;
    if (next) {
      state.reverseLatched = false;
      state.slipLatched = false;
    }
  }

  if (events.reversePressed) {
    bool next = !state.reverseLatched;
    state.reverseLatched = next;
    if (next) {
      state.freezeLatched = false;
      state.freezeLatchedByButton = false;
      state.slipLatched = false;
      if (sampleModeEnabled && sampleLoaded) {
        result.forceSampleTransportPlay = true;
      }
    }
  }

  if (events.slipPressed) {
    if (!state.slipLatched) {
      state.slipLatched = true;
      state.slipReturnMode = temporaldeck::TemporalDeckEngine::SLIP_RETURN_SLOW;
      state.freezeLatched = false;
      state.freezeLatchedByButton = false;
      state.reverseLatched = false;
    } else if (state.slipReturnMode == temporaldeck::TemporalDeckEngine::SLIP_RETURN_SLOW) {
      state.slipReturnMode = temporaldeck::TemporalDeckEngine::SLIP_RETURN_NORMAL;
      state.freezeLatched = false;
      state.freezeLatchedByButton = false;
      state.reverseLatched = false;
    } else if (state.slipReturnMode == temporaldeck::TemporalDeckEngine::SLIP_RETURN_NORMAL) {
      state.slipReturnMode = temporaldeck::TemporalDeckEngine::SLIP_RETURN_INSTANT;
      state.freezeLatched = false;
      state.freezeLatchedByButton = false;
      state.reverseLatched = false;
    } else {
      state.slipLatched = false;
    }
  }

  return result;
}

void applyFreezeGateEdge(TransportControlState &state, bool freezeGateHigh) {
  bool freezeGateFallingEdge = state.prevFreezeGateHigh && !freezeGateHigh;
  state.prevFreezeGateHigh = freezeGateHigh;
  if (freezeGateFallingEdge && state.freezeLatched && state.freezeLatchedByButton) {
    state.freezeLatched = false;
    state.freezeLatchedByButton = false;
  }
}

void applyAutoFreezeRequest(TransportControlState &state, bool autoFreezeRequested, bool freezeGateHigh) {
  if (autoFreezeRequested && !state.freezeLatched && !freezeGateHigh) {
    state.freezeLatched = true;
    state.freezeLatchedByButton = false;
    state.reverseLatched = false;
    state.slipLatched = false;
  }
}

uint32_t applyPendingSampleSeek(temporaldeck::TemporalDeckEngine &engine, uint32_t appliedRevision,
                                uint32_t pendingRevision, float pendingNormalized, float bufferKnob) {
  if (pendingRevision == appliedRevision) {
    return appliedRevision;
  }

  float seekNorm = std::max(0.f, std::min(pendingNormalized, 1.f));
  if (engine.sampleLoaded && engine.sampleFrames > 0) {
    double sampleEndPos = std::max(0.0, double(engine.sampleFrames - 1));
    double sampleWindowEndPos = sampleEndPos * double(std::max(0.f, std::min(bufferKnob, 1.f)));
    // Arc seek maps to full sample time and then clamps to active window end.
    double targetFrame = clampd(double(seekNorm) * sampleEndPos, 0.0, sampleWindowEndPos);
    engine.samplePlayhead = targetFrame;
    engine.readHead = targetFrame;
    engine.scratchLagSamples = 0.0;
    engine.scratchLagTargetSamples = 0.0;
    engine.nowCatchActive = false;
    engine.cancelSlipReturnState();
  }

  return pendingRevision;
}

} // namespace temporaldeck_transport

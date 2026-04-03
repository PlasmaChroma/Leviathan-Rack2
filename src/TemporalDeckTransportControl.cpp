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

uint32_t applyPendingLiveSeekArc(temporaldeck::TemporalDeckEngine &engine, uint32_t appliedRevision,
                                 uint32_t pendingRevision, float pendingArcNormalized, float bufferKnob) {
  if (pendingRevision == appliedRevision) {
    return appliedRevision;
  }

  if (engine.sampleModeEnabled && engine.sampleLoaded) {
    return pendingRevision;
  }
  if (engine.buffer.size <= 0 || engine.buffer.filled <= 0) {
    return pendingRevision;
  }

  double maxLag = std::max(0.0, engine.maxLagFromKnob(1.f));
  double limitLag = std::max(0.0, engine.accessibleLag(bufferKnob));
  float arcNorm = std::max(0.f, std::min(pendingArcNormalized, 1.f));
  // Arc seek maps across full arc range and clamps to current live limit.
  double targetLag = clampd(double(arcNorm) * maxLag, 0.0, limitLag);
  double newestPos = engine.newestReadablePos();
  double targetRead = newestPos - targetLag;
  engine.readHead = engine.buffer.wrapPosition(targetRead);
  engine.scratchLagSamples = targetLag;
  engine.scratchLagTargetSamples = targetLag;
  engine.filteredManualLagTargetSamples = targetLag;
  engine.liveManualScratchAnchorNewestPos = newestPos;
  engine.liveManualScratchAnchorLagSamples = targetLag;
  engine.scratchHandVelocity = 0.f;
  engine.scratchMotionVelocity = 0.f;
  engine.scratch3LagVelocity = 0.f;
  engine.nowCatchActive = false;
  engine.cancelSlipReturnState();
  return pendingRevision;
}

} // namespace temporaldeck_transport

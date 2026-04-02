#pragma once

#include "TemporalDeckEngine.hpp"

#include <cstdint>

namespace temporaldeck_transport {

struct TransportControlState {
  bool freezeLatched = false;
  bool freezeLatchedByButton = false;
  bool reverseLatched = false;
  bool slipLatched = false;
  bool prevFreezeGateHigh = false;
  int slipReturnMode = 1; // normal
};

struct TransportButtonEvents {
  bool freezePressed = false;
  bool reversePressed = false;
  bool slipPressed = false;
};

struct TransportButtonResult {
  bool forceSampleTransportPlay = false;
};

TransportButtonResult applyTransportButtonEvents(TransportControlState &state, const TransportButtonEvents &events,
                                                 bool sampleModeEnabled, bool sampleLoaded);

void applyFreezeGateEdge(TransportControlState &state, bool freezeGateHigh);

void applyAutoFreezeRequest(TransportControlState &state, bool autoFreezeRequested, bool freezeGateHigh);

uint32_t applyPendingSampleSeek(temporaldeck::TemporalDeckEngine &engine, uint32_t appliedRevision,
                                uint32_t pendingRevision, float pendingNormalized, float bufferKnob);

} // namespace temporaldeck_transport

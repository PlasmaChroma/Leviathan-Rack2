#pragma once

#include <cstdint>

namespace debug_terminal {

void submitTDScopeUiMetrics(uint32_t instanceId,
                            float uiMs,
                            int rows,
                            float densityPct,
                            float zoom,
                            float thickness,
                            uint64_t publishSeq,
                            uint64_t drawSeq,
                            uint64_t drawCalls);

void submitTemporalDeckUiMetrics(uint32_t instanceId,
                                 float uiMs,
                                 float audioUs,
                                 float scopePreviewUs,
                                 int scopeStride,
                                 bool scopeMetricValid);

void submitBifurxUiMetrics(uint32_t instanceId,
                           float uiMs,
                           float uiDrawMs,
                           float uiSyncMs,
                           float uiLocalPrepMs,
                           bool renderOpengl,
                           float audioUs,
                           float curvePrepUs,
                           float overlayPrepUs,
                           int visualWorkerMode,
                           float visualWorkerAgeMs,
                           float visualWorkerQueueMs);

void submitWyrmMetrics(uint32_t instanceId,
                       float uiMs,
                       float editorDrawUs,
                       float sandUpdateUs,
                       float sandDrawUs,
                       float sandGlUs,
                       float audioUs,
                       int channels,
                       int bodySamples,
                       uint64_t bodySampleCacheHits,
                       uint64_t bodySampleCacheMisses);

void submitIntegralFluxMetrics(uint32_t instanceId,
                               float uiMs,
                               float audioUs);

} // namespace debug_terminal

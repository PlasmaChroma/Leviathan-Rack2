#pragma once

#include <cstdint>

namespace debug_terminal {

void submitTDScopeUiMetrics(uint32_t instanceId,
                            float processUs,
                            float stepUs,
                            float drawUs,
                            int rows,
                            float densityPct,
                            float zoom,
                            float thickness,
                            uint64_t publishSeq,
                            uint64_t drawSeq,
                            uint64_t drawCalls);

void submitTemporalDeckUiMetrics(uint32_t instanceId,
                                 float processUs,
                                 float stepUs,
                                 float drawUs,
                                 float scopePreviewUs,
                                 int scopeStride,
                                 bool scopeMetricValid);

void submitBifurxUiMetrics(uint32_t instanceId,
                           float processUs,
                           float stepUs,
                           float drawUs,
                           float uiLocalPrepUs,
                           bool renderOpengl,
                           float curvePrepUs,
                           float overlayPrepUs,
                           int visualWorkerMode,
                           float visualWorkerAgeMs,
                           float visualWorkerQueueMs);

void submitWyrmMetrics(uint32_t instanceId,
                       float processUs,
                       float stepUs,
                       float drawUs,
                       float editorDrawUs,
                       float sandUpdateUs,
                       float sandDrawUs,
                       float sandGlUs,
                       int channels,
                       int bodySamples,
                       uint64_t bodySampleCacheHits,
                       uint64_t bodySampleCacheMisses);

void submitIntegralFluxMetrics(uint32_t instanceId,
                               float processUs,
                               float stepUs,
                               float drawUs,
                               float gearUs,
                               float eclipseUs,
                               float eclipseShadowUs,
                               uint64_t eclipseShadowDraws);

void submitProcMetrics(uint32_t instanceId,
                       float processUs,
                       float stepUs,
                       float drawUs);

void submitUndertowMetrics(uint32_t instanceId,
                           float processUs,
                           float stepUs,
                           float drawUs);

} // namespace debug_terminal

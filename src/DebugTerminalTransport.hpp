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
                                 float scopePreviewUs,
                                 int scopeStride,
                                 bool scopeMetricValid);

void submitBifurxUiMetrics(uint32_t instanceId,
                           float uiMs,
                           int filterMode,
                           bool renderOpengl,
                           float audioUs,
                           float curvePrepUs,
                           float overlayPrepUs);

void submitWyrmMetrics(uint32_t instanceId,
                       float uiMs,
                       float editorDrawUs,
                       float sandUpdateUs,
                       float sandDrawUs,
                       float audioUs,
                       int channels,
                       int pointCount,
                       int rockCount,
                       bool sandEnabled,
                       int bodySamples,
                       bool fmConnected,
                       bool foldActive,
                       bool slitherActive,
                       bool lfoMode,
                       bool wavetableRebuilt);

} // namespace debug_terminal

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
                           uint32_t previewSeq,
                           uint32_t analysisSeq,
                           uint64_t drawVertexCount,
                           float curvePrepUs,
                           float overlayPrepUs);

} // namespace debug_terminal

#pragma once

#include <cstdint>

namespace debug_terminal {

void submitTDScopeUiMetrics(uint32_t instanceId,
                            float uiMs,
                            int rows,
                            float densityPct,
                            float zoom,
                            float thickness,
                            uint64_t misses);

void submitTemporalDeckUiMetrics(uint32_t instanceId,
                                 float uiMs,
                                 float scopePreviewUs,
                                 int scopeStride,
                                 bool scopeMetricValid);

} // namespace debug_terminal

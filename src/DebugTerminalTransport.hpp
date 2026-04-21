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

} // namespace debug_terminal

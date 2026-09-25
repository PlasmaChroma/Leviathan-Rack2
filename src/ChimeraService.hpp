#pragma once

#include "ChimeraJobs.hpp"
#include <memory>

namespace chimera {

class RateBridge;
struct RateBridgeSlot {
    std::atomic<unsigned>* requested;
    std::atomic<unsigned>* active;
    std::atomic<RateBridge*>* prepared;
    std::atomic<RateBridge*>* retired;
    std::atomic<bool>* error;
};
// Register only from the non-realtime control dispatcher. A single shared
// worker then follows rate changes even while the Rack widget is hidden.
#ifdef CHIMERA_RATE_SERVICE_TEST_HOOKS
void setRatePreparationHook(void (*hook)(unsigned));
#endif
void registerRateBridge(const RateBridgeSlot& slot);
void unregisterRateBridge(std::atomic<RateBridge*>* prepared);

// Non-realtime callers only. No workers are started until first use.
std::shared_ptr<IoService> chimeraIoService();
void shutdownChimeraIoService();

} // namespace chimera

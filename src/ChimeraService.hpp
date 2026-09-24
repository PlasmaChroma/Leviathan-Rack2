#pragma once

#include "ChimeraJobs.hpp"
#include <memory>

namespace chimera {

// Non-realtime callers only. No workers are started until first use.
std::shared_ptr<IoService> chimeraIoService();
void shutdownChimeraIoService();

} // namespace chimera

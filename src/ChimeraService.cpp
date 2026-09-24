#include "ChimeraService.hpp"
#include <mutex>

namespace chimera {
namespace {
std::mutex serviceMutex;
std::shared_ptr<IoService> service;
bool closing = false;
}

std::shared_ptr<IoService> chimeraIoService() {
    std::lock_guard<std::mutex> lock(serviceMutex);
    if (closing) return std::shared_ptr<IoService>();
    if (!service) service.reset(new IoService(2));
    return service;
}

void shutdownChimeraIoService() {
    std::shared_ptr<IoService> local;
    {
        std::lock_guard<std::mutex> lock(serviceMutex);
        closing = true;
        local.swap(service);
    }
    if (local) local->shutdown(); // Join off audio and outside the global lock.
}

} // namespace chimera

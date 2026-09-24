#include "ChimeraService.hpp"
#include <cstdio>
#include <cstdlib>

static void need(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}
int main() {
    std::shared_ptr<chimera::IoService> first = chimera::chimeraIoService();
    std::shared_ptr<chimera::IoService> second = chimera::chimeraIoService();
    need(first && first == second, "one lazy plugin-scoped service");
    std::shared_ptr<chimera::JobGeneration> token(new chimera::JobGeneration);
    need(first->prepare(token, 1, 1, 1) == chimera::IoService::Accepted,
         "service accepts prepare before shutdown");
    chimera::shutdownChimeraIoService();
    need(!chimera::chimeraIoService(), "shutdown closes lazy entry point");
    chimera::IoService::Result result;
    need(first->poll(result) && result.status == chimera::IoService::Ready &&
         result.prepared && first->outstanding() == 0,
         "shutdown joins worker and leaves completion available off audio");
    need(first->prepare(token, 2, 1, 1) == chimera::IoService::Closed,
         "closed service rejects new work");
    first.reset(); second.reset();
    std::puts("PASS: Chimera lazy plugin service and off-audio shutdown");
}

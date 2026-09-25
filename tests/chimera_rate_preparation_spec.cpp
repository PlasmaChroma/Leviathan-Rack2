#include "ChimeraService.hpp"
#include "ChimeraRateBridge.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

static std::atomic<bool> entered{false}, resume{false};
static std::atomic<unsigned> constructions{0};
static void need(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::_Exit(1); }
}
static void pauseConstruction(unsigned) {
    ++constructions;
    if (entered.exchange(true)) return;
    while (!resume.load()) std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
template<class Predicate> static void await(Predicate predicate, const char* message) {
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!predicate() && std::chrono::steady_clock::now() < deadline)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    need(predicate(), message);
}
int main() {
    std::atomic<unsigned> requested{96000}, active{0};
    std::atomic<chimera::RateBridge*> prepared{nullptr}, retired{nullptr};
    std::atomic<bool> error{false};
    const chimera::RateBridgeSlot slot{&requested, &active, &prepared, &retired, &error};
    chimera::setRatePreparationHook(pauseConstruction);
    chimera::registerRateBridge(slot);
    await([] { return entered.load(); }, "worker entered construction");
    std::atomic<bool> removed{false};
    std::thread remover([&] { chimera::unregisterRateBridge(&prepared); removed.store(true); });
    await([&] { return removed.load(); }, "removal does not wait for Speex construction");
    remover.join();
    // Reuse the same atomic addresses while the old allocation is in flight.
    // Keep the same requested rate: only identity, not a rate mismatch, can
    // reject the old job and force a second construction for this registration.
    requested.store(96000);
    chimera::registerRateBridge(slot);
    resume.store(true);
    await([&] { return prepared.load() != nullptr; }, "new registration receives a bridge");
    need(prepared.load()->rate() == 96000 && constructions.load() == 2 && !error.load(), "stale registration cannot publish into reused storage");
    chimera::unregisterRateBridge(&prepared);
    delete prepared.exchange(nullptr);
    struct Storage {
        std::atomic<unsigned> requested{192000}, active{0};
        std::atomic<chimera::RateBridge*> prepared{nullptr}, retired{nullptr};
        std::atomic<bool> error{false};
    };
    entered.store(false); resume.store(false);
    auto* storage = new Storage;
    chimera::registerRateBridge({&storage->requested, &storage->active, &storage->prepared,
        &storage->retired, &storage->error});
    await([] { return entered.load(); }, "second worker enters construction");
    chimera::unregisterRateBridge(&storage->prepared);
    delete storage; // ASAN/TSAN verify that completion never dereferences it.
    resume.store(true);
    chimera::shutdownChimeraIoService();
    chimera::setRatePreparationHook(nullptr);
    std::puts("PASS: rate construction outside registration lock, removal and address reuse");
}

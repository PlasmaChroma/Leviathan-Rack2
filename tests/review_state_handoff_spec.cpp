#include "../src/Bulkhead.hpp"
#include "../src/Chronomaw.hpp"
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <thread>

ModuleTeardownTimer::ModuleTeardownTimer(const char* name) : moduleName(name) {}
ModuleTeardownTimer::~ModuleTeardownTimer() {}
void ModuleTeardownTimer::begin(int) {}

int main() {
    Module::ProcessArgs args; args.sampleRate = 48000; args.sampleTime = 1.f/48000;
    Bulkhead bulk;
    bulk.inputs[Bulkhead::WALL_LEFT_INPUT].channels = 1;
    bulk.inputs[Bulkhead::WALL_LEFT_INPUT].setVoltage(5.f);
    for (int i = 0; i < 1600; ++i) bulk.process(args);
    bulk.serviceGeometryUi();
    assert(bulk.room.left == -4.f && std::fabs(bulk.displayGeometry.room.left + 3.5f) < 1e-5f);
    bulk.inputs[Bulkhead::WALL_LEFT_INPUT].channels = 0;
    for (int i = 0; i < 1600; ++i) bulk.process(args);
    bulk.serviceGeometryUi();
    assert(bulk.displayGeometry.room.left == -4.f);
    bulk.room.left = -5.f; bulk.publishGeometry();
    json_t* saved = bulk.dataToJson();
    Bulkhead restored; restored.dataFromJson(saved); json_decref(saved);
    assert(restored.room.left == -5.f);

    Chronomaw chrono;
    chrono.state.live.outputs[0].levelPct = 13;
    chrono.serviceUi(); chrono.process(args);
    assert(chrono.audioLive.outputs[0].levelPct == 13);
    chrono.params[Chronomaw::SAVE_BANK_PARAM].setValue(1);
    chrono.process(args); chrono.serviceUi();
    assert(chrono.state.banks[0].outputs[0].levelPct == 13);
    chrono.params[Chronomaw::SAVE_BANK_PARAM].setValue(0);
    chrono.state.live.outputs[0].levelPct = 73;
    chrono.serviceUi(); chrono.process(args);
    chrono.params[Chronomaw::LOAD_BANK_PARAM].setValue(1);
    chrono.process(args);
    assert(chrono.audioLive.outputs[0].levelPct == 73); // deferred UI-owned bank operation
    chrono.serviceUi(); chrono.process(args);
    assert(chrono.audioLive.outputs[0].levelPct == 13);
    chrono.params[Chronomaw::LOAD_BANK_PARAM].setValue(0);
    chrono.params[Chronomaw::RUN_PARAM].setValue(1); chrono.process(args);
    const bool running = chrono.audioLive.running;
    chrono.serviceUi(); chrono.process(args);
    assert(chrono.audioLive.running == running); // config publication doesn't reset transport
    saved = chrono.dataToJson();
    Chronomaw second; second.dataFromJson(saved); json_decref(saved); second.process(args);
    assert(second.audioLive.outputs[0].levelPct == 13 && second.audioLive.running == running);

    // UI servicing before audio accepts reset must not revive the old Run latch.
    chrono.params[Chronomaw::RUN_PARAM].setValue(0);
    chrono.onReset(); chrono.serviceUi(); chrono.process(args);
    assert(!chrono.audioLive.running && !chrono.state.live.running);
    // Preserve the engine contract: connected Run CV overrides the saved Run latch.
    chrono.inputs[Chronomaw::RUN_INPUT].channels = 1;
    chrono.inputs[Chronomaw::RUN_INPUT].setVoltage(10.f);
    chrono.process(args); chrono.serviceUi();
    assert(chrono.state.live.running);
    chrono.inputs[Chronomaw::RUN_INPUT].channels = 0;

    std::atomic<bool> done {false};
    std::thread audio([&]() {
        do { bulk.process(args); chrono.process(args); } while (!done.load(std::memory_order_acquire));
    });
    for (int i = 0; i < 10000; ++i) {
        bulk.room.left = -4.f - float(i%10)*0.1f;
        bulk.room.right = -bulk.room.left;
        bulk.serviceGeometryUi();
        chrono.state.live.outputs[0].levelPct = float(i%101);
        chrono.state.live.outputs[0].widthPct = chrono.state.live.outputs[0].levelPct;
        chrono.serviceUi();
    }
    done.store(true, std::memory_order_release); audio.join();
    chrono.process(args);
    assert(chrono.audioLive.outputs[0].levelPct == chrono.audioLive.outputs[0].widthPct);
    assert(std::isfinite(bulk.outputs[Bulkhead::OUT_L_OUTPUT].getVoltage()));
    std::puts("Review state handoffs: CV stability, banks, transport, persistence, concurrent edit/process passed");
}

#include "TiamatBufferEngine.hpp"
#include "TiamatMacro.hpp"
#include "TiamatCorrupt.hpp"

namespace tiamat {

void BufferEngine::processCorrupt(Corrupt& effect, const float* input, float* output) noexcept {
    effect.processBlock(input, output, corrupt_, random_);
}

void BufferEngine::processBlock(const float* input, float* wet, const BufferBlockInput& block) noexcept {
    if (block.restoreSecondary) events_.restoreSecondaryDefaults();
    if (block.updateSettings) events_.settings = block.settings;
    if (block.restartRandom) { events_.controls.seed = block.seed; random_.restart(block.seed); }
    events_.normalizeSelection();
    std::array<bool, blockFrames> resets {};
    for (unsigned i = 0; i < block.events.count && i < block.events.values.size(); ++i) {
        const auto& event = block.events.values[i];
        if (events_.apply(event) && event.frame < blockFrames) resets[event.frame] = true;
    }
    const auto effective = events_.effective();
    mapped_ = mapper_.map(block.primary, block.cv, events_.settings, events_.controls.mode, effective);
    clock_.configure(mapped_.time, events_.controls.clockSource);
    bool pending = false;
    ClockTick tick;
    for (unsigned frame = 0; frame < blockFrames; ++frame) {
        const double seconds = double(frames_ + frame + 1) / rendererRate;
        if (resets[frame]) clock_.resetPhaseAt(seconds);
        tick = clock_.advance(seconds, block.clockRises[frame]);
        boundaries_ += tick.boundaries;
        if (!tick.timeChanging && tick.boundaries) pending = true;
    }
    frames_ += blockFrames;
    corrupt_.primary = events_.controls.effect;
    corrupt_.amount = mapped_.corrupt;
    if (pending) retainedCorruptDecision(corrupt_, mapped_.corrupt, random_);
    BufferControls controls;
    controls.frequency = tick.bufferFrequency;
    controls.baseRate = mapped_.baseRate;
    controls.silence = mapped_.manualSilence;
    controls.traverse = mapped_.traverse;
    controls.window = mapped_.windowSquared;
    controls.macroBend = mapped_.macroBend;
    controls.macroBreak = mapped_.macroBreak;
    controls.repeatsExponent = mapped_.repeatsExponent;
    controls.timeChanging = tick.timeChanging;
    controls.freezeRequested = effective.freeze;
    controls.momentaryFreeze = events_.settings.freezeButton == FreezeButton::Momentary;
    controls.clockRequest = pending;
    controls.unique = events_.settings.unique;
    buffer_.processBlock(input, wet, controls, random_);
}

} // namespace tiamat

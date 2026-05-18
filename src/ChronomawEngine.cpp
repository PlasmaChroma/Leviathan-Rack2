#include "ChronomawEngine.hpp"
#include "ChronomawWaveforms.hpp"

#include "plugin.hpp"

namespace chronomaw {

void Engine::reset() {
	phaseBeats_ = 0.f;
	cycleCount_ = 0u;
	currentGate_ = 0.f;
}

void Engine::process(const FrameInputs& in, LiveState& live, FrameOutputs* out) {
	if (!out) {
		return;
	}
	out->running = live.running;
	out->phaseBeats = phaseBeats_;
	out->cycleCount = cycleCount_;
	if (in.runConnected) {
		live.running = (in.runVoltage >= 2.f);
	}
	if (in.resetConnected && in.resetVoltage >= 2.f) {
		phaseBeats_ = 0.f;
		cycleCount_ = 0u;
	}
	live.bpm = clamp(live.bpm, kMinBpm, kMaxBpm);
	if (!live.running) {
		currentGate_ = 0.f;
			out->running = false;
			out->phaseBeats = phaseBeats_;
			out->cycleCount = cycleCount_;
			for (int i = 0; i < kNumOutputs; ++i) {
			out->internalVolts[size_t(i)] = 0.f;
			out->outVolts[size_t(i)] = 0.f;
		}
		return;
	}

	const float beatsPerSecond = live.bpm / 60.f;
	phaseBeats_ += beatsPerSecond * in.sampleTime;
	if (phaseBeats_ >= 1.f) {
		const float wraps = std::floor(phaseBeats_);
		phaseBeats_ -= wraps;
		cycleCount_ += uint64_t(std::max(0.f, wraps));
	}
	out->running = true;
	out->phaseBeats = phaseBeats_;
	out->cycleCount = cycleCount_;

	// First-pass engine: generate a simple per-output gate clock.
	// Phase control offsets each output by up to +/- half a cycle.
	currentGate_ = (phaseBeats_ < 0.5f) ? kOutputMaxV : kOutputMinV;
	for (int i = 0; i < kNumOutputs; ++i) {
		const OutputState& ch = live.outputs[size_t(i)];
		float channelPhase = applyTimingPhase(ch, phaseBeats_);
		int64_t cycleOffset = 0;
		if (channelPhase >= 1.f) {
			const float wraps = std::floor(channelPhase);
			channelPhase -= wraps;
			cycleOffset = int64_t(wraps);
		}
		else if (channelPhase < 0.f) {
			const float wraps = std::ceil(-channelPhase);
			channelPhase += wraps;
			cycleOffset = -int64_t(wraps);
		}
		channelPhase -= std::floor(channelPhase);
		uint64_t channelCycle = cycleCount_;
		if (cycleOffset > 0) {
			channelCycle += uint64_t(cycleOffset);
		}
		else if (cycleOffset < 0) {
			const uint64_t dec = uint64_t(-cycleOffset);
			channelCycle = (channelCycle >= dec) ? (channelCycle - dec) : 0u;
		}
		const float internalV = live.running ? waveformInternalVoltage(ch, channelPhase, channelCycle) : kOutputMinV;
		out->internalVolts[size_t(i)] = internalV;
		out->outVolts[size_t(i)] = renderOutputVoltage(ch, live.running, phaseBeats_, channelCycle);
	}
}

} // namespace chronomaw

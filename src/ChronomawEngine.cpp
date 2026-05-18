#include "ChronomawEngine.hpp"

#include "plugin.hpp"

namespace chronomaw {

void Engine::reset() {
	phaseBeats_ = 0.f;
	currentGate_ = 0.f;
}

void Engine::process(const FrameInputs& in, LiveState& live, FrameOutputs* out) {
	if (!out) {
		return;
	}
	out->running = live.running;
	out->phaseBeats = phaseBeats_;
	if (in.runConnected) {
		live.running = (in.runVoltage >= 2.f);
	}
	if (in.resetConnected && in.resetVoltage >= 2.f) {
		phaseBeats_ = 0.f;
	}
	live.bpm = clamp(live.bpm, kMinBpm, kMaxBpm);
	if (!live.running) {
		currentGate_ = 0.f;
		out->running = false;
		out->phaseBeats = phaseBeats_;
		for (int i = 0; i < kNumOutputs; ++i) {
			out->internalVolts[size_t(i)] = 0.f;
			out->outVolts[size_t(i)] = 0.f;
		}
		return;
	}

	const float beatsPerSecond = live.bpm / 60.f;
	phaseBeats_ += beatsPerSecond * in.sampleTime;
	if (phaseBeats_ >= 1.f) {
		phaseBeats_ -= std::floor(phaseBeats_);
	}
	out->running = true;
	out->phaseBeats = phaseBeats_;

	// First-pass engine: generate a simple per-output gate clock.
	// Phase control offsets each output by up to +/- half a cycle.
	currentGate_ = (phaseBeats_ < 0.5f) ? kOutputMaxV : kOutputMinV;
	for (int i = 0; i < kNumOutputs; ++i) {
		const OutputState& ch = live.outputs[size_t(i)];
		const float phaseOffset = clamp(ch.phasePct * 0.005f, -0.5f, 0.5f);
		float channelPhase = phaseBeats_ + phaseOffset;
		channelPhase -= std::floor(channelPhase);
		const float channelGate = (channelPhase < 0.5f) ? kOutputMaxV : kOutputMinV;
		out->internalVolts[size_t(i)] = channelGate;
		float v = channelGate;
		if (ch.muted) {
			v = 0.f;
		}
		else {
			float level = clamp(ch.levelPct * 0.01f, 0.f, 1.f);
			float offset = clamp(ch.offsetPct * 0.01f, -1.f, 1.f);
			v = v * level + offset * kOutputMaxV;
			if (ch.invert) {
				v = kOutputMaxV - v;
			}
		}
		out->outVolts[size_t(i)] = clamp(v, kOutputMinV, kOutputMaxV);
	}
}

} // namespace chronomaw

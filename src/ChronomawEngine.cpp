#include "ChronomawEngine.hpp"
#include "ChronomawWaveforms.hpp"

#include "plugin.hpp"

namespace chronomaw {

namespace {

inline bool timingRatioChanged(const OutputState& a, const OutputState& b) {
	return a.modifierMode != b.modifierMode || std::fabs(effectiveTimingMultiplier(a) - effectiveTimingMultiplier(b)) > 1e-12;
}

} // namespace

void Engine::reset() {
	phaseBeats_ = 0.f;
	cycleCount_ = 0u;
	currentGate_ = 0.f;
	for (int i = 0; i < kNumOutputs; ++i) {
		timingPhaseOffsets_[size_t(i)] = 0.0;
		prevOutputs_[size_t(i)] = OutputState {};
		prevOutputValid_[size_t(i)] = false;
	}
}

void Engine::process(const FrameInputs& in, LiveState& live, FrameOutputs* out) {
	if (!out) {
		return;
	}
	out->running = live.running;
	out->phaseBeats = phaseBeats_;
	out->cycleCount = cycleCount_;
	for (int i = 0; i < kNumOutputs; ++i) {
		out->timingPhaseOffsets[size_t(i)] = timingPhaseOffsets_[size_t(i)];
	}
	if (in.runConnected) {
		live.running = (in.runVoltage >= 2.f);
	}
	if (in.resetConnected && in.resetVoltage >= 2.f) {
		phaseBeats_ = 0.f;
		cycleCount_ = 0u;
		for (int i = 0; i < kNumOutputs; ++i) {
			timingPhaseOffsets_[size_t(i)] = 0.0;
			prevOutputValid_[size_t(i)] = false;
		}
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
	const double basePosition = double(cycleCount_) + double(phaseBeats_);
	bool anyUnmuted = false;
	for (int i = 0; i < kNumOutputs; ++i) {
		if (!live.outputs[size_t(i)].muted) {
			anyUnmuted = true;
			break;
		}
	}
	if (!anyUnmuted) {
		for (int i = 0; i < kNumOutputs; ++i) {
			const size_t idx = size_t(i);
			out->internalVolts[idx] = 0.f;
			out->outVolts[idx] = 0.f;
			out->timingPhaseOffsets[idx] = timingPhaseOffsets_[idx];
			prevOutputs_[idx] = live.outputs[idx];
			prevOutputValid_[idx] = true;
		}
		return;
	}

	// First-pass engine: generate a simple per-output gate clock.
	// Phase control offsets each output by up to +/- half a cycle.
	currentGate_ = (phaseBeats_ < 0.5f) ? kOutputMaxV : kOutputMinV;
	for (int i = 0; i < kNumOutputs; ++i) {
		const OutputState& ch = live.outputs[size_t(i)];
		const size_t idx = size_t(i);
		if (prevOutputValid_[idx] && timingRatioChanged(prevOutputs_[idx], ch)) {
			const double prevBase = basePosition + timingPhaseOffsets_[idx];
			double targetFrac = rawTimingPhase(prevOutputs_[idx], prevBase);
			targetFrac -= std::floor(targetFrac);

			const double m = effectiveTimingMultiplier(ch);
			const double rawNoOffset = rawTimingPhase(ch, basePosition);
			const double alignedRaw = rawNoOffset + m * timingPhaseOffsets_[idx];
			const double n = std::round(alignedRaw - targetFrac);
			timingPhaseOffsets_[idx] = (targetFrac + n - rawNoOffset) / std::max(m, 1e-12);
		}

		const double phaseBase = basePosition + timingPhaseOffsets_[idx];
		if (ch.muted) {
			out->internalVolts[idx] = 0.f;
			out->outVolts[idx] = 0.f;
		}
		else {
			const double rawPhase = rawTimingPhase(ch, phaseBase);
			const float channelPhase = applyTimingPhase(ch, phaseBase);
			const double rawCycle = std::floor(rawPhase);
			const uint64_t channelCycle = (rawCycle > 0.0) ? uint64_t(rawCycle) : 0u;
			const float internalV = live.running ? waveformInternalVoltage(ch, channelPhase, channelCycle) : kOutputMinV;
			out->internalVolts[idx] = internalV;
			const float level = clamp(ch.levelPct * 0.01f, 0.f, 1.f);
			const float offset = clamp(ch.offsetPct * 0.01f, -1.f, 1.f);
			float outV = internalV * level + offset * kOutputMaxV;
			if (ch.invert) {
				outV = kOutputMaxV - outV;
			}
			out->outVolts[idx] = clamp(outV, kOutputMinV, kOutputMaxV);
		}
		out->timingPhaseOffsets[idx] = timingPhaseOffsets_[idx];
		prevOutputs_[idx] = ch;
		prevOutputValid_[idx] = true;
	}
}

} // namespace chronomaw

#pragma once

#include "BifurxInputStage.hpp"
#include "BifurxOutputStage.hpp"

namespace bifurx {

// Keep the stateful TPT filter core at the host rate while running only the
// two memoryless nonlinear boundaries at 2x. Rack's polyphase helpers provide
// the reconstruction/anti-alias filtering that a naive midpoint evaluation
// would omit.
struct BifurxNonlinearOversampling2x {
	static constexpr int kFactor = 2;
	static constexpr int kQuality = 8;

	dsp::Upsampler<kFactor, kQuality> inputUpsampler;
	dsp::Decimator<kFactor, kQuality> inputDecimator;
	dsp::Upsampler<kFactor, kQuality> outputUpsampler;
	dsp::Decimator<kFactor, kQuality> outputDecimator;

	void reset() {
		inputUpsampler.reset();
		inputDecimator.reset();
		outputUpsampler.reset();
		outputDecimator.reset();
	}

	float processInput(float input, float levelKnob) {
		float lanes[kFactor] {};
		inputUpsampler.process(input, lanes);
		for (float& lane : lanes) {
			lane = applyLevelInputStage(lane, levelKnob);
		}
		return inputDecimator.process(lanes);
	}

	float processOutput(float input, float levelKnob, bool softLimitingEnabled) {
		float lanes[kFactor] {};
		outputUpsampler.process(input, lanes);
		for (float& lane : lanes) {
			lane = applyLevelOutputStage(lane, levelKnob, softLimitingEnabled);
		}
		const float output = outputDecimator.process(lanes);
		if (!softLimitingEnabled) {
			return output;
		}
		// The reconstruction filter can ring a little beyond the bounded 2x
		// samples. Preserve the public +/-5 V safety contract at the physical
		// output; this clamp only catches that small reconstruction overshoot.
		return std::max(
			-kOutputSoftLimitCeilingVolts,
			std::min(kOutputSoftLimitCeilingVolts, output));
	}
};

} // namespace bifurx

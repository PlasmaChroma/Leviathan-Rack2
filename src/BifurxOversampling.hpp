#pragma once

#include "BifurxInputStage.hpp"
#include "BifurxOutputStage.hpp"

namespace bifurx {

// Keep the stateful TPT filter core at the host rate while running only the
// two memoryless nonlinear boundaries at 2x. The 64-tap FIR candidate keeps
// the audible passband flat; the 16-tap legacy chain remains for listening A/B.
// Both paths run continuously while selected (no signal-dependent bypass).
template<int Quality>
struct BifurxBoundaryResampling2x {
	static constexpr int kFactor = 2;
	static constexpr int kQuality = Quality;

	dsp::Upsampler<kFactor, kQuality> inputUpsampler;
	dsp::Decimator<kFactor, kQuality> inputDecimator;
	dsp::Upsampler<kFactor, kQuality> outputUpsampler;
	dsp::Decimator<kFactor, kQuality> outputDecimator;

	BifurxBoundaryResampling2x()
		: inputUpsampler(Quality == 8 ? 0.9f : 1.f), inputDecimator(Quality == 8 ? 0.9f : 1.f),
		  outputUpsampler(Quality == 8 ? 0.9f : 1.f), outputDecimator(Quality == 8 ? 0.9f : 1.f) {}

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

using BifurxNonlinearOversampling2x = BifurxBoundaryResampling2x<32>;
using BifurxLegacyOversampling2x = BifurxBoundaryResampling2x<8>;

} // namespace bifurx

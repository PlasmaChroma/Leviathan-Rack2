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

// Four-coefficient polyphase allpass, 2x, transition width 0.08.
// Coefficient design and branch ordering follow Laurent de Soras's HIIR
// (WTFPL v2). See test-results/bifurx-alternatives-20260917/design.py.
// Each nonlinear boundary has its own up/down state; the core stays host-rate.
struct BifurxIirOversampling2x {
	struct AllpassPair {
		float previousInput[4] {}, previousOutput[4] {};
		void process(float& even, float& odd) {
			static constexpr float coeff[4] = {0.09199472379f, 0.3171081174f, 0.584460827f, 0.8541800687f};
			for (int i = 0; i < 4; i += 2) {
				const float e = (even - previousOutput[i]) * coeff[i] + previousInput[i];
				const float o = (odd - previousOutput[i + 1]) * coeff[i + 1] + previousInput[i + 1];
				previousInput[i] = even; previousInput[i + 1] = odd;
				previousOutput[i] = even = e; previousOutput[i + 1] = odd = o;
			}
		}
	};
	AllpassPair inputUp, inputDown, outputUp, outputDown;
	void reset() { inputUp = {}; inputDown = {}; outputUp = {}; outputDown = {}; }
	float processInput(float input, float levelKnob) {
		float even = input, odd = input;
		inputUp.process(even, odd);
		even = applyLevelInputStage(even, levelKnob);
		odd = applyLevelInputStage(odd, levelKnob);
		inputDown.process(odd, even);
		return 0.5f * (even + odd);
	}
	float processOutput(float input, float levelKnob, bool softLimitingEnabled) {
		float even = input, odd = input;
		outputUp.process(even, odd);
		even = applyLevelOutputStage(even, levelKnob, softLimitingEnabled);
		odd = applyLevelOutputStage(odd, levelKnob, softLimitingEnabled);
		outputDown.process(odd, even);
		const float output = 0.5f * (even + odd);
		// IIR reconstruction can overshoot; preserve the physical limiter bound.
		return softLimitingEnabled ? std::max(-kOutputSoftLimitCeilingVolts,
			std::min(kOutputSoftLimitCeilingVolts, output)) : output;
	}
};

} // namespace bifurx

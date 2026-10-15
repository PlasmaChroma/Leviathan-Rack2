#pragma once

#include <algorithm>
#include <cmath>

namespace sil {

// Matched LR4 split, followed by a correlation-controlled, energy-limited mono fold.
// This deliberately changes the mono sum when cancellation would otherwise lose bass.
class LowBandRecovery {
public:
	static constexpr float kCutoffHz = 120.f;
	struct Output {
		float left;
		float right;
		float lowMid;
	};

	void configure(float sampleRate) {
		const double sr = std::max(1000.0, double(sampleRate));
		const double g = std::tan(3.14159265358979323846 * kCutoffHz / sr);
		const double a = 1.0 / (1.0 + g * (g + kButterworth));
		a1 = float(a);
		a2 = float(g * a);
		a3 = float(g * g * a);
		energyMix = float(-std::expm1(-1.0 / (0.100 * sr)));
		attackMix = float(-std::expm1(-1.0 / (0.050 * sr)));
		releaseMix = float(-std::expm1(-1.0 / (0.250 * sr)));
		reset();
	}

	void reset() {
		left = {};
		right = {};
		ll = rr = lr = inputEnergy = 0.f;
		meterInputEnergy = 0.f;
		amount = targetAmount = 0.f;
		feedback = targetFeedback = 0.f;
		referenceRight = false;
		referencePolarity = 1.f;
		weightL = targetWeightL = 1.f;
		weightR = targetWeightR = 0.f;
		referenceGain = targetReferenceGain = 1.f;
		decisionCounter = 15;
	}

	float recoveryAmount() const { return float(amount); }
	// Fraction of full-band input power removed from low-band Side. Unlike the
	// fold control, this reflects how much program material is being corrected.
	float feedbackAmount() const { return feedback; }

	Output process(float inL, float inR) {
		const Split l = split(left, inL);
		const Split r = split(right, inR);
		ll += energyMix * (l.low * l.low - ll);
		rr += energyMix * (r.low * r.low - rr);
		lr += energyMix * (l.low * r.low - lr);
		inputEnergy += energyMix * (inL * inL + inR * inR - inputEnergy);
		// Match the denominator's phase to the crossover so low sine tones do
		// not make the power ratio ripple solely because of crossover phase lag.
		const float neutralL = l.low + l.high, neutralR = r.low + r.high;
		meterInputEnergy += energyMix * (neutralL * neutralL + neutralR * neutralR - meterInputEnergy);
		if (++decisionCounter >= 16) {
			decisionCounter = 0;
			const float sideGain = float(1.0 - amount);
			const float removed = (ll + rr - 2.f * lr) * (1.f - sideGain * sideGain);
			targetFeedback = meterInputEnergy > 1e-10f
				? std::max(0.f, std::min(1.f, removed / (2.f * meterInputEnergy))) : 0.f;
			updateTargets();
		}
		feedback += (targetFeedback > feedback ? attackMix : releaseMix) * (targetFeedback - feedback);
		amount += (targetAmount > amount ? attackMix : releaseMix) * (targetAmount - amount);
		weightL += attackMix * (targetWeightL - weightL);
		weightR += attackMix * (targetWeightR - weightR);
		referenceGain += attackMix * (targetReferenceGain - referenceGain);
		const float mid = 0.5f * (l.low + r.low);
		const float side = 0.5f * (l.low - r.low);
		const float reference = referenceGain * (weightL * l.low + weightR * r.low);
		const float recoveredMid = mid + float(amount) * (reference - mid);
		const float recoveredSide = side * float(1.0 - amount);
		return {l.high + recoveredMid + recoveredSide,
			r.high + recoveredMid - recoveredSide, recoveredMid};
	}

private:
	static constexpr double kButterworth = 1.4142135623730950488;
	struct Svf { float ic1 = 0.f, ic2 = 0.f; };
	struct Channel { Svf first, low; };
	struct Split { float low, high; };
	struct FilterOutput { float low, allpass; };
	Channel left, right;
	float a1 = 1.f, a2 = 0.f, a3 = 0.f;
	float energyMix = 0.f, attackMix = 0.f, releaseMix = 0.f;
	float ll = 0.f, rr = 0.f, lr = 0.f, inputEnergy = 0.f;
	float meterInputEnergy = 0.f;
	// Keep the controller in double so a 192 kHz float rounding plateau cannot
	// leave a permanent residual Side gain when the target is fully mono.
	double amount = 0.0;
	float targetAmount = 0.f;
	float feedback = 0.f, targetFeedback = 0.f;
	bool referenceRight = false;
	float referencePolarity = 1.f;
	float weightL = 1.f, weightR = 0.f;
	float targetWeightL = 1.f, targetWeightR = 0.f;
	float referenceGain = 1.f, targetReferenceGain = 1.f;
	int decisionCounter = 15;

	FilterOutput filter(Svf& state, float in) const {
		const float v3 = in - state.ic2;
		const float v1 = a1 * state.ic1 + a2 * v3;
		const float v2 = state.ic2 + a2 * state.ic1 + a3 * v3;
		state.ic1 = 2.f * v1 - state.ic1;
		state.ic2 = 2.f * v2 - state.ic2;
		return {v2, in - 2.f * float(kButterworth) * v1};
	}
	Split split(Channel& state, float in) const {
		const FilterOutput first = filter(state.first, in);
		const float low = filter(state.low, first.low).low;
		// LR4 LP + HP is the first Butterworth section's second-order all-pass.
		// Derive HP from that phase-matched sum, saving two SVFs per stereo sample.
		// Subtracting from the *dry input* instead would produce the old peaking bug.
		return {low, first.allpass - low};
	}

	void updateTargets() {
		const float energy = ll + rr;
		// Ignore silence and negligible low-pass leakage from treble-only material.
		// Gate fades between -40 and -30 dB low-band/full-band energy ratio.
		const float fraction = energy / std::max(inputEnergy, 1e-20f);
		const float confidence = std::max(0.f, std::min(1.f, (fraction - 0.0001f) / 0.0009f));
		if (energy < 1e-10f || confidence == 0.f) {
			targetAmount = 0.f;
			return;
		}
		const float corr = std::max(-1.f, std::min(1.f, lr / std::sqrt(std::max(ll * rr, 1e-30f))));
		targetAmount = confidence * std::max(0.f, std::min(1.f, 1.f - corr / 0.70f));

		// Keep a stable phase reference: left wins a tie, and a 6 dB energy
		// advantage is needed to change channels. Align polarity on handoff so
		// opposite-polarity bass does not cancel during the smoothed transition.
		const bool nextRight = referenceRight ? !(ll > 4.f * rr) : (rr > 4.f * ll);
		if (nextRight != referenceRight) {
			if (corr < 0.f) referencePolarity = -referencePolarity;
			referenceRight = nextRight;
		}
		targetWeightL = referenceRight ? 0.f : referencePolarity;
		targetWeightR = referenceRight ? referencePolarity : 0.f;
		const float referenceEnergy = weightL * weightL * ll + weightR * weightR * rr
			+ 2.f * weightL * weightR * lr;
		// Match per-channel average input energy, never chase a vanishing reference
		// with unbounded gain. sqrt is control-rate, not per sample.
		targetReferenceGain = std::sqrt(std::min(2.f, 0.5f * energy / std::max(referenceEnergy, 1e-20f)));
	}
};

} // namespace sil

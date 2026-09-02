#include "PhonexEngine.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace phonex {

void ChirpGenerator::reset() {
	periodPhase_ = 0;
}

float ChirpGenerator::next(float pitchPeriodTicks) {
	// A zero period is malformed/inaudible input, and must not create a divide
	// or wrap hazard. Voiced corpus frames use positive periods.
	if (!(pitchPeriodTicks > 0.f) || !std::isfinite(pitchPeriodTicks))
		return 0.f;
	const std::size_t chirpIndex = static_cast<std::size_t>(periodPhase_);
	const float output = chirpIndex < kChirp.size() ? kChirp[chirpIndex] : 0.f;
	periodPhase_ += 1.f;
	while (periodPhase_ >= pitchPeriodTicks)
		periodPhase_ -= pitchPeriodTicks;
	return output;
}

void NoiseGenerator::reset() {
	state_ = kLfsrReset;
}

float NoiseGenerator::next() {
	const std::uint32_t old = state_;
	const float output = (old & 1u) ? 1.f : -1.f;
	state_ = ((old >> 1) | kLfsrTopBit)
		^ (((old & 1u) - 1u) & kLfsrXorMask);
	state_ &= kLfsrMask;
	return output;
}

void LatticeFilter::reset() {
	state_.fill(0.f);
}

void LatticeFilter::leak(float multiplier) {
	for (float& value : state_)
		value *= multiplier;
}

float LatticeFilter::process(
	float excitation,
	const std::array<float, kLpcOrder>& reflection,
	float coefficientLimit) {
	coefficientLimit = std::max(0.f, std::min(1.08f, coefficientLimit));
	std::array<float, kLpcOrder + 1> u{};
	u[kLpcOrder] = excitation;
	for (int i = kLpcOrder - 1; i >= 0; --i) {
		const float k = std::max(-coefficientLimit, std::min(coefficientLimit, reflection[i]));
		u[i] = u[i + 1] - k * state_[i];
	}
	for (int i = kLpcOrder - 1; i >= 1; --i) {
		const float k = std::max(-coefficientLimit, std::min(coefficientLimit, reflection[i - 1]));
		state_[i] = state_[i - 1] + k * u[i - 1];
	}
	state_[0] = u[0];
	return u[0];
}

namespace {

float clampf(float value, float low, float high) {
	return std::max(low, std::min(high, value));
}

float quietSpeed(float speed) {
	return std::abs(speed) < 0.025f ? 0.f : clampf(speed, -4.f, 4.f);
}

float unitFloat(std::uint32_t value) {
	return static_cast<float>(value >> 8) * (1.f / 16777216.f);
}

bool sameReflection(const std::array<float, kLpcOrder>& a,
	const std::array<float, kLpcOrder>& b) {
	for (int i = 0; i < kLpcOrder; ++i) {
		if (std::abs(a[i] - b[i]) > 1e-5f)
			return false;
	}
	return true;
}

float fastTanh(float value) {
	const float squared = value * value;
	return clampf(value * (27.f + squared) / (27.f + 9.f * squared), -1.f, 1.f);
}

float fastExp2(float value) {
	value = clampf(value, -8.f, 8.f);
	const int exponent = static_cast<int>(std::floor(value));
	const float fraction = value - static_cast<float>(exponent);
	const float polynomial = 1.f + fraction * (0.69314718f
		+ fraction * (0.24022651f + fraction * (0.05550411f
		+ fraction * (0.00961813f + fraction * 0.00133336f))));
	return std::ldexp(polynomial, exponent);
}

using Predictor = std::array<double, kLpcOrder + 1>;

Predictor reflectionToPredictor(const std::array<float, kLpcOrder>& reflection) {
	Predictor coefficients{};
	coefficients[0] = 1.0;
	for (int order = 1; order <= kLpcOrder; ++order) {
		const double k = clampf(reflection[order - 1], -0.985f, 0.985f);
		Predictor next = coefficients;
		for (int i = 1; i < order; ++i)
			next[i] = coefficients[i] + k * coefficients[order - i];
		next[order] = k;
		coefficients = next;
	}
	return coefficients;
}

bool predictorToReflection(Predictor coefficients,
	std::array<float, kLpcOrder>& reflection) {
	if (!std::isfinite(coefficients[0]) || std::abs(coefficients[0]) < 1e-12)
		return false;
	for (double& value : coefficients)
		value /= coefficients[0];
	for (int order = kLpcOrder; order >= 1; --order) {
		const double k = coefficients[order];
		if (!std::isfinite(k) || std::abs(k) >= 0.999)
			return false;
		reflection[order - 1] = static_cast<float>(clampf(
			static_cast<float>(k), -0.985f, 0.985f));
		const double denominator = 1.0 - k * k;
		Predictor next{};
		next[0] = 1.0;
		for (int i = 1; i < order; ++i)
			next[i] = (coefficients[i] - k * coefficients[order - i]) / denominator;
		coefficients = next;
	}
	return true;
}

} // namespace

float applyOutputStage(float reconstructed, OutputStage stage) {
	if (stage == OutputStage::LegacyCurve)
		return reconstructed / (0.25f + std::abs(reconstructed) * 0.2f);
	constexpr float kLinearOutputGain = 1.1f;
	const float linear = reconstructed * kLinearOutputGain;
	if (stage == OutputStage::CalibratedLinear)
		return linear;
	const float magnitude = std::abs(linear);
	if (magnitude <= 4.5f)
		return linear;
	const float excess = magnitude - 4.5f;
	const float limited = 4.5f + 0.5f * excess / (0.5f + excess);
	return std::copysign(limited, linear);
}

float tms5100InterpolationMix(float frameFraction) {
	// The TMS5100 approached each target through eight fixed interpolation
	// periods. These cumulative mixes are the result of its successive
	// divide-by-8, divide-by-4, and divide-by-2 parameter updates.
	constexpr std::array<float, 8> kMix {{
		0.f,
		0.125f,
		0.234375f,
		0.330078125f,
		0.497558594f,
		0.623168945f,
		0.811584473f,
		0.905792236f,
	}};
	if (!(frameFraction > 0.f))
		return 0.f;
	if (frameFraction >= 1.f)
		return 1.f;
	const int period = std::min(7, static_cast<int>(frameFraction * 8.f));
	return kMix[period];
}

std::array<float, kLpcOrder> formantShiftReflection(
	const std::array<float, kLpcOrder>& reflection, float amount) {
	amount = clampf(amount, -1.f, 1.f);
	if (std::abs(amount) < 1e-5f)
		return reflection;
	const double alpha = -0.24 * static_cast<double>(amount);
	std::array<Predictor, kLpcOrder + 1> numeratorPowers{};
	std::array<Predictor, kLpcOrder + 1> denominatorPowers{};
	numeratorPowers[0][0] = 1.0;
	denominatorPowers[0][0] = 1.0;
	for (int power = 1; power <= kLpcOrder; ++power) {
		for (int degree = 0; degree < power; ++degree) {
			numeratorPowers[power][degree] += -alpha * numeratorPowers[power - 1][degree];
			numeratorPowers[power][degree + 1] += numeratorPowers[power - 1][degree];
			denominatorPowers[power][degree] += denominatorPowers[power - 1][degree];
			denominatorPowers[power][degree + 1] += -alpha * denominatorPowers[power - 1][degree];
		}
	}
	const Predictor source = reflectionToPredictor(reflection);
	Predictor shifted{};
	for (int term = 0; term <= kLpcOrder; ++term) {
		const int denominatorPower = kLpcOrder - term;
		for (int i = 0; i <= term; ++i)
			for (int j = 0; j <= denominatorPower; ++j)
				shifted[i + j] += source[term]
					* numeratorPowers[term][i] * denominatorPowers[denominatorPower][j];
	}
	std::array<float, kLpcOrder> result{};
	return predictorToReflection(shifted, result) ? result : reflection;
}

std::array<float, kLpcOrder> warpReflectionCoefficients(
	const std::array<float, kLpcOrder>& reflection, float amount) {
	if (std::abs(amount) < 1e-6f)
		return reflection;
	std::array<float, kLpcOrder> result{};
	const float scale = 1.f + 0.8f * clampf(amount, -1.f, 1.f);
	for (int i = 0; i < kLpcOrder; ++i)
		result[i] = clampf(fastTanh(reflection[i] * scale), -0.995f, 0.995f);
	return result;
}

std::uint32_t xorshift32(std::uint32_t& state) {
	if (state == 0)
		state = 0x6d2b79f5u;
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

std::uint32_t phonexFrameHash(
	std::uint32_t seed, std::uint32_t frameIndex, std::uint32_t level) {
	std::uint32_t state = seed
		^ (frameIndex * 0x9e3779b9u)
		^ (level * 0x85ebca6bu);
	if (state == 0)
		state = 0x27d4eb2du;
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

LpcFrame selectGlitchedFrame(const LpcSequence& sequence,
	std::uint16_t frameIndex, std::uint8_t requestedLevel, std::uint32_t seed) {
	if (!sequence.valid() || sequence.frameCount == 0)
		return {};
	const std::uint8_t level = std::min<std::uint8_t>(requestedLevel, 15);
	const std::uint16_t last = sequence.frameCount - 1;
	const std::uint16_t sourceIndex = std::min(frameIndex, last);
	if (level == 0)
		return sequence.frames[sourceIndex];
	const std::uint32_t hash = phonexFrameHash(seed, sourceIndex, level);
	int selected = sourceIndex;
	switch (level) {
		case 1: if ((sourceIndex & 3u) == 3u) selected -= 1; break;
		case 2: if ((sourceIndex & 3u) == 3u) selected += 1; break;
		case 3: selected = sourceIndex ^ 1u; break;
		case 4: selected = sourceIndex ^ 2u; break;
		case 5: selected += (hash & 1u) ? 2 : -2; break;
		case 6: selected = (sourceIndex & ~3u) + (3u - (sourceIndex & 3u)); break;
		case 15: selected = sourceIndex ^ 3u; break;
		default: break;
	}
	selected = std::max(0, std::min<int>(last, selected));
	LpcFrame frame = sequence.frames[static_cast<std::uint16_t>(selected)];
	switch (level) {
		case 7:
			frame.energy = std::round(frame.energy * 3.f) / 3.f;
			break;
		case 8:
			if (frame.excitation == Excitation::Voiced)
				frame.pitchPeriod10k = std::round(frame.pitchPeriod10k / 8.f) * 8.f;
			break;
		case 9:
			if (frame.excitation == Excitation::Voiced)
				frame.pitchPeriod10k *= (hash & 1u) ? 2.f : 0.5f;
			break;
		case 10:
			for (int i = 0; i < kLpcOrder; i += 3)
				frame.reflection[i] = -frame.reflection[i];
			break;
		case 11: {
			const auto original = frame.reflection;
			for (int i = 0; i < kLpcOrder; ++i)
				frame.reflection[(i + 2) % kLpcOrder] = original[i];
			break;
		}
		case 12:
			for (int i = 1; i < kLpcOrder; i += 2)
				frame.reflection[i] = 0.f;
			break;
		case 13:
			for (float& coefficient : frame.reflection)
				coefficient = clampf(std::round(coefficient * 8.f) / 8.f, -0.995f, 0.995f);
			break;
		case 14:
			if (sourceIndex & 1u) {
				if (frame.excitation == Excitation::Voiced)
					frame.excitation = Excitation::Unvoiced;
				else if (frame.excitation == Excitation::Unvoiced)
					frame.excitation = Excitation::Voiced;
			}
			break;
		case 15:
			frame.energy = std::round(frame.energy * 3.f) / 3.f;
			if (frame.excitation == Excitation::Voiced)
				frame.pitchPeriod10k = std::round(frame.pitchPeriod10k / 8.f) * 8.f;
			for (int i = 0; i < kLpcOrder; ++i) {
				if ((hash >> i) & 1u)
					frame.reflection[i] = -frame.reflection[i];
			}
			break;
		default: break;
	}
	return frame;
}

void Engine::setSequence(const LpcSequence* sequence) {
	sequence_ = sequence && sequence->valid() ? sequence : nullptr;
	retrigger(1.f);
}

void Engine::setInternalRate(float rateHz) {
	internalRate_ = rateHz < 9000.f ? 8000.f : 10000.f;
	reconstructionHostRate_ = 0.f;
}

void Engine::setSeed(std::uint32_t seed) {
	seed_ = seed;
	bendState_ = seed_;
}

void Engine::clearSynthesis() {
	lattice_.reset();
	chirp_.reset();
	noise_.reset();
	internalPhase_ = 0.0;
	heldSample_ = 0.f;
	lastVoicedPitchPeriod_ = 0.f;
	reconstructionZ1_.fill(0.f);
	reconstructionZ2_.fill(0.f);
	completionTailRemaining_ = 0;
	completionRampRemaining_ = 0;
	completionRampTotal_ = 0;
	completionHeldSample_ = 0.f;
	jitterScale_ = 1.f;
	bendState_ = seed_;
	warpedReflectionValid_ = false;
}

float Engine::reconstructFiltered(float input, float hostSampleRate) {
	if (!(hostSampleRate > 0.f) || !std::isfinite(hostSampleRate))
		return input;
	if (std::abs(hostSampleRate - reconstructionHostRate_) > 0.5f
		|| internalRate_ != reconstructionInternalRate_) {
		// Cascaded Butterworth sections remove zero-order-hold images while
		// preserving the measured speech passband. Coefficients are rebuilt only
		// when a sample rate or selected offline-tested order changes.
		const float cutoff = std::min(0.49f * internalRate_, 0.45f * hostSampleRate);
		const float k = std::tan(3.14159265358979323846f * cutoff / hostSampleRate);
		constexpr std::array<std::array<float, 3>, 3> kButterworthQ {{
			{{0.7071067812f, 0.f, 0.f}},
			{{0.5411961001f, 1.306562965f, 0.f}},
			{{0.5176380902f, 0.7071067812f, 1.931851653f}},
		}};
		const int sections = static_cast<int>(reconstructionOrder_);
		for (int section = 0; section < sections; ++section) {
			const float inverseQ = 1.f / kButterworthQ[sections - 1][section];
			const float norm = 1.f / (1.f + inverseQ * k + k * k);
			reconstructionB0_[section] = k * k * norm;
			reconstructionB1_[section] = 2.f * reconstructionB0_[section];
			reconstructionB2_[section] = reconstructionB0_[section];
			reconstructionA1_[section] = 2.f * (k * k - 1.f) * norm;
			reconstructionA2_[section] = (1.f - inverseQ * k + k * k) * norm;
		}
		reconstructionHostRate_ = hostSampleRate;
		reconstructionInternalRate_ = internalRate_;
		reconstructionZ1_.fill(0.f);
		reconstructionZ2_.fill(0.f);
	}
	float output = input;
	const int sections = static_cast<int>(reconstructionOrder_);
	for (int section = 0; section < sections; ++section) {
		const float next = reconstructionB0_[section] * output + reconstructionZ1_[section];
		reconstructionZ1_[section] = reconstructionB1_[section] * output
			- reconstructionA1_[section] * next + reconstructionZ2_[section];
		reconstructionZ2_[section] = reconstructionB2_[section] * output
			- reconstructionA2_[section] * next;
		output = next;
	}
	return output;
}

void Engine::retrigger(float speed) {
	clearSynthesis();
	position_ = sequence_ && sequence_->frameCount > 0 && speed < 0.f
		? static_cast<float>(sequence_->frameCount - 1) : 0.f;
	observedFrame_ = static_cast<std::uint16_t>(position_);
	eoxArmed_ = true;
	playbackComplete_ = false;
	frameChanged_ = false;
	eoxEvent_ = false;
	framePulseRemaining_ = 0;
	eoxPulseRemaining_ = 0;
}

LpcFrame Engine::interpolatedFrame() const {
	LpcFrame result;
	if (!sequence_ || sequence_->frameCount == 0)
		return result;
	const float last = static_cast<float>(sequence_->frameCount - 1);
	const float bounded = clampf(position_, 0.f, last);
	const std::uint16_t i0 = static_cast<std::uint16_t>(std::floor(bounded));
	const std::uint16_t i1 = std::min<std::uint16_t>(i0 + 1, sequence_->frameCount - 1);
	const LpcFrame a = selectGlitchedFrame(*sequence_, i0, activeGlitchLevel_, seed_);
	const LpcFrame b = selectGlitchedFrame(*sequence_, i1, activeGlitchLevel_, seed_);
	if (a.excitation != b.excitation || i0 == i1)
		return a;
	const float mix = tms5100InterpolationMix(bounded - static_cast<float>(i0));
	result.energy = a.energy + (b.energy - a.energy) * mix;
	result.pitchPeriod10k = a.pitchPeriod10k + (b.pitchPeriod10k - a.pitchPeriod10k) * mix;
	for (int i = 0; i < kLpcOrder; ++i)
		result.reflection[i] = a.reflection[i] + (b.reflection[i] - a.reflection[i]) * mix;
	result.excitation = a.excitation;
	return result;
}

void Engine::updateTransport(const EngineControls& controls, bool triggerRise) {
	frameChanged_ = false;
	eoxEvent_ = false;
	if (!sequence_ || sequence_->frameCount == 0) {
		position_ = 0.f;
		observedFrame_ = 0;
		return;
	}
	const float last = static_cast<float>(sequence_->frameCount - 1);
	const float forwardEnd = static_cast<float>(sequence_->frameCount);
	const float speed = quietSpeed(controls.speed);
	if (triggerMode_ == TriggerMode::AdvanceOneFrame) {
		if (triggerRise) {
			position_ = clampf(position_ + (speed < 0.f ? -1.f : 1.f), 0.f, last);
		}
	}
	else if (controls.scrubConnected) {
		position_ = clampf((controls.scrubVoltage + 5.f) * 0.1f, 0.f, 1.f) * last;
	}
	else if (controls.hostSampleRate > 0.f && std::isfinite(controls.hostSampleRate)) {
		// A sequence of N 20 ms frames occupies transport time [0, N]. Keep
		// rendering the final frame over [N - 1, N) instead of completing as
		// soon as its index is first reached.
		position_ = clampf(position_ + speed * 50.f / controls.hostSampleRate,
			0.f, speed < 0.f ? last : forwardEnd);
		if (speed > 0.f && forwardEnd - position_ < 1e-5f)
			position_ = forwardEnd;
	}
	const std::uint16_t nextFrame = std::min<std::uint16_t>(
		static_cast<std::uint16_t>(std::floor(position_)), sequence_->frameCount - 1);
	frameChanged_ = nextFrame != observedFrame_;
	observedFrame_ = nextFrame;
	const bool interactiveTransport = triggerMode_ == TriggerMode::AdvanceOneFrame
		|| controls.scrubConnected;
	const bool atEnd = speed < 0.f ? position_ <= 0.f
		: position_ >= (interactiveTransport ? last : forwardEnd);
	if (interactiveTransport)
		playbackComplete_ = false;
	if (!atEnd) {
		eoxArmed_ = true;
		playbackComplete_ = false;
	}
	else if (eoxArmed_) {
		eoxEvent_ = true;
		eoxArmed_ = false;
		if (!interactiveTransport && speed != 0.f)
			playbackComplete_ = true;
	}
}

const std::array<float, kLpcOrder>& Engine::warpedReflection(
	const LpcFrame& frame, float formant, float warp, float overdrive) {
	if (!warpedReflectionValid_ || std::abs(formant - cachedFormant_) > 1e-4f
		|| std::abs(warp - cachedWarp_) > 1e-4f
		|| std::abs(overdrive - cachedOverdrive_) > 1e-4f
		|| !sameReflection(frame.reflection, cachedReflection_)) {
		const std::array<float, kLpcOrder> formantReflection =
			formantShiftReflection(frame.reflection, formant);
		const std::array<float, kLpcOrder> warpReflection =
			warpReflectionCoefficients(formantReflection, warp);
		for (int i = 0; i < kLpcOrder; ++i) {
			warpedReflection_[i] = clampf(
				warpReflection[i] * overdrive, -1.08f, 1.08f);
		}
		cachedReflection_ = frame.reflection;
		cachedFormant_ = formant;
		cachedWarp_ = warp;
		cachedOverdrive_ = overdrive;
		warpedReflectionValid_ = true;
	}
	return warpedReflection_;
}

float Engine::synthesizeTick(const LpcFrame& frame, const EngineControls& controls,
	float bend, float skipProbability, float leakAmount, float overdrive) {
	if (bend > 0.f && unitFloat(xorshift32(bendState_)) < skipProbability) {
		lattice_.leak(1.f - leakAmount);
		return heldSample_;
	}
	float automatic = 0.f;
	const float pitchScale = fastExp2(
		controls.pitchOctaves + controls.voct);
	if (frame.excitation == Excitation::Voiced) {
		if (frame.pitchPeriod10k > 0.f && std::isfinite(frame.pitchPeriod10k))
			lastVoicedPitchPeriod_ = frame.pitchPeriod10k;
		automatic = chirp_.next(frame.pitchPeriod10k / pitchScale);
	}
	else if (frame.excitation == Excitation::Unvoiced)
		automatic = noise_.next();
	const float blend = clampf(controls.exciteBlend, 0.f, 1.f);
	float carrier = automatic;
	if (blend > 0.f) {
		float forced = 0.f;
		if (controls.forcedExcitation == ForcedExcitation::Voiced) {
			const float fallbackPitch = lastVoicedPitchPeriod_ > 0.f
				? lastVoicedPitchPeriod_ : internalRate_ / 125.f;
			forced = frame.excitation == Excitation::Voiced
				? automatic : chirp_.next(fallbackPitch / pitchScale);
		}
		else {
			forced = frame.excitation == Excitation::Unvoiced ? automatic : noise_.next();
		}
		carrier += (forced - automatic) * blend;
	}
	if (controls.externalConnected)
		carrier = std::isfinite(controls.externalExcitation)
			? clampf(controls.externalExcitation / 5.f, -2.f, 2.f)
			: std::numeric_limits<float>::quiet_NaN();
	return lattice_.process(carrier * clampf(frame.energy, 0.f, 1.f),
		warpedReflection(frame, smoothedFormant_, smoothedWarp_, overdrive),
		bend > 0.f ? 1.08f : 0.995f);
}

EngineOutput Engine::process(const EngineControls& controls) {
	const float hostRate = controls.hostSampleRate;
	activeGlitchLevel_ = std::min<std::uint8_t>(controls.glitchLevel, 15);
	const bool triggerRise = controls.triggerGate && !triggerHigh_;
	const bool wordPushRise = controls.wordPush && !wordPushHigh_;
	triggerHigh_ = controls.triggerGate;
	wordPushHigh_ = controls.wordPush;
	if (wordPushRise || (triggerRise && triggerMode_ == TriggerMode::RetriggerPhrase))
		retrigger(controls.speed);
	updateTransport(controls, triggerRise);
	if (frameChanged_ || eoxEvent_) {
		const std::uint32_t pulseSamples = controls.hostSampleRate > 0.f
			? std::max<std::uint32_t>(1u, static_cast<std::uint32_t>(
				std::ceil(static_cast<double>(controls.hostSampleRate) / 1000.0)))
			: 1u;
		if (frameChanged_)
			framePulseRemaining_ = pulseSamples;
		if (eoxEvent_)
			eoxPulseRemaining_ = pulseSamples;
	}
	if (eoxEvent_ && playbackComplete_) {
		// Stop driving the zero-order hold, but let the default reconstruction
		// filter discharge naturally. Hard-resetting its state here produced a
		// broadband push-to-talk/squelch click at the end of phrases.
		completionHeldSample_ = heldSample_;
		internalPhase_ = 0.0;
		completionTailRemaining_ = reconstructionMode_ == ReconstructionMode::Filtered
			&& hostRate > 0.f && std::isfinite(hostRate)
			? std::max<std::uint32_t>(1u, static_cast<std::uint32_t>(
				std::ceil(static_cast<double>(hostRate) * 0.005)))
			: 0u;
		completionRampTotal_ = completionTailRemaining_ > 0
			? std::max<std::uint32_t>(1u, static_cast<std::uint32_t>(
				std::ceil(static_cast<double>(hostRate) * 0.002)))
			: 0u;
		completionRampRemaining_ = completionRampTotal_;
		if (completionTailRemaining_ == 0)
			clearSynthesis();
	}
	const LpcFrame frame = interpolatedFrame();
	const float targetFormant = clampf(controls.formant, -1.f, 1.f);
	const float targetWarp = clampf(controls.warp + controls.warpCv / 5.f, -1.f, 1.f);
	if (!warpSmootherReady_) {
		smoothedFormant_ = targetFormant;
		smoothedWarp_ = targetWarp;
		warpSmootherReady_ = true;
	}
	else if (hostRate > 0.f && std::isfinite(hostRate)) {
		const float alpha = std::min(1.f, 100.f / hostRate);
		smoothedFormant_ += alpha * (targetFormant - smoothedFormant_);
		smoothedWarp_ += alpha * (targetWarp - smoothedWarp_);
	}
	const float bend = clampf(controls.bend + controls.bendCv / 5.f, 0.f, 1.f);
	const float slow = bend * bend;
	const float clockMultiplier = 1.f - 0.55f * slow;
	const float jitterAmount = 0.12f * clampf((bend - 0.25f) / 0.75f, 0.f, 1.f);
	const float skipShape = clampf((bend - 0.45f) / 0.55f, 0.f, 1.f);
	const float skipProbability = 0.30f * skipShape * skipShape;
	const float leakAmount = 0.04f * clampf((bend - 0.55f) / 0.45f, 0.f, 1.f);
	const float overdrive = 1.f + 0.10f * clampf((bend - 0.70f) / 0.30f, 0.f, 1.f);
	if (!playbackComplete_ && hostRate > 0.f && std::isfinite(hostRate)) {
		internalPhase_ += static_cast<double>(internalRate_ * clockMultiplier * jitterScale_)
			/ static_cast<double>(hostRate);
		while (internalPhase_ >= 1.0 - 1e-12) {
			heldSample_ = synthesizeTick(frame, controls, bend, skipProbability,
				leakAmount, overdrive);
			internalPhase_ -= 1.0;
			if (internalPhase_ < 0.0)
				internalPhase_ = 0.0;
			++internalTicks_;
			if (bend > 0.f)
				jitterScale_ = 1.f + jitterAmount
					* (2.f * unitFloat(xorshift32(bendState_)) - 1.f);
			else
				jitterScale_ = 1.f;
		}
	}
	else if (completionRampRemaining_ > 0) {
		heldSample_ = completionHeldSample_
			* static_cast<float>(completionRampRemaining_)
			/ static_cast<float>(completionRampTotal_);
		--completionRampRemaining_;
	}
	else if (playbackComplete_) {
		heldSample_ = 0.f;
	}
	float reconstructed = heldSample_;
	if (reconstructionMode_ == ReconstructionMode::Filtered
		&& (!playbackComplete_ || completionTailRemaining_ > 0))
		reconstructed = reconstructFiltered(heldSample_, hostRate);
	float voltage = playbackComplete_ && completionTailRemaining_ == 0
		? 0.f : reconstructed;
	if (!std::isfinite(voltage)) {
		clearSynthesis();
		voltage = 0.f;
	}
	else {
		voltage = applyOutputStage(voltage, outputStage_);
		voltage = clampf(voltage, -5.f, 5.f);
	}
	if (completionTailRemaining_ > 0 && --completionTailRemaining_ == 0)
		clearSynthesis();
	EngineOutput output;
	output.audio = voltage;
	output.position = position();
	output.frameIndex = observedFrame_;
	output.framePulse = framePulseRemaining_ > 0;
	output.eoxPulse = eoxPulseRemaining_ > 0;
	if (framePulseRemaining_ > 0)
		--framePulseRemaining_;
	if (eoxPulseRemaining_ > 0)
		--eoxPulseRemaining_;
	output.voiced = !playbackComplete_ && frame.excitation == Excitation::Voiced;
	return output;
}

} // namespace phonex

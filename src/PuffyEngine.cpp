#include "PuffyEngine.hpp"

#include "MathHelpers.hpp"

#include <array>
#include <limits>

namespace puffy {
namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kMinimumSampleRate = 1000.f;
constexpr float kLiveCeilingVolts = 5.f;

float clamp01(float value) {
	return std::max(0.f, std::min(value, 1.f));
}

float onePoleCoefficient(float seconds, float sampleRate) {
	if (!(seconds > 0.f) || !(sampleRate > 0.f)) {
		return 1.f;
	}
	return 1.f - std::exp(-1.f / (seconds * sampleRate));
}

float fastNegativeExp(float x) {
	// Puffy's Auto Deflate exponent is bounded to approximately [-0.47, 0].
	const float x2 = x * x;
	const float x3 = x2 * x;
	const float x4 = x2 * x2;
	const float x5 = x4 * x;
	return 1.f + x + 0.5f * x2 + (1.f / 6.f) * x3
		+ (1.f / 24.f) * x4 + (1.f / 120.f) * x5;
}

Character clampCharacter(int character) {
	if (character <= int(Character::Bloom)) {
		return Character::Bloom;
	}
	if (character >= int(Character::Void)) {
		return Character::Void;
	}
	return static_cast<Character>(character);
}

float fractalShape(float input) {
	// Five dyadic levels sampled from an original hand-fitted fold curve. The
	// equal spacing makes the hot path one lookup and one linear interpolation;
	// mirroring the positive half below preserves exact odd symmetry.
	static constexpr std::array<float, 33> kFoldResponse {{
		0.0000f, 0.2135f, 0.2691f, 0.2240f, 0.2986f, 0.4132f,
		0.5851f, 0.5660f, 0.4583f, 0.4193f, 0.4931f, 0.5556f,
		0.4583f, 0.3220f, 0.2552f, 0.3142f, 0.3750f, 0.3186f,
		0.2170f, 0.2240f, 0.3958f, 0.5122f, 0.5243f, 0.4748f,
		0.5556f, 0.7378f, 0.8837f, 0.8733f, 0.8333f, 0.8602f,
		0.9427f, 1.0000f, 1.0000f,
	}};

	const float magnitude = std::fabs(input);
	if (magnitude >= 1.f) {
		return std::copysign(1.f, input);
	}

	const float tablePosition = magnitude * 32.f;
	const int index = static_cast<int>(tablePosition);
	const float fraction = tablePosition - float(index);
	const float shaped = kFoldResponse[index]
		+ (kFoldResponse[index + 1] - kFoldResponse[index]) * fraction;
	return std::copysign(shaped, input);
}

float autoDeflateDb(Character character, float amount) {
	switch (character) {
		case Character::Void:
			// VOID already removes energy around the zero crossing. Additional
			// automatic attenuation would make it collapse in linked mode.
			return 0.f;
		case Character::Spine:
			return -4.f * amount;
		case Character::Riptide:
			return -3.5f * amount;
		case Character::Frenzy:
			return -3.f * amount;
		case Character::Bloom:
		default:
			return -2.5f * amount;
	}
}

} // namespace

Engine::Engine() {
	setSampleRate(48000.f);
	reset();
}

void Engine::DcBlocker::reset() {
	x1 = 0.f;
	y1 = 0.f;
}

float Engine::DcBlocker::process(float input) {
	const float output = input - x1 + coefficient * y1;
	x1 = input;
	y1 = std::fabs(output) < 1e-20f ? 0.f : output;
	return y1;
}

void Engine::PathState::reset() {
	decimatorLeft.reset();
	decimatorRight.reset();
	dcLeft.reset();
	dcRight.reset();
}

void Engine::setSampleRate(float requestedSampleRate) {
	sampleRate = std::isfinite(requestedSampleRate)
		? std::max(requestedSampleRate, kMinimumSampleRate)
		: 48000.f;
	amountCoefficient = onePoleCoefficient(0.001f, sampleRate);
	autoDeflateCoefficient = onePoleCoefficient(0.010f, sampleRate);
	detectorAttackCoefficient = onePoleCoefficient(0.001f, sampleRate);
	detectorReleaseCoefficient = onePoleCoefficient(0.045f, sampleRate);
	detectorSlowCoefficient = onePoleCoefficient(0.180f, sampleRate);
	activityAttackCoefficient = onePoleCoefficient(0.005f, sampleRate);
	activityReleaseCoefficient = onePoleCoefficient(0.120f, sampleRate);
	limiterReleaseCoefficient = onePoleCoefficient(0.050f, sampleRate);
	dcCoefficient = std::exp(-2.f * kPi * 5.f / sampleRate);
	transitionLength = std::max(1, int(std::lround(0.005f * sampleRate)));
	reset();
}

void Engine::resetSharedControlState() {
	dynamics = {};
	inputActivity = 0.f;
	positiveInputActivity = 0.f;
	negativeInputActivity = 0.f;
	limiterGain = 1.f;
}

void Engine::reset() {
	upsamplerLeft.reset();
	upsamplerRight.reset();
	primaryPath.reset();
	secondaryPath.reset();
	primaryPath.dcLeft.coefficient = dcCoefficient;
	primaryPath.dcRight.coefficient = dcCoefficient;
	secondaryPath.dcLeft.coefficient = dcCoefficient;
	secondaryPath.dcRight.coefficient = dcCoefficient;
	resetSharedControlState();
	amount = 0.f;
	wetMix = 1.f;
	autoDeflateMix = 1.f;
	autoDeflateStateInitialized = false;
	cachedManualDeflate = -1.f;
	cachedManualGain = 1.f;
	currentCharacters = {};
	transitionFrom = {};
	transitionTo = {};
	pendingCharacters = {};
	transitionSample = 0;
	transitionActive = false;
	pendingCharacterActive = false;
}

void Engine::resetChannel(bool left) {
	if (left) {
		upsamplerLeft.reset();
		primaryPath.decimatorLeft.reset();
		secondaryPath.decimatorLeft.reset();
		primaryPath.dcLeft.reset();
		secondaryPath.dcLeft.reset();
	}
	else {
		upsamplerRight.reset();
		primaryPath.decimatorRight.reset();
		secondaryPath.decimatorRight.reset();
		primaryPath.dcRight.reset();
		secondaryPath.dcRight.reset();
	}
}

float Engine::updateFollower(
	float current,
	float target,
	float attack,
	float release) const {
	const float coefficient = target > current ? attack : release;
	return current + (target - current) * coefficient;
}

void Engine::beginCharacterTransition(CharacterPair requested) {
	if (transitionActive) {
		if (requested == transitionTo) {
			pendingCharacterActive = false;
			return;
		}
		if (requested == transitionFrom) {
			std::swap(primaryPath, secondaryPath);
			std::swap(transitionFrom, transitionTo);
			transitionSample = std::max(0, transitionLength - transitionSample);
			pendingCharacterActive = false;
			return;
		}
		pendingCharacters = requested;
		pendingCharacterActive = true;
		return;
	}
	if (requested == currentCharacters) {
		pendingCharacterActive = false;
		return;
	}
	transitionFrom = currentCharacters;
	transitionTo = requested;
	secondaryPath = primaryPath;
	transitionSample = 0;
	transitionActive = true;
}

Engine::CharacterCoefficients Engine::prepareCharacter(
	Character character,
	float amount,
	const DynamicsState& dynamicsState) {
	CharacterCoefficients coefficients;
	coefficients.character = character;
	coefficients.amount = clamp01(amount);
	const float a = coefficients.amount;
	switch (character) {
		case Character::Void:
			// At full Puff, open a 30%-of-reference dead zone. Squaring the
			// control keeps the first half of the knob nuanced.
			coefficients.voidThreshold = 0.30f * a * a;
			coefficients.voidInverseSpan =
				1.f / std::max(1.f - coefficients.voidThreshold, 1e-6f);
			break;
		case Character::Spine:
			coefficients.drive = 1.f + 9.f * a * a;
			break;
		case Character::Riptide:
			coefficients.drive = 1.f + 1.5f * a * a;
			break;
		case Character::Frenzy: {
			const float fastControl = clamp01(dynamicsState.fast);
			const float transient = clamp01(dynamicsState.transient);
			// Puff sweeps continuously from one to four complete sine-fold
			// cycles while contracting their vertical span. Dynamics bends the
			// phase asymmetrically without moving the origin or end anchors.
			coefficients.foldCycles = 1.f + 3.f * a;
			coefficients.foldGain = 1.f / (1.f + 0.5f * a);
			coefficients.phaseSkew =
				0.12f + 0.18f * fastControl + 0.12f * transient;
			break;
		}
		case Character::Bloom:
		default:
			coefficients.drive = 1.f + 4.f * a * a;
			coefficients.normalization = std::max(
				levi_math::tanhAudio(coefficients.drive), 1e-6f);
			break;
	}
	return coefficients;
}

float Engine::applyCharacter(
	float input,
	const CharacterCoefficients& coefficients) {
	const float a = coefficients.amount;
	switch (coefficients.character) {
		case Character::Void: {
			const float magnitude = std::fabs(input);
			const float kneePosition = clamp01(
				(magnitude - coefficients.voidThreshold)
				* coefficients.voidInverseSpan);
			// Smoothstep gives the dead-zone boundary and outer ceiling zero
			// slope, avoiding threshold clicks while retaining a visible void.
			const float knee = kneePosition * kneePosition
				* (3.f - 2.f * kneePosition);
			const float excavated = std::copysign(knee, input);
			return input + (excavated - input) * a;
		}
		case Character::Spine: {
			const float z = coefficients.drive * input;
			float saturated = 0.f;
			if (std::fabs(z) < 1.f) {
				saturated = z * (1.5f - 0.5f * z * z);
			}
			else {
				saturated = std::copysign(1.f, z);
			}
			return input + (saturated - input) * a;
		}
		case Character::Riptide: {
			const float saturated =
				fractalShape(coefficients.drive * input);
			return input + (saturated - input) * a;
		}
		case Character::Frenzy: {
			const float z = std::max(-1.f, std::min(input, 1.f));
			const float z2 = z * z;
			const float warped = z
				+ coefficients.phaseSkew * z2 * (1.f - z2);
			const float saturated = coefficients.foldGain * std::sin(
				kPi * coefficients.foldCycles * warped);
			return input + (saturated - input) * a;
		}
		case Character::Bloom:
		default: {
			const float saturated =
				levi_math::tanhAudio(coefficients.drive * input)
				/ coefficients.normalization;
			return input + (saturated - input) * a;
		}
	}
}

float Engine::processCharacter(
	Character character,
	float input,
	float amount,
	const DynamicsState& dynamicsState) {
	return applyCharacter(
		input,
		prepareCharacter(character, amount, dynamicsState));
}


float Engine::updateAutoGain(Character character, float currentAmount) const {
	const float exponent = autoDeflateDb(character, currentAmount)
		* 0.1151292546497023f;
	return fastNegativeExp(exponent);
}

float Engine::manualGain(float normalizedDeflate) {
	const float clamped = clamp01(normalizedDeflate);
	if (clamped != cachedManualDeflate) {
		cachedManualDeflate = clamped;
		cachedManualGain = std::pow(10.f, -12.f * clamped / 20.f);
	}
	return cachedManualGain;
}

float Engine::processPath(
	PathState& path,
	const CharacterCoefficients& negativeCoefficients,
	const CharacterCoefficients& positiveCoefficients,
	float* oversampledLeft,
	float* oversampledRight,
	float autoDeflateAmount,
	float wetAmount,
	bool left) {
	std::array<float, kOversampleFactor> shaped {};
	float* input = left ? oversampledLeft : oversampledRight;
	for (int i = 0; i < kOversampleFactor; ++i) {
		const CharacterCoefficients& coefficients = input[i] < 0.f
			? negativeCoefficients
			: positiveCoefficients;
		const float wet = applyCharacter(input[i], coefficients);
		shaped[static_cast<std::size_t>(i)] =
			input[i] + (wet - input[i]) * wetAmount;
	}
	float output = left
		? path.decimatorLeft.process(shaped.data())
		: path.decimatorRight.process(shaped.data());
	output = left ? path.dcLeft.process(output) : path.dcRight.process(output);
	// Keep compensation constant across both polarities. Polarity-dependent
	// gain would create a second, unintended asymmetric transfer function.
	const float negativeDb = autoDeflateDb(
		negativeCoefficients.character, negativeCoefficients.amount);
	const float positiveDb = autoDeflateDb(
		positiveCoefficients.character, positiveCoefficients.amount);
	const float compensationGain = fastNegativeExp(
		0.5f * (negativeDb + positiveDb) * 0.1151292546497023f);
	output *= 1.f + (compensationGain - 1.f)
		* clamp01(autoDeflateAmount) * wetAmount;
	return output;
}

Frame Engine::process(
	float inputLeft,
	float inputRight,
	float amountTarget,
	int negativeCharacter,
	int positiveCharacter,
	bool autoDeflate,
	float manualDeflate,
	float wetTarget) {
	const bool invalidLeft = !std::isfinite(inputLeft);
	const bool invalidRight = !std::isfinite(inputRight);
	if (invalidLeft) {
		inputLeft = 0.f;
		resetChannel(true);
	}
	if (invalidRight) {
		inputRight = 0.f;
		resetChannel(false);
	}
	if (invalidLeft || invalidRight) {
		resetSharedControlState();
	}

	inputLeft = std::max(-20.f, std::min(inputLeft, 20.f));
	inputRight = std::max(-20.f, std::min(inputRight, 20.f));
	const float normalizedPeak =
		std::max(std::fabs(inputLeft), std::fabs(inputRight)) / kReferenceVolts;
	const float normalizedPositive =
		std::max(0.f, std::max(inputLeft, inputRight)) / kReferenceVolts;
	const float normalizedNegative =
		std::max(0.f, std::max(-inputLeft, -inputRight)) / kReferenceVolts;
	dynamics.fast = updateFollower(
		dynamics.fast,
		normalizedPeak,
		detectorAttackCoefficient,
		detectorReleaseCoefficient);
	dynamics.slowSq +=
		(normalizedPeak * normalizedPeak - dynamics.slowSq) * detectorSlowCoefficient;
	const float slow = std::sqrt(std::max(dynamics.slowSq, 0.f));
	dynamics.transient = clamp01(
		(dynamics.fast / std::max(slow, 1e-4f) - 1.f) * 0.5f);
	inputActivity = updateFollower(
		inputActivity,
		clamp01(normalizedPeak),
		activityAttackCoefficient,
		activityReleaseCoefficient);
	positiveInputActivity = updateFollower(
		positiveInputActivity,
		std::min(normalizedPositive, 1.25f),
		activityAttackCoefficient,
		activityReleaseCoefficient);
	negativeInputActivity = updateFollower(
		negativeInputActivity,
		std::min(normalizedNegative, 1.25f),
		activityAttackCoefficient,
		activityReleaseCoefficient);

	const float safeTarget = std::isfinite(amountTarget) ? clamp01(amountTarget) : 0.f;
	amount += (safeTarget - amount) * amountCoefficient;
	const float safeWetTarget = std::isfinite(wetTarget) ? clamp01(wetTarget) : 1.f;
	wetMix += (safeWetTarget - wetMix) * amountCoefficient;
	const float autoDeflateTarget = autoDeflate ? 1.f : 0.f;
	if (!autoDeflateStateInitialized) {
		autoDeflateMix = autoDeflateTarget;
		autoDeflateStateInitialized = true;
	}
	else {
		autoDeflateMix +=
			(autoDeflateTarget - autoDeflateMix) * autoDeflateCoefficient;
	}
	const CharacterPair requestedCharacters {
		clampCharacter(negativeCharacter),
		clampCharacter(positiveCharacter)
	};
	beginCharacterTransition(requestedCharacters);

	std::array<float, kOversampleFactor> oversampledLeft {};
	std::array<float, kOversampleFactor> oversampledRight {};
	upsamplerLeft.process(inputLeft / kReferenceVolts, oversampledLeft.data());
	upsamplerRight.process(inputRight / kReferenceVolts, oversampledRight.data());

	float normalizedLeft = 0.f;
	float normalizedRight = 0.f;
	if (transitionActive) {
		const CharacterCoefficients oldNegativeCoefficients =
			prepareCharacter(transitionFrom.negative, amount, dynamics);
		const CharacterCoefficients oldPositiveCoefficients =
			prepareCharacter(transitionFrom.positive, amount, dynamics);
		const CharacterCoefficients newNegativeCoefficients =
			prepareCharacter(transitionTo.negative, amount, dynamics);
		const CharacterCoefficients newPositiveCoefficients =
			prepareCharacter(transitionTo.positive, amount, dynamics);
		const float oldLeft = processPath(
			primaryPath, oldNegativeCoefficients, oldPositiveCoefficients,
			oversampledLeft.data(),
			oversampledRight.data(), autoDeflateMix, wetMix, true);
		const float oldRight = processPath(
			primaryPath, oldNegativeCoefficients, oldPositiveCoefficients,
			oversampledLeft.data(),
			oversampledRight.data(), autoDeflateMix, wetMix, false);
		const float newLeft = processPath(
			secondaryPath, newNegativeCoefficients, newPositiveCoefficients,
			oversampledLeft.data(),
			oversampledRight.data(), autoDeflateMix, wetMix, true);
		const float newRight = processPath(
			secondaryPath, newNegativeCoefficients, newPositiveCoefficients,
			oversampledLeft.data(),
			oversampledRight.data(), autoDeflateMix, wetMix, false);
		const float t = clamp01(
			float(transitionSample) / float(std::max(transitionLength - 1, 1)));
		const float oldGain = 1.f - t;
		const float newGain = t;
		normalizedLeft = oldLeft * oldGain + newLeft * newGain;
		normalizedRight = oldRight * oldGain + newRight * newGain;
		transitionSample++;
		if (transitionSample >= transitionLength) {
			primaryPath = secondaryPath;
			currentCharacters = transitionTo;
			transitionActive = false;
			if (pendingCharacterActive) {
				const CharacterPair queuedCharacters = pendingCharacters;
				pendingCharacterActive = false;
				beginCharacterTransition(queuedCharacters);
			}
		}
	}
	else {
		const CharacterCoefficients negativeCoefficients =
			prepareCharacter(currentCharacters.negative, amount, dynamics);
		const CharacterCoefficients positiveCoefficients =
			prepareCharacter(currentCharacters.positive, amount, dynamics);
		normalizedLeft = processPath(
			primaryPath, negativeCoefficients, positiveCoefficients,
			oversampledLeft.data(),
			oversampledRight.data(), autoDeflateMix, wetMix, true);
		normalizedRight = processPath(
			primaryPath, negativeCoefficients, positiveCoefficients,
			oversampledLeft.data(),
			oversampledRight.data(), autoDeflateMix, wetMix, false);
	}

	float outputLeft = normalizedLeft * kReferenceVolts;
	float outputRight = normalizedRight * kReferenceVolts;
	const float peak = std::max(std::fabs(outputLeft), std::fabs(outputRight));
	const float desiredGain = peak > kLiveCeilingVolts
		? kLiveCeilingVolts / std::max(peak, std::numeric_limits<float>::min())
		: 1.f;
	limiterGain += (1.f - limiterGain) * limiterReleaseCoefficient;
	limiterGain = std::min(limiterGain, desiredGain);
	outputLeft *= limiterGain;
	outputRight *= limiterGain;

	const float limitedPeak = std::max(std::fabs(outputLeft), std::fabs(outputRight));
	if (limitedPeak > kLiveCeilingVolts) {
		const float guard = kLiveCeilingVolts / limitedPeak;
		outputLeft *= guard;
		outputRight *= guard;
		limiterGain *= guard;
	}

	const float outputGain = manualGain(manualDeflate);
	outputLeft *= outputGain;
	outputRight *= outputGain;
	if (!std::isfinite(outputLeft) || !std::isfinite(outputRight)) {
		outputLeft = 0.f;
		outputRight = 0.f;
		reset();
	}

	Frame frame;
	frame.left = outputLeft;
	frame.right = outputRight;
	frame.effectiveAmount = amount;
	frame.wetMix = wetMix;
	frame.inputActivity = clamp01(inputActivity);
	frame.positiveInputActivity =
		std::max(0.f, std::min(positiveInputActivity, 1.25f));
	frame.negativeInputActivity =
		std::max(0.f, std::min(negativeInputActivity, 1.25f));
	frame.transientActivity = clamp01(dynamics.transient);
	frame.limiterGain = clamp01(limiterGain);
	const CharacterPair displayedCharacters =
		transitionActive ? transitionTo : currentCharacters;
	frame.negativeCharacter = int(displayedCharacters.negative);
	frame.positiveCharacter = int(displayedCharacters.positive);
	return frame;
}

} // namespace puffy

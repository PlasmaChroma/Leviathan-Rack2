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
	if (character >= int(Character::Frenzy)) {
		return Character::Frenzy;
	}
	return static_cast<Character>(character);
}

float autoDeflateDb(Character character, float amount) {
	switch (character) {
		case Character::Spine:
			return -4.f * amount;
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
	detectorAttackCoefficient = onePoleCoefficient(0.001f, sampleRate);
	detectorReleaseCoefficient = onePoleCoefficient(0.045f, sampleRate);
	detectorSlowCoefficient = onePoleCoefficient(0.180f, sampleRate);
	activityAttackCoefficient = onePoleCoefficient(0.005f, sampleRate);
	activityReleaseCoefficient = onePoleCoefficient(0.120f, sampleRate);
	limiterReleaseCoefficient = onePoleCoefficient(0.050f, sampleRate);
	dcCoefficient = std::exp(-2.f * kPi * 5.f / sampleRate);
	transitionLength = std::max(1, int(std::lround(0.010f * sampleRate)));
	reset();
}

void Engine::resetSharedControlState() {
	dynamics = {};
	inputActivity = 0.f;
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
	cachedManualDeflate = -1.f;
	cachedManualGain = 1.f;
	currentCharacter = Character::Bloom;
	transitionFrom = Character::Bloom;
	transitionTo = Character::Bloom;
	transitionSample = 0;
	transitionActive = false;
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

void Engine::beginCharacterTransition(Character requested) {
	if (transitionActive) {
		if (requested == transitionTo) {
			return;
		}
		if (transitionSample * 2 >= transitionLength) {
			primaryPath = secondaryPath;
			currentCharacter = transitionTo;
		}
		transitionActive = false;
	}
	if (requested == currentCharacter) {
		return;
	}
	transitionFrom = currentCharacter;
	transitionTo = requested;
	secondaryPath = primaryPath;
	transitionSample = 0;
	transitionActive = true;
}

float Engine::processCharacter(
	Character character,
	float input,
	float amount,
	const DynamicsState& dynamicsState) {
	const float a = clamp01(amount);
	switch (character) {
		case Character::Spine: {
			const float drive = 1.f + 9.f * a * a;
			const float z = drive * input;
			float saturated = 0.f;
			if (std::fabs(z) < 1.f) {
				saturated = z * (1.5f - 0.5f * z * z);
			}
			else {
				saturated = std::copysign(1.f, z);
			}
			return input + (saturated - input) * a;
		}
		case Character::Frenzy: {
			const float fastControl = clamp01(dynamicsState.fast);
			const float transient = clamp01(dynamicsState.transient);
			const float drive = 1.f + 6.f * a * a
				* (0.65f + 0.55f * fastControl + 0.35f * transient);
			const float bias = 0.12f * a * (0.25f + 0.75f * fastControl);
			const float zero = levi_math::tanhAudio(drive * bias);
			const float positiveNorm = std::max(
				levi_math::tanhAudio(drive * (1.f + bias)) - zero, 1e-4f);
			const float negativeNorm = std::max(
				zero - levi_math::tanhAudio(drive * (-1.f + bias)), 1e-4f);
			const float raw = levi_math::tanhAudio(drive * (input + bias)) - zero;
			float saturated = raw >= 0.f ? raw / positiveNorm : raw / negativeNorm;
			if (saturated < 0.f) {
				saturated *= 1.f + 0.10f * a * fastControl;
			}
			saturated = std::max(-1.25f, std::min(saturated, 1.25f));
			return input + (saturated - input) * a;
		}
		case Character::Bloom:
		default: {
			const float drive = 1.f + 4.f * a * a;
			const float normalization = std::max(levi_math::tanhAudio(drive), 1e-6f);
			const float saturated = levi_math::tanhAudio(drive * input) / normalization;
			return input + (saturated - input) * a;
		}
	}
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
	Character character,
	float* oversampledLeft,
	float* oversampledRight,
	float currentAmount,
	bool autoDeflate,
	bool left) {
	std::array<float, kOversampleFactor> shaped {};
	float* input = left ? oversampledLeft : oversampledRight;
	for (int i = 0; i < kOversampleFactor; ++i) {
		shaped[static_cast<std::size_t>(i)] =
			processCharacter(character, input[i], currentAmount, dynamics);
	}
	float output = left
		? path.decimatorLeft.process(shaped.data())
		: path.decimatorRight.process(shaped.data());
	output = left ? path.dcLeft.process(output) : path.dcRight.process(output);
	if (autoDeflate) {
		output *= updateAutoGain(character, currentAmount);
	}
	return output;
}

Frame Engine::process(
	float inputLeft,
	float inputRight,
	float amountTarget,
	int character,
	bool autoDeflate,
	float manualDeflate) {
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

	const float safeTarget = std::isfinite(amountTarget) ? clamp01(amountTarget) : 0.f;
	amount += (safeTarget - amount) * amountCoefficient;
	const Character requestedCharacter = clampCharacter(character);
	beginCharacterTransition(requestedCharacter);

	std::array<float, kOversampleFactor> oversampledLeft {};
	std::array<float, kOversampleFactor> oversampledRight {};
	upsamplerLeft.process(inputLeft / kReferenceVolts, oversampledLeft.data());
	upsamplerRight.process(inputRight / kReferenceVolts, oversampledRight.data());

	float normalizedLeft = 0.f;
	float normalizedRight = 0.f;
	if (transitionActive) {
		const float oldLeft = processPath(
			primaryPath, transitionFrom, oversampledLeft.data(),
			oversampledRight.data(), amount, autoDeflate, true);
		const float oldRight = processPath(
			primaryPath, transitionFrom, oversampledLeft.data(),
			oversampledRight.data(), amount, autoDeflate, false);
		const float newLeft = processPath(
			secondaryPath, transitionTo, oversampledLeft.data(),
			oversampledRight.data(), amount, autoDeflate, true);
		const float newRight = processPath(
			secondaryPath, transitionTo, oversampledLeft.data(),
			oversampledRight.data(), amount, autoDeflate, false);
		const float t = clamp01(
			float(transitionSample) / float(std::max(transitionLength - 1, 1)));
		const float oldGain = std::sqrt(1.f - t);
		const float newGain = std::sqrt(t);
		normalizedLeft = oldLeft * oldGain + newLeft * newGain;
		normalizedRight = oldRight * oldGain + newRight * newGain;
		transitionSample++;
		if (transitionSample >= transitionLength) {
			primaryPath = secondaryPath;
			currentCharacter = transitionTo;
			transitionActive = false;
		}
	}
	else {
		normalizedLeft = processPath(
			primaryPath, currentCharacter, oversampledLeft.data(),
			oversampledRight.data(), amount, autoDeflate, true);
		normalizedRight = processPath(
			primaryPath, currentCharacter, oversampledLeft.data(),
			oversampledRight.data(), amount, autoDeflate, false);
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
	frame.inputActivity = clamp01(inputActivity);
	frame.transientActivity = clamp01(dynamics.transient);
	frame.limiterGain = clamp01(limiterGain);
	frame.character = int(transitionActive ? transitionTo : currentCharacter);
	return frame;
}

} // namespace puffy

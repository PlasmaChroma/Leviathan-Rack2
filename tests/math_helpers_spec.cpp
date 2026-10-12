#include "../src/MathHelpers.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

namespace {

bool check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	return condition;
}

} // namespace

int main() {
	int failures = 0;

	const bool legacyAnchors =
		std::fabs(levi_math::tanhLegacy(0.5f) - 0.46581197f) < 1e-7f
		&& std::fabs(levi_math::tanhLegacy(1.f) - 0.77777779f) < 1e-7f
		&& std::fabs(levi_math::tanhLegacy(2.f) - 0.98412699f) < 1e-7f
		&& levi_math::tanhLegacy(3.f) == 1.f
		&& levi_math::tanhLegacy(-3.f) == -1.f;
	failures += !check(legacyAnchors, "legacy tanh curve anchors remain stable");

	float maximumError = 0.f;
	float previous = -1.f;
	bool monotonic = true;
	bool odd = true;
	bool bounded = true;
	constexpr int sampleCount = 400000;
	for (int i = 0; i <= sampleCount; ++i) {
		const float x = -12.f + 24.f * float(i) / float(sampleCount);
		const float approximation = levi_math::tanhAudio(x);
		maximumError = std::max(
			maximumError,
			std::fabs(approximation - std::tanh(x)));
		monotonic = monotonic && approximation >= previous;
		odd = odd
			&& std::fabs(approximation + levi_math::tanhAudio(-x)) < 1e-7f;
		bounded = bounded && std::isfinite(approximation)
			&& std::fabs(approximation) <= 1.f;
		previous = approximation;
	}
	failures += !check(
		maximumError < 6.1e-6f,
		"audio tanh LUT stays within its accuracy contract");
	failures += !check(monotonic && odd && bounded,
		"audio tanh LUT is monotonic, odd, finite, and bounded");

	const float positiveInfinity = levi_math::tanhAudio(
		std::numeric_limits<float>::infinity());
	const float negativeInfinity = levi_math::tanhAudio(
		-std::numeric_limits<float>::infinity());
	const float notANumber = levi_math::tanhAudio(
		std::numeric_limits<float>::quiet_NaN());
	failures += !check(
		positiveInfinity == 1.f && negativeInfinity == -1.f
			&& std::isnan(notANumber),
		"audio tanh handles infinities and propagates NaN");

	const std::uintptr_t tableAddress = reinterpret_cast<std::uintptr_t>(
		levi_math::detail::audioTanhLut.data());
	failures += !check(
		tableAddress % 64u == 0u,
		"shared audio tanh table is cache-line aligned");

	float maximumSineError = 0.f;
	float maximumSineOddError = 0.f;
	constexpr float kTwoPi = 6.28318530717958647692f;
	for (int i = 0; i <= sampleCount; ++i) {
		const float cycles = -2.f + 4.f * float(i) / float(sampleCount);
		const float approximation = levi_math::sinCyclesAudioBounded(cycles);
		maximumSineError = std::max(
			maximumSineError,
			std::fabs(approximation - std::sin(kTwoPi * cycles)));
		maximumSineOddError = std::max(
			maximumSineOddError,
			std::fabs(
				approximation + levi_math::sinCyclesAudioBounded(-cycles)));
	}
	failures += !check(
		maximumSineError < 1.7e-6f && maximumSineOddError < 2e-6f,
		"audio sine LUT stays accurate and odd across FRENZY's phase range");
	const std::uintptr_t sineTableAddress = reinterpret_cast<std::uintptr_t>(
		levi_math::detail::audioSineLut.data());
	failures += !check(
		sineTableAddress % 64u == 0u,
		"shared audio sine table is cache-line aligned");

	const bool triangleAnchors =
		levi_math::triangleCyclesAudioBounded(-2.f) == 0.f
		&& levi_math::triangleCyclesAudioBounded(-1.75f) == 1.f
		&& levi_math::triangleCyclesAudioBounded(-1.5f) == 0.f
		&& levi_math::triangleCyclesAudioBounded(-1.25f) == -1.f
		&& levi_math::triangleCyclesAudioBounded(0.f) == 0.f
		&& levi_math::triangleCyclesAudioBounded(0.25f) == 1.f
		&& levi_math::triangleCyclesAudioBounded(0.5f) == 0.f
		&& levi_math::triangleCyclesAudioBounded(0.75f) == -1.f
		&& levi_math::triangleCyclesAudioBounded(2.f) == 0.f;
	bool triangleOddAndBounded = true;
	for (int i = 0; i <= sampleCount; ++i) {
		const float cycles = -2.f + 4.f * float(i) / float(sampleCount);
		const float triangle = levi_math::triangleCyclesAudioBounded(cycles);
		triangleOddAndBounded = triangleOddAndBounded
			&& std::fabs(triangle) <= 1.f
			&& std::fabs(
				triangle + levi_math::triangleCyclesAudioBounded(-cycles)) < 2e-6f;
	}
	failures += !check(
		triangleAnchors && triangleOddAndBounded,
		"bounded triangle carrier has exact corners, odd symmetry, and unit bounds");

	std::cout << "max audio tanh error: " << maximumError << "\n";
	std::cout << "max audio sine error: " << maximumSineError << "\n";
	std::cout << "max audio sine odd error: " << maximumSineOddError << "\n";
	std::cout << "Summary: " << (8 - failures) << "/8 passed\n";
	return failures == 0 ? 0 : 1;
}

#include "../src/FastMath.hpp"

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

	std::cout << "max audio tanh error: " << maximumError << "\n";
	std::cout << "Summary: " << (5 - failures) << "/5 passed\n";
	return failures == 0 ? 0 : 1;
}

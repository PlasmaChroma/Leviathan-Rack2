#include "../src/visual/ApertureLightTransfer.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

int main() {
	float maxGlowError = 0.f;
	float maxCoreError = 0.f;
	float maxHotError = 0.f;
	for (int i = 0; i <= 1000000; ++i) {
		const float t = float(i) / 1000000.f;
		const aperture_light::Transfer transfer = aperture_light::transferFromBrightness(t);
		maxGlowError = std::max(maxGlowError, std::fabs(transfer.glow - std::pow(t, 0.55f)));
		maxCoreError = std::max(maxCoreError, std::fabs(transfer.core - std::pow(t, 0.85f)));
		maxHotError = std::max(maxHotError, std::fabs(transfer.hot - std::pow(t, 3.f)));
	}

	const bool endpointsCorrect =
		aperture_light::transferFromBrightness(-1.f).glow == 0.f
		&& aperture_light::transferFromBrightness(2.f).glow == 1.f;
	const bool errorsAcceptable =
		maxGlowError < 0.006f
		&& maxCoreError < 0.0005f
		&& maxHotError < 1e-6f;

	std::cout << "Aperture Light Transfer Spec\n"
		<< "max glow error: " << maxGlowError << "\n"
		<< "max core error: " << maxCoreError << "\n"
		<< "max hot error: " << maxHotError << "\n";
	if (!endpointsCorrect || !errorsAcceptable) {
		std::cerr << "[FAIL] LUT accuracy or clamping\n";
		return 1;
	}
	std::cout << "[PASS] LUT accuracy and clamping\n";
	return 0;
}

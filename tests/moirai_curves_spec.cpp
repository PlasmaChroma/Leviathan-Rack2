#include "MoiraiCurves.hpp"

#include <cmath>
#include <iostream>

namespace {
int failures = 0;
void check(bool condition, const char* name) {
	std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
	if (!condition) ++failures;
}
}

int main() {
	const moirai::CurveType curveTypes[] = {
		moirai::CurveType::LINEAR, moirai::CurveType::SMOOTHSTEP,
		moirai::CurveType::SIGMOID, moirai::CurveType::HOLD,
		moirai::CurveType::STEP, moirai::CurveType::EXPONENTIAL,
		moirai::CurveType::LOGARITHMIC
	};
	for (moirai::CurveType type : curveTypes) {
		float previous = -1.f;
		bool finiteAndBounded = true;
		bool monotone = true;
		for (int index = 0; index <= 1000; ++index) {
			const float phase = static_cast<float>(index) / 1000.f;
			const float value = moirai::shapeCurve(phase, type, 0.7f);
			finiteAndBounded = finiteAndBounded && std::isfinite(value) && value >= 0.f && value <= 1.f;
			monotone = monotone && value + 1e-6f >= previous;
			previous = value;
		}
		check(finiteAndBounded, "curve remains finite and bounded");
		check(monotone, "curve remains monotone");
		check(std::abs(moirai::shapeCurve(0.f, type, 0.7f)
			- (type == moirai::CurveType::STEP ? 1.f : 0.f)) < 1e-6f,
			"curve has its contracted start endpoint");
		check(std::abs(moirai::shapeCurve(1.f, type, 0.7f) - 1.f) < 1e-6f,
			"curve reaches its exact target endpoint");
	}
	check(moirai::powerBias(0.25f, 1.f) < 0.25f && moirai::powerBias(0.25f, -1.f) > 0.25f,
		"power bias mirrors slow-start and fast-start shapes");
	check(std::abs(moirai::evaluateSegment(0.25f, 0.75f, 0.f,
		moirai::CurveType::HOLD, 0.f) - 0.25f) < 1e-6f &&
		std::abs(moirai::evaluateSegment(0.25f, 0.75f, 1.f,
			moirai::CurveType::HOLD, 0.f) - 0.75f) < 1e-6f,
		"hold curve retains the segment start until completion");

	moirai::CompiledProgram contour;
	contour.kind = moirai::ProgramKind::CONTOUR;
	contour.interpolation = moirai::Interpolation::MONOTONE_CUBIC;
	contour.points.push_back(moirai::CompiledContourPoint(0.f, 0.f, 4.f));
	contour.points.push_back(moirai::CompiledContourPoint(0.25f, 1.f, 0.f));
	contour.points.push_back(moirai::CompiledContourPoint(0.75f, 0.2f, 0.f));
	contour.points.push_back(moirai::CompiledContourPoint(1.f, 0.f, -1.f));
	bool contourBounded = true;
	for (int index = -100; index <= 1100; ++index) {
		const float value = moirai::evaluateContour(contour, static_cast<float>(index) / 1000.f);
		contourBounded = contourBounded && std::isfinite(value) && value >= 0.f && value <= 1.f;
	}
	check(contourBounded, "monotone contour interpolation clamps every segment against overshoot");
	check(moirai::evaluateContour(contour, -0.5f) == 0.f && moirai::evaluateContour(contour, 1.5f) == 0.f,
		"ordinary contour evaluation clamps rather than wrapping");
	std::cout << (failures ? "[SUMMARY] moirai_curves_spec: FAILED\n" : "[SUMMARY] moirai_curves_spec: passed\n");
	return failures ? 1 : 0;
}

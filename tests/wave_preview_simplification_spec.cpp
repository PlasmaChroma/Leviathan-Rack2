#include "../src/WavePreviewSimplifier.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include <string>

namespace {

struct TestResult {
	std::string name;
	bool pass = false;
	std::string detail;
};

struct TestPoint {
	float x;
	float y;
};

TestResult testStraightLineSimplifiesToEnds() {
	constexpr int POINT_COUNT = 128;
	std::vector<TestPoint> points(POINT_COUNT);
	for (int i = 0; i < POINT_COUNT; ++i) {
		points[i] = {float(i), 5.f + 2.f * float(i)}; // y = 5 + 2x
	}

	std::vector<TestPoint> simplified;
	wave_preview::simplifyPath(points.data(), points.size(), 1, 0.01f, [&](const TestPoint& pt, bool isMove) {
		(void)isMove;
		simplified.push_back(pt);
	});

	bool pass = (simplified.size() == 2) &&
	             (simplified[0].x == points[0].x && simplified[0].y == points[0].y) &&
	             (simplified[1].x == points[POINT_COUNT - 1].x && simplified[1].y == points[POINT_COUNT - 1].y);

	std::string detail = "Reduced " + std::to_string(POINT_COUNT) + " points to " + std::to_string(simplified.size());
	return {"Straight line simplifies to 2 endpoints", pass, detail};
}

TestResult testTriangleWaveSimplifiesToThreePoints() {
	constexpr int POINT_COUNT = 128;
	std::vector<TestPoint> points(POINT_COUNT);
	// Rise from 0 to 64, then fall from 64 to 127
	for (int i = 0; i < POINT_COUNT; ++i) {
		if (i <= 64) {
			points[i] = {float(i), float(i)};
		} else {
			points[i] = {float(i), float(128 - i)};
		}
	}

	std::vector<TestPoint> simplified;
	wave_preview::simplifyPath(points.data(), points.size(), 1, 0.01f, [&](const TestPoint& pt, bool isMove) {
		(void)isMove;
		simplified.push_back(pt);
	});

	bool pass = (simplified.size() == 3) &&
	             (simplified[0].x == 0.f && simplified[0].y == 0.f) &&
	             (simplified[1].x == 64.f && simplified[1].y == 64.f) &&
	             (simplified[2].x == 127.f && simplified[2].y == 1.f);

	std::string detail = "Reduced " + std::to_string(POINT_COUNT) + " points to " + std::to_string(simplified.size()) +
	                     " (Peak at x=" + std::to_string(simplified[1].x) + ", y=" + std::to_string(simplified[1].y) + ")";
	return {"Triangle wave simplifies to 3 peak/endpoint vertices", pass, detail};
}

TestResult testSineWaveSimplification() {
	constexpr int POINT_COUNT = 128;
	std::vector<TestPoint> points(POINT_COUNT);
	for (int i = 0; i < POINT_COUNT; ++i) {
		float phase = float(i) / float(POINT_COUNT - 1) * 2.f * M_PI;
		points[i] = {float(i), std::sin(phase) * 50.f};
	}

	std::string detail;
	float tolerances[] = {1.0f, 0.5f, 0.25f, 0.1f, 0.05f, 0.02f, 0.01f, 0.005f};
	for (float tol : tolerances) {
		std::vector<TestPoint> simplified;
		wave_preview::simplifyPath(points.data(), points.size(), 1, tol, [&](const TestPoint& pt, bool isMove) {
			(void)isMove;
			simplified.push_back(pt);
		});
		detail += "tol=" + std::to_string(tol) + "->" + std::to_string(simplified.size()) + " ";
	}

	return {"Sine wave simplification counts over tolerance sweep", true, detail};
}
TestResult testLargePointCountFallback() {
	constexpr int POINT_COUNT = 1000; // Greater than 512, forcing heap allocation fallback
	std::vector<TestPoint> points(POINT_COUNT);
	for (int i = 0; i < POINT_COUNT; ++i) {
		points[i] = {float(i), 10.f - float(i)}; // y = 10 - x
	}

	std::vector<TestPoint> simplified;
	wave_preview::simplifyPath(points.data(), points.size(), 1, 0.01f, [&](const TestPoint& pt, bool isMove) {
		(void)isMove;
		simplified.push_back(pt);
	});

	bool pass = (simplified.size() == 2) &&
	             (simplified[0].x == points[0].x && simplified[0].y == points[0].y) &&
	             (simplified[1].x == points[POINT_COUNT - 1].x && simplified[1].y == points[POINT_COUNT - 1].y);

	std::string detail = "Reduced " + std::to_string(POINT_COUNT) + " points to " + std::to_string(simplified.size()) + " using heap fallback";
	return {"Large point count fallback (1000 points)", pass, detail};
}

} // namespace

int main() {
	std::vector<TestResult> tests;
	tests.push_back(testStraightLineSimplifiesToEnds());
	tests.push_back(testTriangleWaveSimplifiesToThreePoints());
	tests.push_back(testSineWaveSimplification());
	tests.push_back(testLargePointCountFallback());

	int failed = 0;
	std::cout << "Wave Preview Simplification Spec\n";
	std::cout << "--------------------------------\n";
	for (const TestResult& test : tests) {
		std::cout << (test.pass ? "[PASS] " : "[FAIL] ") << test.name << " :: " << test.detail << "\n";
		if (!test.pass) {
			failed++;
		}
	}
	std::cout << "--------------------------------\n";
	std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}

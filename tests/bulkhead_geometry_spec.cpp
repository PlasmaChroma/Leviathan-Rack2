#include "../src/BulkheadGeometry.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct TestResult {
	std::string name;
	bool pass = false;
	std::string detail;
};

bool nearlyEqual(float a, float b, float eps = 1e-5f) {
	return std::fabs(a - b) <= eps;
}

TestResult testMirrorLeftWall() {
	const bulkhead::geometry::RoomBounds room {-4.f, 4.f, -3.f, 3.f};
	const bulkhead::geometry::Vec2 source {1.f, 2.f};
	const bulkhead::geometry::Vec2 image =
		bulkhead::geometry::mirrorSourceAcrossWall(room, source, bulkhead::geometry::WALL_LEFT);
	const bool pass = nearlyEqual(image.x, -9.f) && nearlyEqual(image.y, 2.f);
	return {"Mirror left wall", pass, "x=" + std::to_string(image.x) + " y=" + std::to_string(image.y)};
}

TestResult testFirstOrderImageSources() {
	const bulkhead::geometry::RoomBounds room {-4.f, 4.f, -2.f, 2.f};
	const bulkhead::geometry::Vec2 source {1.f, -1.f};
	const auto images = bulkhead::geometry::firstOrderImageSources(room, source);
	const bool pass =
		nearlyEqual(images[bulkhead::geometry::WALL_LEFT].x, -9.f) &&
		nearlyEqual(images[bulkhead::geometry::WALL_RIGHT].x, 7.f) &&
		nearlyEqual(images[bulkhead::geometry::WALL_FRONT].y, 5.f) &&
		nearlyEqual(images[bulkhead::geometry::WALL_BACK].y, -3.f);
	return {"First-order image sources", pass,
		"lx=" + std::to_string(images[bulkhead::geometry::WALL_LEFT].x) +
		" rx=" + std::to_string(images[bulkhead::geometry::WALL_RIGHT].x)};
}

TestResult testReflectionDistanceSymmetry() {
	const bulkhead::geometry::RoomBounds room {-4.f, 4.f, -2.f, 2.f};
	const bulkhead::geometry::Vec2 source {-2.f, 0.f};
	const bulkhead::geometry::Vec2 listener {2.f, 0.f};
	const auto distances = bulkhead::geometry::firstOrderReflectionDistances(room, source, listener);
	const bool pass =
		nearlyEqual(distances[bulkhead::geometry::WALL_LEFT], 8.f) &&
		nearlyEqual(distances[bulkhead::geometry::WALL_RIGHT], 8.f) &&
		nearlyEqual(distances[bulkhead::geometry::WALL_FRONT], std::sqrt(32.f)) &&
		nearlyEqual(distances[bulkhead::geometry::WALL_BACK], std::sqrt(32.f));
	return {"Reflection distance symmetry", pass,
		"left=" + std::to_string(distances[bulkhead::geometry::WALL_LEFT]) +
		" right=" + std::to_string(distances[bulkhead::geometry::WALL_RIGHT])};
}

} // namespace

int main() {
	std::vector<TestResult> tests;
	tests.push_back(testMirrorLeftWall());
	tests.push_back(testFirstOrderImageSources());
	tests.push_back(testReflectionDistanceSymmetry());

	int failed = 0;
	std::cout << "Bulkhead Geometry Spec\n";
	std::cout << "----------------------\n";
	for (const auto& test : tests) {
		std::cout << (test.pass ? "[PASS] " : "[FAIL] ") << test.name << " :: " << test.detail << "\n";
		if (!test.pass) {
			failed++;
		}
	}
	std::cout << "----------------------\n";
	std::cout << "Summary: " << (tests.size() - failed) << "/" << tests.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}


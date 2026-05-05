#include "../src/SilMicropeak.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct TestResult {
	std::string name;
	bool passed = false;
	std::string detail;
};

TestResult expectCleanSineNotDetected() {
	const int n = 4096;
	std::vector<float> l(size_t(n), 0.f);
	std::vector<float> r(size_t(n), 0.f);
	for (int i = 0; i < n; ++i) {
		const float v = 2.0f * std::sin(2.f * 3.14159265358979323846f * float(i) / 96.f);
		l[size_t(i)] = v;
		r[size_t(i)] = v;
	}
	sil_micropeak::Result result = sil_micropeak::analyzeChunk(l.data(), r.data(), l.size(), 5.f);
	return {"Clean sine is not flagged", !result.detected,
	        "events=" + std::to_string(result.eventCount) + " severity=" + std::to_string(result.strongestSeverity)};
}

TestResult expectIsolatedSpikesDetected() {
	const int n = 4096;
	std::vector<float> l(size_t(n), 0.08f);
	std::vector<float> r(size_t(n), 0.08f);
	const int positions[] = {768, 1900, 3072};
	for (int pos : positions) {
		l[size_t(pos)] = 4.7f;
		r[size_t(pos)] = -4.65f;
	}
	sil_micropeak::Result result = sil_micropeak::analyzeChunk(l.data(), r.data(), l.size(), 5.f);
	return {"Repeated isolated micropeaks are flagged", result.detected && result.eventCount >= 3,
	        "events=" + std::to_string(result.eventCount) + " severity=" + std::to_string(result.strongestSeverity)};
}

TestResult expectBroadTransientNotDetected() {
	const int n = 4096;
	std::vector<float> l(size_t(n), 0.f);
	std::vector<float> r(size_t(n), 0.f);
	for (int i = 1800; i < 1820; ++i) {
		float envelope = 1.f - std::fabs(float(i - 1810)) / 10.f;
		l[size_t(i)] = 4.2f * std::max(envelope, 0.f);
		r[size_t(i)] = l[size_t(i)];
	}
	sil_micropeak::Result result = sil_micropeak::analyzeChunk(l.data(), r.data(), l.size(), 5.f);
	return {"Broad transient is not treated as a micropeak", !result.detected,
	        "events=" + std::to_string(result.eventCount) + " severity=" + std::to_string(result.strongestSeverity)};
}

TestResult expectCleanupRepairsIsolatedSpikeWhenActive() {
	const sil_micropeak::StereoSample previous {0.10f, -0.08f};
	const sil_micropeak::StereoSample center {4.80f, -4.70f};
	const sil_micropeak::StereoSample next {0.14f, -0.10f};
	const sil_micropeak::StereoSample repaired = sil_micropeak::repairMicropeak(previous, center, next, 5.f);
	const float expectedL = 0.5f * (previous.l + next.l);
	const float expectedR = 0.5f * (previous.r + next.r);
	const bool passed = std::fabs(repaired.l - expectedL) < 1e-6f && std::fabs(repaired.r - expectedR) < 1e-6f;
	return {"Cleanup interpolates isolated micropeak",
	        passed,
	        "l=" + std::to_string(repaired.l) + " r=" + std::to_string(repaired.r)};
}

TestResult expectCleanupLeavesBroadTransient() {
	const sil_micropeak::StereoSample previous {3.80f, 3.75f};
	const sil_micropeak::StereoSample center {4.10f, 4.05f};
	const sil_micropeak::StereoSample next {3.70f, 3.65f};
	const sil_micropeak::StereoSample repaired = sil_micropeak::repairMicropeak(previous, center, next, 5.f);
	const bool passed = std::fabs(repaired.l - center.l) < 1e-6f && std::fabs(repaired.r - center.r) < 1e-6f;
	return {"Cleanup leaves broad transient unchanged",
	        passed,
	        "l=" + std::to_string(repaired.l) + " r=" + std::to_string(repaired.r)};
}

TestResult expectStatefulCleanupOnlyActsWhenActive() {
	sil_micropeak::CleanupFilter inactive;
	inactive.process({0.10f, 0.10f}, false, 5.f);
	inactive.process({4.80f, -4.80f}, false, 5.f);
	const sil_micropeak::StereoSample inactiveOut = inactive.process({0.12f, 0.12f}, false, 5.f);

	sil_micropeak::CleanupFilter active;
	active.process({0.10f, 0.10f}, true, 5.f);
	active.process({4.80f, -4.80f}, true, 5.f);
	const sil_micropeak::StereoSample activeOut = active.process({0.12f, 0.12f}, true, 5.f);

	const bool inactivePreserved = std::fabs(inactiveOut.l - 4.80f) < 1e-6f && std::fabs(inactiveOut.r + 4.80f) < 1e-6f;
	const bool activeRepaired = std::fabs(activeOut.l - 0.11f) < 1e-6f && std::fabs(activeOut.r - 0.11f) < 1e-6f;
	return {"Stateful cleanup is gated by active hold",
	        inactivePreserved && activeRepaired,
	        "inactive=(" + std::to_string(inactiveOut.l) + "," + std::to_string(inactiveOut.r) +
	            ") active=(" + std::to_string(activeOut.l) + "," + std::to_string(activeOut.r) + ")"};
}

} // namespace

int main() {
	TestResult results[] = {
		expectCleanSineNotDetected(),
		expectIsolatedSpikesDetected(),
		expectBroadTransientNotDetected(),
		expectCleanupRepairsIsolatedSpikeWhenActive(),
		expectCleanupLeavesBroadTransient(),
		expectStatefulCleanupOnlyActsWhenActive(),
	};

	int passed = 0;
	for (const TestResult& result : results) {
		if (result.passed) {
			passed++;
			std::cout << "[PASS] " << result.name << " :: " << result.detail << "\n";
		}
		else {
			std::cout << "[FAIL] " << result.name << " :: " << result.detail << "\n";
		}
	}

	std::cout << "Summary: " << passed << "/" << (sizeof(results) / sizeof(results[0])) << " passed\n";
	return passed == int(sizeof(results) / sizeof(results[0])) ? 0 : 1;
}

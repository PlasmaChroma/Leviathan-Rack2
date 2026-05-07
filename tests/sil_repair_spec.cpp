#include "../src/RepairBuffer.hpp"
#include "../src/RepairKernel.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct TestResult {
	std::string name;
	bool pass = false;
	std::string detail;
};

TestResult testRepairBufferPreservesChronologicalNeighbors() {
	sil::repair::RepairBuffer buf;
	buf.configure(4, 2);
	for (int i = 0; i < 32; ++i) {
		buf.push(float(i), float(-i));
	}

	const sil::repair::StereoWindows w = buf.readCurrentWindows();
	const bool pass =
		std::fabs(w.left.prev2 - 25.f) < 1e-4f &&
		std::fabs(w.left.prev1 - 26.f) < 1e-4f &&
		std::fabs(w.left.center - 27.f) < 1e-4f &&
		std::fabs(w.left.next1 - 28.f) < 1e-4f &&
		std::fabs(w.left.next2 - 29.f) < 1e-4f;

	return {
		"buffer chronology around delayed center",
		pass,
		"L[p2,p1,c,n1,n2]=[" +
			std::to_string(w.left.prev2) + "," +
			std::to_string(w.left.prev1) + "," +
			std::to_string(w.left.center) + "," +
			std::to_string(w.left.next1) + "," +
			std::to_string(w.left.next2) + "]"
	};
}

TestResult testIsolatedSpikeDetectedAndRepaired() {
	sil::repair::CandidateConfig cfg;
	sil::repair::Window5 w;
	w.prev2 = 0.15f;
	w.prev1 = 0.20f;
	w.center = 4.50f;
	w.next1 = 0.12f;
	w.next2 = 0.18f;

	const bool candidate = sil::repair::detectCandidate(w, cfg, 5.f);
	const sil::repair::RepairDecision d = sil::repair::repairCenterLinear(w, candidate);
	const float expected = 0.5f * (w.prev1 + w.next1);
	const bool pass = candidate && d.candidate && std::fabs(d.repaired - expected) < 1e-6f && d.depth > 0.90f;
	return {
		"isolated spike candidate and linear repair",
		pass,
		"candidate=" + std::to_string(int(candidate)) +
		" repaired=" + std::to_string(d.repaired) +
		" depth=" + std::to_string(d.depth)
	};
}

TestResult testBroadTransientRejected() {
	sil::repair::CandidateConfig cfg;
	sil::repair::Window5 w;
	w.prev2 = 2.6f;
	w.prev1 = 3.4f;
	w.center = 4.4f;
	w.next1 = 3.2f;
	w.next2 = 2.5f;

	const bool candidate = sil::repair::detectCandidate(w, cfg, 5.f);
	const sil::repair::RepairDecision d = sil::repair::repairCenterLinear(w, candidate);
	const bool pass = !candidate && !d.candidate && std::fabs(d.repaired - w.center) < 1e-6f;
	return {
		"broad transient veto",
		pass,
		"candidate=" + std::to_string(int(candidate)) +
		" repaired=" + std::to_string(d.repaired)
	};
}

} // namespace

int main() {
	std::vector<TestResult> tests;
	tests.push_back(testRepairBufferPreservesChronologicalNeighbors());
	tests.push_back(testIsolatedSpikeDetectedAndRepaired());
	tests.push_back(testBroadTransientRejected());

	int failed = 0;
	std::cout << "Sil Repair Spec\n";
	std::cout << "---------------\n";
	for (const TestResult& t : tests) {
		std::cout << (t.pass ? "[PASS] " : "[FAIL] ") << t.name << " :: " << t.detail << "\n";
		if (!t.pass) {
			failed++;
		}
	}
	std::cout << "---------------\n";
	std::cout << "Summary: " << (tests.size() - size_t(failed)) << "/" << tests.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}

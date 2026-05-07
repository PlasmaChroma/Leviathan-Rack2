#include "../src/SilRepairBuffer.hpp"
#include "../src/SilRepairKernel.hpp"

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

uint32_t countCandidatesInStream(
	const std::vector<float>& stream,
	int lookaheadSamples,
	const sil::repair::CandidateConfig& cfg,
	float fullScale
) {
	sil::repair::RepairBuffer buf;
	buf.configure(lookaheadSamples, 2);
	uint32_t hits = 0;
	for (float s : stream) {
		buf.push(s, 0.f);
		const sil::repair::StereoWindows w = buf.readCurrentWindows();
		if (sil::repair::detectCandidate(w.left, cfg, fullScale)) {
			hits++;
		}
	}
	return hits;
}

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

TestResult testBelowMinPeakRejected() {
	sil::repair::CandidateConfig cfg;
	const float fullScale = 5.f;
	sil::repair::Window5 w;
	w.prev2 = 0.1f;
	w.prev1 = 0.1f;
	w.center = cfg.minPeakFullScale * fullScale - 0.01f;
	w.next1 = 0.1f;
	w.next2 = 0.1f;
	const bool candidate = sil::repair::detectCandidate(w, cfg, fullScale);
	return {"below min-peak veto", !candidate, "candidate=" + std::to_string(int(candidate))};
}

TestResult testNeighborRatioVeto() {
	sil::repair::CandidateConfig cfg;
	sil::repair::Window5 w;
	w.prev2 = 0.20f;
	w.prev1 = 3.40f;
	w.center = 4.20f;
	w.next1 = 3.10f;
	w.next2 = 0.15f;
	const bool candidate = sil::repair::detectCandidate(w, cfg, 5.f);
	return {"neighbor ratio veto", !candidate, "candidate=" + std::to_string(int(candidate))};
}

TestResult testPerChannelIndependence() {
	sil::repair::CandidateConfig cfg;
	sil::repair::Window5 left;
	left.prev2 = 0.1f;
	left.prev1 = 0.2f;
	left.center = 4.8f;
	left.next1 = 0.2f;
	left.next2 = 0.1f;
	sil::repair::Window5 right;
	right.prev2 = 1.2f;
	right.prev1 = 2.4f;
	right.center = 3.7f;
	right.next1 = 2.2f;
	right.next2 = 1.1f;

	const bool candidateL = sil::repair::detectCandidate(left, cfg, 5.f);
	const bool candidateR = sil::repair::detectCandidate(right, cfg, 5.f);
	const sil::repair::RepairDecision repL = sil::repair::repairCenterLinear(left, candidateL);
	const sil::repair::RepairDecision repR = sil::repair::repairCenterLinear(right, candidateR);
	const bool pass = candidateL && !candidateR && repL.candidate && !repR.candidate &&
		std::fabs(repR.repaired - right.center) < 1e-6f;
	return {
		"per-channel decision independence",
		pass,
		"candL=" + std::to_string(int(candidateL)) + " candR=" + std::to_string(int(candidateR))
	};
}

TestResult testCleanHfSineHasNoHits() {
	sil::repair::CandidateConfig cfg;
	std::vector<float> stream;
	stream.reserve(4096);
	const float sr = 48000.f;
	const float hz = 10000.f;
	for (int n = 0; n < 4096; ++n) {
		stream.push_back(2.0f * std::sin(2.f * float(M_PI) * hz * (float(n) / sr)));
	}
	const uint32_t hits = countCandidatesInStream(stream, 48, cfg, 5.f);
	return {"clean HF sine no false hits", hits == 0, "hits=" + std::to_string(hits)};
}

TestResult testSingleSyntheticSpikeCountsOnce() {
	sil::repair::CandidateConfig cfg;
	std::vector<float> stream(4096, 0.f);
	stream[2048] = 4.9f;
	const uint32_t hits = countCandidatesInStream(stream, 48, cfg, 5.f);
	return {"single isolated spike emits one hit", hits == 1, "hits=" + std::to_string(hits)};
}

TestResult testModerateSyntheticSpikeCountsOnce() {
	sil::repair::CandidateConfig cfg;
	std::vector<float> stream(4096, 0.f);
	stream[2048] = cfg.minPeakFullScale * 5.f + 0.25f;
	const uint32_t hits = countCandidatesInStream(stream, 48, cfg, 5.f);
	return {"moderate isolated spike emits one hit", hits == 1, "hits=" + std::to_string(hits)};
}

} // namespace

int main() {
	std::vector<TestResult> tests;
	tests.push_back(testRepairBufferPreservesChronologicalNeighbors());
	tests.push_back(testIsolatedSpikeDetectedAndRepaired());
	tests.push_back(testBroadTransientRejected());
	tests.push_back(testBelowMinPeakRejected());
	tests.push_back(testNeighborRatioVeto());
	tests.push_back(testPerChannelIndependence());
	tests.push_back(testCleanHfSineHasNoHits());
	tests.push_back(testSingleSyntheticSpikeCountsOnce());
	tests.push_back(testModerateSyntheticSpikeCountsOnce());

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

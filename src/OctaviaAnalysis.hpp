#pragma once

#include "OctaviaObservation.hpp"

#include <array>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace octavia {

struct SpectrumBin {
	float hz = 0.f;
	float db = -140.f;
};

struct ResonanceCandidate {
	float hz = 0.f;
	float db = -140.f;
	float prominenceDb = 0.f;
	bool stable = false;
};

struct HumAnalysis {
	bool detected50 = false;
	bool detected60 = false;
	std::array<float, 4> series50Db{{-140.f, -140.f, -140.f, -140.f}};
	std::array<float, 4> series60Db{{-140.f, -140.f, -140.f, -140.f}};
};

struct ChannelAnalysis {
	ObserveChannel channel = ObserveChannel::MasterL;
	bool connected = false;
	uint64_t frames = 0;
	float rms = 0.f;
	float rmsDb = -140.f;
	float peak = 0.f;
	float peakDb = -140.f;
	float crestDb = 0.f;
	float dcOffset = 0.f;
	uint64_t clippedSamples = 0;
	float noiseFloorDb = -140.f;
	std::array<float, 7> bandsDb{{-140.f, -140.f, -140.f, -140.f, -140.f, -140.f, -140.f}};
	uint64_t temporalSeparationFrames = 0;
	HumAnalysis hum;
	bool feedbackSuspect = false;
	float feedbackHz = 0.f;
	float feedbackRiseDb = 0.f;
	std::vector<ResonanceCandidate> resonances;
	std::vector<std::string> issues;
	std::vector<SpectrumBin> spectrum;
};

struct StereoAnalysis {
	ObserveChannel left = ObserveChannel::MasterL;
	ObserveChannel right = ObserveChannel::MasterR;
	ChannelAnalysis leftAnalysis;
	ChannelAnalysis rightAnalysis;
	float balanceDb = 0.f;
	float correlation = 0.f;
	float midRms = 0.f;
	float sideRms = 0.f;
	float sideToMidDb = -140.f;
};

struct AnalysisGroup {
	bool stereo = false;
	ObserveChannel first = ObserveChannel::MasterL;
	ObserveChannel second = ObserveChannel::MasterR;
};

struct GroupAnalysis {
	AnalysisGroup group;
	ChannelAnalysis mono;
	StereoAnalysis stereo;
};

struct ComparisonAnalysis {
	GroupAnalysis reference;
	GroupAnalysis target;
	float rmsDeltaDb = 0.f;
	float peakDeltaDb = 0.f;
	float crestDeltaDb = 0.f;
	float dcOffsetDelta = 0.f;
	std::array<float, 7> spectralDeltaDb{};
	std::array<float, 7> normalizedSpectralDeltaDb{};
	float balanceDeltaDb = 0.f;
	float correlationDelta = 0.f;
	float widthDeltaDb = 0.f;
};

// Detailed work is performed by HTTP/server threads, never process(). The
// try-lock makes the heavyweight path explicitly bounded to one active job.
class AnalysisEngine {
public:
	bool tryAnalyze(const FrozenObservation& snapshot, const AnalysisGroup& group,
		bool detailed, bool includeSpectrum, GroupAnalysis* result, std::string* error);
	bool tryCompare(const FrozenObservation& snapshot, const AnalysisGroup& reference,
		const AnalysisGroup& target, bool detailed, bool includeSpectrum,
		ComparisonAnalysis* result, std::string* error);

private:
	std::mutex executionMutex_;
};

const std::array<const char*, 7>& analysisBandNames();

} // namespace octavia

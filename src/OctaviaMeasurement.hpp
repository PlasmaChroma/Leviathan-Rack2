#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace octavia {

static const uint32_t MEASUREMENT_BLOCKS = 36000;

enum class MeasurementState : uint8_t { Idle, Pending, Active, Complete, Cancelled };

const char* measurementStateName(MeasurementState state);

struct MeasurementResult {
	uint64_t id = 0;
	MeasurementState state = MeasurementState::Idle;
	uint64_t startFrame = 0;
	uint64_t endFrame = 0;
	uint64_t targetFrames = 0;
	uint64_t measuredFrames = 0;
	float sampleRate = 0.f;
	std::array<double, 2> kSum{{0.0, 0.0}};
	std::array<double, 2> rawSum{{0.0, 0.0}};
	std::array<double, 2> peak{{0.0, 0.0}};
	std::array<uint64_t, 2> clipped{{0, 0}};
	double sumLR = 0.0;
	std::vector<float> blockPowers;
	bool blockHistoryTruncated = false;
};

class MasterMeasurement {
public:
	MasterMeasurement();

	// durationFrames=0 arms an open-ended legacy reset/read session.
	bool arm(uint64_t durationFrames, bool replaceActive, uint64_t* id, std::string* error);
	void process(uint64_t frame, float sampleRate,
		const std::array<double, 2>& normalized,
		const std::array<double, 2>& kPower);
	MeasurementResult read() const;
	MeasurementState state() const;

private:
	void beginRequest(uint64_t frame, float sampleRate, uint64_t generation);
	void publish(bool force);
	void finish(uint64_t frame, MeasurementState state);

	mutable std::mutex requestMutex_;
	std::atomic<uint64_t> requestedGeneration_{0};
	std::atomic<uint64_t> requestedDurationFrames_{0};
	std::atomic<uint64_t> nextId_{1};
	std::atomic<uint64_t> requestedId_{0};
	std::atomic<uint8_t> state_{static_cast<uint8_t>(MeasurementState::Idle)};

	// Audio-thread-owned working state.
	uint64_t activeGeneration_ = 0;
	uint64_t activeId_ = 0;
	uint64_t startFrame_ = 0;
	uint64_t targetFrames_ = 0;
	uint64_t measuredFrames_ = 0;
	float activeSampleRate_ = 0.f;
	std::array<double, 2> kSum_{{0.0, 0.0}};
	std::array<double, 2> rawSum_{{0.0, 0.0}};
	std::array<double, 2> peak_{{0.0, 0.0}};
	std::array<uint64_t, 2> clipped_{{0, 0}};
	double sumLR_ = 0.0;
	double blockKSum_ = 0.0;
	uint32_t blockFrames_ = 0;
	uint32_t blockTarget_ = 4800;
	uint64_t blockTotal_ = 0;
	uint32_t publishCountdown_ = 2048;

	// Atomically published result; audio thread never takes requestMutex_.
	std::atomic<uint64_t> publishedSequence_{0};
	std::atomic<uint64_t> publishedId_{0};
	std::atomic<uint64_t> publishedStartFrame_{0};
	std::atomic<uint64_t> publishedEndFrame_{0};
	std::atomic<uint64_t> publishedTargetFrames_{0};
	std::atomic<uint64_t> publishedMeasuredFrames_{0};
	std::atomic<float> publishedSampleRate_{0.f};
	std::array<std::atomic<double>, 2> publishedKSum_;
	std::array<std::atomic<double>, 2> publishedRawSum_;
	std::array<std::atomic<double>, 2> publishedPeak_;
	std::array<std::atomic<uint64_t>, 2> publishedClipped_;
	std::atomic<double> publishedSumLR_{0.0};
	std::array<std::atomic<float>, MEASUREMENT_BLOCKS> publishedBlocks_;
	std::atomic<uint64_t> publishedBlockTotal_{0};
};

} // namespace octavia

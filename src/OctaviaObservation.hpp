#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace octavia {

enum class ObserveChannel : uint8_t {
	MasterL = 0,
	MasterR,
	A,
	B,
	C,
	D,
	Count
};

static const uint32_t OBSERVATION_HISTORY_FRAMES = 262144;
static const size_t OBSERVATION_CHANNELS = static_cast<size_t>(ObserveChannel::Count);
static const size_t SNAPSHOT_POOL_LIMIT = 12;

const char* observeChannelName(ObserveChannel channel);
bool parseObserveChannel(const std::string& text, ObserveChannel* channel);
uint8_t observeChannelBit(ObserveChannel channel);

struct ObservationFrame {
	std::array<float, OBSERVATION_CHANNELS> volts;
	uint8_t connectedMask = 0;
	float sampleRate = 0.f;
};

struct FrozenObservation {
	uint64_t id = 0;
	uint64_t triggerFrame = 0;
	uint64_t startFrame = 0;
	uint64_t endFrame = 0;
	uint32_t preFrames = 0;
	uint32_t postFrames = 0;
	uint8_t requestedMask = 0;
	uint8_t allConnectedMask = 0;
	uint8_t anyConnectedMask = 0;
	float sampleRate = 0.f;
	std::string label;
	std::array<std::vector<float>, OBSERVATION_CHANNELS> samples;
	std::vector<uint8_t> connectionMasks;
};

class ObservationHistory {
public:
	ObservationHistory();

	void publish(uint64_t frame, float sampleRate,
		const std::array<float, OBSERVATION_CHANNELS>& volts,
		uint8_t connectedMask,
		const std::array<uint8_t, OBSERVATION_CHANNELS>& channelCounts);
	bool hasPublishedFrame() const;
	uint64_t publishedFrame() const;
	uint64_t oldestAvailableFrame() const;
	float currentSampleRate() const;
	uint8_t currentConnectedMask() const;
	uint8_t currentChannelCount(ObserveChannel channel) const;
	uint64_t snapshotGeneration(ObserveChannel channel) const;
	void markSnapshotRequested(uint8_t requestedMask);

	bool readFrame(uint64_t frame, ObservationFrame* result) const;
	bool freeze(uint64_t startFrame, uint64_t endFrame, uint8_t requestedMask,
		FrozenObservation* result, std::string* error) const;
	bool copyLatest(ObserveChannel channel, size_t count, std::vector<float>* result,
		float* sampleRate = nullptr) const;

private:
	struct Slot {
		std::array<std::atomic<float>, OBSERVATION_CHANNELS> volts;
		std::atomic<uint8_t> connectedMask;
		std::atomic<float> sampleRate;
		std::atomic<uint64_t> sequence;

		Slot();
	};

	static const uint64_t kNoFrame = UINT64_MAX;
	std::unique_ptr<Slot[]> slots_;
	std::atomic<uint64_t> firstPublishedFrame_;
	std::atomic<uint64_t> publishedFrame_;
	std::atomic<float> currentSampleRate_;
	std::atomic<uint8_t> currentConnectedMask_;
	std::array<std::atomic<uint8_t>, OBSERVATION_CHANNELS> channelCounts_;
	std::array<std::atomic<uint64_t>, OBSERVATION_CHANNELS> snapshotGenerations_;
};

enum class SnapshotState : uint8_t { Pending, Complete, Failed };

const char* snapshotStateName(SnapshotState state);

struct ObservationSnapshot {
	FrozenObservation observation;
	SnapshotState state = SnapshotState::Pending;
	std::string error;
};

class ObservationSnapshotPool {
public:
	explicit ObservationSnapshotPool(ObservationHistory* history);

	bool create(uint32_t preFrames, uint32_t postFrames, uint8_t requestedMask,
		const std::string& label, ObservationSnapshot* result, std::string* error);
	bool get(uint64_t id, ObservationSnapshot* result);
	size_t size() const;

private:
	void refresh(ObservationSnapshot* snapshot);
	bool makeRoom();

	ObservationHistory* history_ = nullptr;
	mutable std::mutex mutex_;
	std::deque<ObservationSnapshot> snapshots_;
	uint64_t nextId_ = 1;
};

} // namespace octavia

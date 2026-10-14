#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <string>

namespace octavia {

static const size_t OBSERVATION_TRIGGER_CAPACITY = 64;
static const size_t OBSERVATION_TRIGGER_LABEL_BYTES = 64;

struct ObservationTrigger {
	uint64_t requestId = 0;
	int64_t octaviaModuleId = -1;
	uint64_t triggerFrame = 0;
	uint32_t preFrames = 0;
	uint32_t postFrames = 0;
	uint8_t monitorMask = 0;
	std::array<char, OBSERVATION_TRIGGER_LABEL_BYTES> label{};

	void setLabel(const std::string& text);
	std::string labelString() const;
};

// Lock-free broadcast publication. Each Octavia owns an independent cursor,
// so one module cannot consume another module's targeted trigger.
class ObservationBus {
public:
	ObservationBus();
	uint64_t publish(const ObservationTrigger& trigger);
	uint64_t latestSequence() const;
	bool poll(uint64_t* cursor, ObservationTrigger* trigger, uint64_t* dropped = nullptr) const;

private:
	struct Slot {
		std::atomic<uint64_t> sequence;
		std::atomic<int64_t> octaviaModuleId;
		std::atomic<uint64_t> triggerFrame;
		std::atomic<uint32_t> preFrames;
		std::atomic<uint32_t> postFrames;
		std::atomic<uint8_t> monitorMask;
		std::array<std::atomic<char>, OBSERVATION_TRIGGER_LABEL_BYTES> label;
		Slot();
	};

	std::array<Slot, OBSERVATION_TRIGGER_CAPACITY> slots_;
	std::atomic<uint64_t> nextSequence_;
};

ObservationBus& observationBus();

} // namespace octavia

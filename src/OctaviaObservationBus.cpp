#include "OctaviaObservationBus.hpp"

#include <algorithm>
#include <limits>

namespace octavia {
namespace {
const uint64_t kEmptySequence = std::numeric_limits<uint64_t>::max();
uint64_t completeTag(uint64_t sequence) { return sequence << 1; }
}

void ObservationTrigger::setLabel(const std::string& text) {
	label.fill('\0');
	const size_t count = std::min(text.size(), label.size() - 1);
	std::copy(text.begin(), text.begin() + count, label.begin());
}

std::string ObservationTrigger::labelString() const {
	size_t count = 0;
	while (count < label.size() && label[count]) count++;
	return std::string(label.data(), count);
}

ObservationBus::Slot::Slot()
	: sequence(kEmptySequence), octaviaModuleId(-1), triggerFrame(0),
	  preFrames(0), postFrames(0), monitorMask(0) {
	for (auto& byte : label) byte.store('\0', std::memory_order_relaxed);
}

ObservationBus::ObservationBus() : nextSequence_(1) {}

uint64_t ObservationBus::publish(const ObservationTrigger& trigger) {
	const uint64_t sequence = nextSequence_.fetch_add(1, std::memory_order_relaxed);
	Slot& slot = slots_[sequence % OBSERVATION_TRIGGER_CAPACITY];
	const uint64_t complete = completeTag(sequence);
	uint64_t expected = sequence <= OBSERVATION_TRIGGER_CAPACITY
		? kEmptySequence : completeTag(sequence - OBSERVATION_TRIGGER_CAPACITY);
	if (!slot.sequence.compare_exchange_strong(expected, complete | 1u,
			std::memory_order_acq_rel, std::memory_order_relaxed)) return 0;
	slot.octaviaModuleId.store(trigger.octaviaModuleId, std::memory_order_relaxed);
	slot.triggerFrame.store(trigger.triggerFrame, std::memory_order_relaxed);
	slot.preFrames.store(trigger.preFrames, std::memory_order_relaxed);
	slot.postFrames.store(trigger.postFrames, std::memory_order_relaxed);
	slot.monitorMask.store(trigger.monitorMask, std::memory_order_relaxed);
	for (size_t i = 0; i < trigger.label.size(); ++i)
		slot.label[i].store(trigger.label[i], std::memory_order_relaxed);
	slot.sequence.store(complete, std::memory_order_release);
	return sequence;
}

uint64_t ObservationBus::latestSequence() const {
	return nextSequence_.load(std::memory_order_acquire) - 1;
}

bool ObservationBus::poll(uint64_t* cursor, ObservationTrigger* trigger, uint64_t* dropped) const {
	if (!cursor || !trigger) return false;
	const uint64_t latest = latestSequence();
	uint64_t desired = *cursor + 1;
	if (desired > latest) return false;
	const uint64_t oldest = latest >= OBSERVATION_TRIGGER_CAPACITY
		? latest - OBSERVATION_TRIGGER_CAPACITY + 1 : 1;
	if (desired < oldest) {
		if (dropped) *dropped += oldest - desired;
		desired = oldest;
	}
	const Slot& slot = slots_[desired % OBSERVATION_TRIGGER_CAPACITY];
	const uint64_t expected = completeTag(desired);
	const uint64_t observed = slot.sequence.load(std::memory_order_acquire);
	if (observed == (expected | 1u)) return false;
	if (observed != expected) {
		// A producer may fail fast rather than wait on a writer that still owns
		// this bounded slot. Consumers explicitly account for that missing item.
		*cursor = desired;
		if (dropped) (*dropped)++;
		return false;
	}
	trigger->requestId = desired;
	trigger->octaviaModuleId = slot.octaviaModuleId.load(std::memory_order_relaxed);
	trigger->triggerFrame = slot.triggerFrame.load(std::memory_order_relaxed);
	trigger->preFrames = slot.preFrames.load(std::memory_order_relaxed);
	trigger->postFrames = slot.postFrames.load(std::memory_order_relaxed);
	trigger->monitorMask = slot.monitorMask.load(std::memory_order_relaxed);
	for (size_t i = 0; i < trigger->label.size(); ++i)
		trigger->label[i] = slot.label[i].load(std::memory_order_relaxed);
	if (slot.sequence.load(std::memory_order_acquire) != expected) return false;
	*cursor = desired;
	return true;
}

ObservationBus& observationBus() {
	static ObservationBus bus;
	return bus;
}

} // namespace octavia

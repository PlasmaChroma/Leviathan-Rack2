#include "OctaviaObservation.hpp"

#include <algorithm>
#include <cctype>
#include <limits>

namespace octavia {

namespace {

std::string normalizedChannelName(const std::string& text) {
	std::string normalized;
	normalized.reserve(text.size());
	for (size_t i = 0; i < text.size(); ++i) {
		const unsigned char c = static_cast<unsigned char>(text[i]);
		if (c == '_' || c == '-' || std::isspace(c)) continue;
		normalized.push_back(static_cast<char>(std::tolower(c)));
	}
	return normalized;
}

uint64_t publishedSequence(uint64_t frame) {
	return frame << 1;
}

} // namespace

const char* observeChannelName(ObserveChannel channel) {
	switch (channel) {
		case ObserveChannel::MasterL: return "masterL";
		case ObserveChannel::MasterR: return "masterR";
		case ObserveChannel::A: return "A";
		case ObserveChannel::B: return "B";
		case ObserveChannel::C: return "C";
		case ObserveChannel::D: return "D";
		default: return "unknown";
	}
}

bool parseObserveChannel(const std::string& text, ObserveChannel* channel) {
	if (!channel) return false;
	const std::string normalized = normalizedChannelName(text);
	if (normalized == "masterl" || normalized == "left" || normalized == "0")
		*channel = ObserveChannel::MasterL;
	else if (normalized == "masterr" || normalized == "right" || normalized == "1")
		*channel = ObserveChannel::MasterR;
	else if (normalized == "a") *channel = ObserveChannel::A;
	else if (normalized == "b") *channel = ObserveChannel::B;
	else if (normalized == "c") *channel = ObserveChannel::C;
	else if (normalized == "d") *channel = ObserveChannel::D;
	else return false;
	return true;
}

uint8_t observeChannelBit(ObserveChannel channel) {
	const size_t index = static_cast<size_t>(channel);
	return index < OBSERVATION_CHANNELS ? static_cast<uint8_t>(1u << index) : 0;
}

ObservationHistory::Slot::Slot()
	: connectedMask(0), sampleRate(0.f), sequence(kNoFrame) {
	for (size_t channel = 0; channel < OBSERVATION_CHANNELS; ++channel)
		volts[channel].store(0.f, std::memory_order_relaxed);
}

ObservationHistory::ObservationHistory()
	: slots_(new Slot[OBSERVATION_HISTORY_FRAMES]),
	  firstPublishedFrame_(kNoFrame), publishedFrame_(kNoFrame),
	  currentSampleRate_(0.f), currentConnectedMask_(0) {
	for (size_t channel = 0; channel < OBSERVATION_CHANNELS; ++channel) {
		channelCounts_[channel].store(0, std::memory_order_relaxed);
		snapshotGenerations_[channel].store(0, std::memory_order_relaxed);
	}
}

void ObservationHistory::publish(uint64_t frame, float sampleRate,
		const std::array<float, OBSERVATION_CHANNELS>& volts,
		uint8_t connectedMask,
		const std::array<uint8_t, OBSERVATION_CHANNELS>& channelCounts) {
	Slot& slot = slots_[frame & (OBSERVATION_HISTORY_FRAMES - 1)];
	const uint64_t complete = publishedSequence(frame);
	slot.sequence.store(complete | 1u, std::memory_order_release);
	for (size_t channel = 0; channel < OBSERVATION_CHANNELS; ++channel)
		slot.volts[channel].store(volts[channel], std::memory_order_relaxed);
	slot.connectedMask.store(connectedMask, std::memory_order_relaxed);
	slot.sampleRate.store(sampleRate, std::memory_order_relaxed);
	slot.sequence.store(complete, std::memory_order_release);

	uint64_t expected = kNoFrame;
	firstPublishedFrame_.compare_exchange_strong(expected, frame,
		std::memory_order_release, std::memory_order_relaxed);
	currentSampleRate_.store(sampleRate, std::memory_order_relaxed);
	currentConnectedMask_.store(connectedMask, std::memory_order_relaxed);
	for (size_t channel = 0; channel < OBSERVATION_CHANNELS; ++channel)
		channelCounts_[channel].store(channelCounts[channel], std::memory_order_relaxed);
	publishedFrame_.store(frame, std::memory_order_release);
}

bool ObservationHistory::hasPublishedFrame() const {
	return publishedFrame_.load(std::memory_order_acquire) != kNoFrame;
}

uint64_t ObservationHistory::publishedFrame() const {
	return publishedFrame_.load(std::memory_order_acquire);
}

uint64_t ObservationHistory::oldestAvailableFrame() const {
	const uint64_t first = firstPublishedFrame_.load(std::memory_order_acquire);
	const uint64_t latest = publishedFrame_.load(std::memory_order_acquire);
	if (first == kNoFrame || latest == kNoFrame) return kNoFrame;
	const uint64_t wrappedOldest = latest >= OBSERVATION_HISTORY_FRAMES - 1
		? latest - (OBSERVATION_HISTORY_FRAMES - 1) : 0;
	return std::max(first, wrappedOldest);
}

float ObservationHistory::currentSampleRate() const {
	return currentSampleRate_.load(std::memory_order_relaxed);
}

uint8_t ObservationHistory::currentConnectedMask() const {
	return currentConnectedMask_.load(std::memory_order_relaxed);
}

uint8_t ObservationHistory::currentChannelCount(ObserveChannel channel) const {
	const size_t index = static_cast<size_t>(channel);
	return index < OBSERVATION_CHANNELS
		? channelCounts_[index].load(std::memory_order_relaxed) : 0;
}

uint64_t ObservationHistory::snapshotGeneration(ObserveChannel channel) const {
	const size_t index = static_cast<size_t>(channel);
	return index < OBSERVATION_CHANNELS
		? snapshotGenerations_[index].load(std::memory_order_relaxed) : 0;
}

void ObservationHistory::markSnapshotRequested(uint8_t requestedMask) {
	for (size_t channel = 0; channel < OBSERVATION_CHANNELS; ++channel) {
		if (requestedMask & (1u << channel))
			snapshotGenerations_[channel].fetch_add(1, std::memory_order_relaxed);
	}
}

bool ObservationHistory::readFrame(uint64_t frame, ObservationFrame* result) const {
	if (!result) return false;
	const Slot& slot = slots_[frame & (OBSERVATION_HISTORY_FRAMES - 1)];
	const uint64_t expected = publishedSequence(frame);
	const uint64_t before = slot.sequence.load(std::memory_order_acquire);
	if (before != expected) return false;
	for (size_t channel = 0; channel < OBSERVATION_CHANNELS; ++channel)
		result->volts[channel] = slot.volts[channel].load(std::memory_order_relaxed);
	result->connectedMask = slot.connectedMask.load(std::memory_order_relaxed);
	result->sampleRate = slot.sampleRate.load(std::memory_order_relaxed);
	return slot.sequence.load(std::memory_order_acquire) == expected;
}

bool ObservationHistory::freeze(uint64_t startFrame, uint64_t endFrame,
		uint8_t requestedMask, FrozenObservation* result, std::string* error) const {
	if (!result || startFrame > endFrame || requestedMask == 0) {
		if (error) *error = "invalid_snapshot_request";
		return false;
	}
	const uint64_t latest = publishedFrame();
	if (latest == kNoFrame || endFrame > latest) {
		if (error) *error = "insufficient_history";
		return false;
	}
	const uint64_t oldest = oldestAvailableFrame();
	if (oldest == kNoFrame || startFrame < oldest) {
		if (error) *error = "snapshot_expired";
		return false;
	}
	const uint64_t frameCount64 = endFrame - startFrame + 1;
	if (frameCount64 > OBSERVATION_HISTORY_FRAMES) {
		if (error) *error = "snapshot_expired";
		return false;
	}
	const size_t frameCount = static_cast<size_t>(frameCount64);
	for (size_t channel = 0; channel < OBSERVATION_CHANNELS; ++channel) {
		result->samples[channel].clear();
		if (requestedMask & (1u << channel)) result->samples[channel].reserve(frameCount);
	}
	result->connectionMasks.clear();
	result->connectionMasks.reserve(frameCount);
	result->allConnectedMask = requestedMask;
	result->anyConnectedMask = 0;
	for (uint64_t frame = startFrame; frame <= endFrame; ++frame) {
		ObservationFrame observed;
		if (!readFrame(frame, &observed)) {
			if (error) *error = frame < oldestAvailableFrame()
				? "snapshot_expired" : "insufficient_history";
			return false;
		}
		if (frame == startFrame) result->sampleRate = observed.sampleRate;
		result->connectionMasks.push_back(observed.connectedMask);
		result->allConnectedMask &= observed.connectedMask;
		result->anyConnectedMask |= observed.connectedMask;
		for (size_t channel = 0; channel < OBSERVATION_CHANNELS; ++channel) {
			if (requestedMask & (1u << channel))
				result->samples[channel].push_back(observed.volts[channel]);
		}
		if (frame == std::numeric_limits<uint64_t>::max()) break;
	}
	return true;
}

bool ObservationHistory::copyLatest(ObserveChannel channel, size_t count,
		std::vector<float>* result, float* sampleRate) const {
	if (!result || count == 0 || count > OBSERVATION_HISTORY_FRAMES) return false;
	result->assign(count, 0.f);
	const uint64_t latest = publishedFrame();
	const uint64_t oldest = oldestAvailableFrame();
	if (latest == kNoFrame || oldest == kNoFrame) return false;
	const uint64_t desiredStart = latest >= count - 1 ? latest - (count - 1) : 0;
	const uint64_t start = std::max(desiredStart, oldest);
	const size_t offset = static_cast<size_t>(start - desiredStart);
	ObservationFrame frame;
	for (uint64_t current = start; current <= latest; ++current) {
		if (!readFrame(current, &frame)) return false;
		(*result)[offset + static_cast<size_t>(current - start)] =
			frame.volts[static_cast<size_t>(channel)];
		if (current == std::numeric_limits<uint64_t>::max()) break;
	}
	if (sampleRate) *sampleRate = frame.sampleRate;
	return true;
}

const char* snapshotStateName(SnapshotState state) {
	switch (state) {
		case SnapshotState::Pending: return "pending_postroll";
		case SnapshotState::Complete: return "complete";
		case SnapshotState::Failed: return "failed";
		default: return "failed";
	}
}

ObservationSnapshotPool::ObservationSnapshotPool(ObservationHistory* history)
	: history_(history) {}

bool ObservationSnapshotPool::makeRoom() {
	if (snapshots_.size() < SNAPSHOT_POOL_LIMIT) return true;
	for (std::deque<ObservationSnapshot>::iterator it = snapshots_.begin();
			it != snapshots_.end(); ++it) {
		if (it->state != SnapshotState::Pending) {
			snapshots_.erase(it);
			return true;
		}
	}
	return false;
}

void ObservationSnapshotPool::refresh(ObservationSnapshot* snapshot) {
	if (!snapshot || snapshot->state != SnapshotState::Pending || !history_) return;
	if (!history_->hasPublishedFrame()
			|| history_->publishedFrame() < snapshot->observation.endFrame) return;
	std::string error;
	if (history_->freeze(snapshot->observation.startFrame, snapshot->observation.endFrame,
			snapshot->observation.requestedMask, &snapshot->observation, &error)) {
		snapshot->state = SnapshotState::Complete;
	} else {
		snapshot->state = SnapshotState::Failed;
		snapshot->error = error;
	}
}

bool ObservationSnapshotPool::create(uint32_t preFrames, uint32_t postFrames,
		uint8_t requestedMask, const std::string& label,
		ObservationSnapshot* result, std::string* error) {
	if (!history_ || !history_->hasPublishedFrame()) {
		if (error) *error = "insufficient_history";
		return false;
	}
	const uint64_t totalFrames = static_cast<uint64_t>(preFrames) + postFrames + 1;
	if (requestedMask == 0 || totalFrames > OBSERVATION_HISTORY_FRAMES) {
		if (error) *error = "invalid_snapshot_request";
		return false;
	}
	std::lock_guard<std::mutex> lock(mutex_);
	if (!makeRoom()) {
		if (error) *error = "snapshot_pool_busy";
		return false;
	}
	ObservationSnapshot snapshot;
	snapshot.observation.id = nextId_++;
	snapshot.observation.triggerFrame = history_->publishedFrame();
	if (snapshot.observation.triggerFrame < preFrames) {
		if (error) *error = "insufficient_history";
		return false;
	}
	snapshot.observation.startFrame = snapshot.observation.triggerFrame - preFrames;
	if (snapshot.observation.triggerFrame > std::numeric_limits<uint64_t>::max() - postFrames) {
		if (error) *error = "invalid_snapshot_request";
		return false;
	}
	snapshot.observation.endFrame = snapshot.observation.triggerFrame + postFrames;
	snapshot.observation.preFrames = preFrames;
	snapshot.observation.postFrames = postFrames;
	snapshot.observation.requestedMask = requestedMask;
	snapshot.observation.sampleRate = history_->currentSampleRate();
	snapshot.observation.label = label;
	history_->markSnapshotRequested(requestedMask);
	refresh(&snapshot);
	snapshots_.push_back(snapshot);
	if (result) *result = snapshot;
	return true;
}

bool ObservationSnapshotPool::get(uint64_t id, ObservationSnapshot* result) {
	std::lock_guard<std::mutex> lock(mutex_);
	for (size_t i = 0; i < snapshots_.size(); ++i) {
		if (snapshots_[i].observation.id != id) continue;
		refresh(&snapshots_[i]);
		if (result) *result = snapshots_[i];
		return true;
	}
	return false;
}

size_t ObservationSnapshotPool::size() const {
	std::lock_guard<std::mutex> lock(mutex_);
	return snapshots_.size();
}

} // namespace octavia

#include "OctaviaMeasurement.hpp"

#include <algorithm>
#include <cmath>

namespace octavia {

const char* measurementStateName(MeasurementState state) {
	switch (state) {
		case MeasurementState::Idle: return "idle";
		case MeasurementState::Pending: return "pending";
		case MeasurementState::Active: return "active";
		case MeasurementState::Complete: return "complete";
		case MeasurementState::Cancelled: return "cancelled";
		default: return "cancelled";
	}
}

MasterMeasurement::MasterMeasurement() {
	for (size_t channel = 0; channel < 2; ++channel) {
		publishedKSum_[channel].store(0.0, std::memory_order_relaxed);
		publishedRawSum_[channel].store(0.0, std::memory_order_relaxed);
		publishedPeak_[channel].store(0.0, std::memory_order_relaxed);
		publishedClipped_[channel].store(0, std::memory_order_relaxed);
	}
	for (size_t block = 0; block < MEASUREMENT_BLOCKS; ++block)
		publishedBlocks_[block].store(0.f, std::memory_order_relaxed);
}

MeasurementState MasterMeasurement::state() const {
	return static_cast<MeasurementState>(state_.load(std::memory_order_acquire));
}

bool MasterMeasurement::arm(uint64_t durationFrames, bool replaceActive,
		uint64_t* id, std::string* error) {
	std::lock_guard<std::mutex> lock(requestMutex_);
	const MeasurementState current = state();
	if (!replaceActive && (current == MeasurementState::Pending
			|| current == MeasurementState::Active)) {
		if (error) *error = "measurement_busy";
		return false;
	}
	const uint64_t newId = nextId_.fetch_add(1, std::memory_order_relaxed);
	requestedDurationFrames_.store(durationFrames, std::memory_order_relaxed);
	requestedId_.store(newId, std::memory_order_relaxed);
	state_.store(static_cast<uint8_t>(MeasurementState::Pending), std::memory_order_relaxed);
	requestedGeneration_.fetch_add(1, std::memory_order_release);
	if (id) *id = newId;
	return true;
}

void MasterMeasurement::beginRequest(uint64_t frame, float sampleRate, uint64_t generation) {
	activeGeneration_ = generation;
	activeId_ = requestedId_.load(std::memory_order_relaxed);
	targetFrames_ = requestedDurationFrames_.load(std::memory_order_relaxed);
	startFrame_ = frame;
	measuredFrames_ = 0;
	activeSampleRate_ = sampleRate;
	kSum_ = {{0.0, 0.0}};
	rawSum_ = {{0.0, 0.0}};
	peak_ = {{0.0, 0.0}};
	clipped_ = {{0, 0}};
	sumLR_ = 0.0;
	blockKSum_ = 0.0;
	blockFrames_ = 0;
	blockTarget_ = std::max<uint32_t>(1, static_cast<uint32_t>(std::lround(sampleRate * 0.1f)));
	blockTotal_ = 0;
	publishCountdown_ = 2048;
	publishedBlockTotal_.store(0, std::memory_order_relaxed);
	state_.store(static_cast<uint8_t>(MeasurementState::Active), std::memory_order_release);
	publish(true);
}

void MasterMeasurement::publish(bool force) {
	if (!force && --publishCountdown_ > 0) return;
	publishCountdown_ = 2048;
	const uint64_t sequence = publishedSequence_.load(std::memory_order_relaxed);
	publishedSequence_.store(sequence + 1, std::memory_order_release);
	publishedId_.store(activeId_, std::memory_order_relaxed);
	publishedStartFrame_.store(startFrame_, std::memory_order_relaxed);
	publishedEndFrame_.store(measuredFrames_ ? startFrame_ + measuredFrames_ - 1 : startFrame_,
		std::memory_order_relaxed);
	publishedTargetFrames_.store(targetFrames_, std::memory_order_relaxed);
	publishedMeasuredFrames_.store(measuredFrames_, std::memory_order_relaxed);
	publishedSampleRate_.store(activeSampleRate_, std::memory_order_relaxed);
	for (size_t channel = 0; channel < 2; ++channel) {
		publishedKSum_[channel].store(kSum_[channel], std::memory_order_relaxed);
		publishedRawSum_[channel].store(rawSum_[channel], std::memory_order_relaxed);
		publishedPeak_[channel].store(peak_[channel], std::memory_order_relaxed);
		publishedClipped_[channel].store(clipped_[channel], std::memory_order_relaxed);
	}
	publishedSumLR_.store(sumLR_, std::memory_order_relaxed);
	publishedBlockTotal_.store(blockTotal_, std::memory_order_relaxed);
	publishedSequence_.store(sequence + 2, std::memory_order_release);
}

void MasterMeasurement::finish(uint64_t frame, MeasurementState finalState) {
	publish(true);
	publishedEndFrame_.store(frame, std::memory_order_relaxed);
	state_.store(static_cast<uint8_t>(finalState), std::memory_order_release);
}

void MasterMeasurement::process(uint64_t frame, float sampleRate,
		const std::array<double, 2>& normalized,
		const std::array<double, 2>& kPower) {
	const uint64_t requested = requestedGeneration_.load(std::memory_order_acquire);
	if (requested != activeGeneration_) beginRequest(frame, sampleRate, requested);
	if (state() != MeasurementState::Active) return;
	if (sampleRate != activeSampleRate_) {
		finish(frame, MeasurementState::Cancelled);
		return;
	}
	for (size_t channel = 0; channel < 2; ++channel) {
		const double value = normalized[channel];
		const double magnitude = std::fabs(value);
		kSum_[channel] += kPower[channel];
		rawSum_[channel] += value * value;
		peak_[channel] = std::max(peak_[channel], magnitude);
		if (magnitude >= 1.0) ++clipped_[channel];
	}
	sumLR_ += normalized[0] * normalized[1];
	blockKSum_ += kPower[0] + kPower[1];
	++measuredFrames_;
	if (++blockFrames_ >= blockTarget_) {
		publishedBlocks_[blockTotal_ % MEASUREMENT_BLOCKS].store(
			static_cast<float>(blockKSum_ / blockFrames_), std::memory_order_relaxed);
		++blockTotal_;
		blockKSum_ = 0.0;
		blockFrames_ = 0;
	}
	publish(false);
	if (targetFrames_ > 0 && measuredFrames_ >= targetFrames_)
		finish(frame, MeasurementState::Complete);
}

MeasurementResult MasterMeasurement::read() const {
	MeasurementResult result;
	const MeasurementState currentState = state();
	if (currentState == MeasurementState::Pending) {
		result.id = requestedId_.load(std::memory_order_relaxed);
		result.targetFrames = requestedDurationFrames_.load(std::memory_order_relaxed);
		result.state = currentState;
		return result;
	}
	for (;;) {
		const uint64_t before = publishedSequence_.load(std::memory_order_acquire);
		if (before & 1u) continue;
		result.id = publishedId_.load(std::memory_order_relaxed);
		result.startFrame = publishedStartFrame_.load(std::memory_order_relaxed);
		result.endFrame = publishedEndFrame_.load(std::memory_order_relaxed);
		result.targetFrames = publishedTargetFrames_.load(std::memory_order_relaxed);
		result.measuredFrames = publishedMeasuredFrames_.load(std::memory_order_relaxed);
		result.sampleRate = publishedSampleRate_.load(std::memory_order_relaxed);
		for (size_t channel = 0; channel < 2; ++channel) {
			result.kSum[channel] = publishedKSum_[channel].load(std::memory_order_relaxed);
			result.rawSum[channel] = publishedRawSum_[channel].load(std::memory_order_relaxed);
			result.peak[channel] = publishedPeak_[channel].load(std::memory_order_relaxed);
			result.clipped[channel] = publishedClipped_[channel].load(std::memory_order_relaxed);
		}
		result.sumLR = publishedSumLR_.load(std::memory_order_relaxed);
		const uint64_t totalBlocks = publishedBlockTotal_.load(std::memory_order_relaxed);
		const uint64_t available = std::min<uint64_t>(totalBlocks, MEASUREMENT_BLOCKS);
		result.blockPowers.clear();
		result.blockPowers.reserve(static_cast<size_t>(available));
		for (uint64_t block = totalBlocks - available; block < totalBlocks; ++block)
			result.blockPowers.push_back(
				publishedBlocks_[block % MEASUREMENT_BLOCKS].load(std::memory_order_relaxed));
		result.blockHistoryTruncated = totalBlocks > MEASUREMENT_BLOCKS;
		const uint64_t after = publishedSequence_.load(std::memory_order_acquire);
		if (before == after) break;
	}
	result.state = currentState;
	return result;
}

} // namespace octavia

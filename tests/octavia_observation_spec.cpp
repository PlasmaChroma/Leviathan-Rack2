#include "OctaviaObservation.hpp"

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <thread>

namespace {

int failures = 0;

void check(bool condition, const std::string& name) {
	std::cout << "[" << (condition ? "PASS" : "FAIL") << "] " << name << "\n";
	if (!condition) ++failures;
}

std::array<float, octavia::OBSERVATION_CHANNELS> valuesFor(uint64_t frame) {
	std::array<float, octavia::OBSERVATION_CHANNELS> values{{}};
	for (size_t channel = 0; channel < values.size(); ++channel)
		values[channel] = static_cast<float>(frame * 10 + channel);
	return values;
}

std::array<uint8_t, octavia::OBSERVATION_CHANNELS> monoChannels(uint8_t mask) {
	std::array<uint8_t, octavia::OBSERVATION_CHANNELS> channels{{}};
	for (size_t channel = 0; channel < channels.size(); ++channel)
		channels[channel] = mask & (1u << channel) ? 1 : 0;
	return channels;
}

void publish(octavia::ObservationHistory& history, uint64_t frame, float sampleRate,
		uint8_t mask = 0x3f) {
	history.publish(frame, sampleRate, valuesFor(frame), mask, monoChannels(mask));
}

void testChannelNames() {
	octavia::ObserveChannel channel = octavia::ObserveChannel::Count;
	check(octavia::parseObserveChannel("master_l", &channel)
		&& channel == octavia::ObserveChannel::MasterL,
		"Master aliases parse without changing stable channel identity");
	check(octavia::parseObserveChannel("d", &channel)
		&& channel == octavia::ObserveChannel::D
		&& std::string(octavia::observeChannelName(channel)) == "D",
		"A-D parsing is case-insensitive and names remain stable");
}

void testAlignedHistoryAndConnections() {
	octavia::ObservationHistory history;
	const uint8_t mask = 0x3f & ~octavia::observeChannelBit(octavia::ObserveChannel::C);
	for (uint64_t frame = 100; frame <= 103; ++frame) publish(history, frame, 48000.f, mask);
	octavia::FrozenObservation frozen;
	std::string error;
	const bool ok = history.freeze(100, 103, 0x3f, &frozen, &error);
	bool aligned = ok && frozen.samples[0].size() == 4 && frozen.samples[5].size() == 4;
	for (size_t index = 0; aligned && index < 4; ++index) {
		for (size_t channel = 0; channel < octavia::OBSERVATION_CHANNELS; ++channel)
			aligned = frozen.samples[channel][index] == valuesFor(100 + index)[channel];
	}
	check(aligned, "all six channels freeze from identical start/end Rack frames");
	check(ok && !(frozen.allConnectedMask & (1u << 4))
		&& !(frozen.anyConnectedMask & (1u << 4))
		&& frozen.connectionMasks.size() == 4,
		"disconnected monitor state is retained independently from sample values");
	check(history.currentChannelCount(octavia::ObserveChannel::A) == 1
		&& history.currentChannelCount(octavia::ObserveChannel::C) == 0,
		"physical channel counts are published as monitor metadata");
}

void testWrapAndSampleRate() {
	octavia::ObservationHistory history;
	for (uint64_t frame = 0; frame <= octavia::OBSERVATION_HISTORY_FRAMES; ++frame)
		publish(history, frame, frame == octavia::OBSERVATION_HISTORY_FRAMES ? 96000.f : 44100.f);
	octavia::ObservationFrame frame;
	check(!history.readFrame(0, &frame)
		&& history.oldestAvailableFrame() == 1,
		"ring wrap rejects an overwritten frame tag instead of returning torn data");
	check(history.readFrame(octavia::OBSERVATION_HISTORY_FRAMES, &frame)
		&& frame.sampleRate == 96000.f
		&& history.currentSampleRate() == 96000.f,
		"sample-rate changes are attached to frame and current timing metadata");
	octavia::FrozenObservation expired;
	std::string error;
	check(!history.freeze(0, 1, 1, &expired, &error) && error == "snapshot_expired",
		"expired snapshot windows fail explicitly");
}

void testPendingPostrollAndPool() {
	octavia::ObservationHistory history;
	for (uint64_t frame = 10; frame <= 12; ++frame) publish(history, frame, 48000.f);
	octavia::ObservationSnapshotPool pool(&history);
	octavia::ObservationSnapshot snapshot;
	std::string error;
	const auto started = std::chrono::steady_clock::now();
	const bool created = pool.create(2, 2,
		octavia::observeChannelBit(octavia::ObserveChannel::A)
			| octavia::observeChannelBit(octavia::ObserveChannel::B),
		"post-roll", &snapshot, &error);
	const double elapsedMs = std::chrono::duration<double, std::milli>(
		std::chrono::steady_clock::now() - started).count();
	check(created && snapshot.state == octavia::SnapshotState::Pending && elapsedMs < 20.0,
		"post-roll request becomes pending without blocking publication");
	check(snapshot.observation.triggerFrame == 12 && snapshot.observation.startFrame == 10
		&& snapshot.observation.endFrame == 14,
		"pre/post-roll frames are anchored to the latest published frame");
	publish(history, 13, 48000.f);
	pool.get(snapshot.observation.id, &snapshot);
	check(snapshot.state == octavia::SnapshotState::Pending,
		"post-roll remains pending until its exact end frame exists");
	publish(history, 14, 48000.f);
	pool.get(snapshot.observation.id, &snapshot);
	check(snapshot.state == octavia::SnapshotState::Complete
		&& snapshot.observation.samples[2].size() == 5
		&& snapshot.observation.samples[3].front() == valuesFor(10)[3]
		&& snapshot.observation.samples[3].back() == valuesFor(14)[3],
		"completed post-roll snapshot freezes a sample-aligned immutable window");
	check(history.snapshotGeneration(octavia::ObserveChannel::A) == 1
		&& history.snapshotGeneration(octavia::ObserveChannel::B) == 1
		&& history.snapshotGeneration(octavia::ObserveChannel::C) == 0,
		"snapshot attention generation marks only selected monitors");
}

void testLegacyRecentWindow() {
	octavia::ObservationHistory history;
	for (uint64_t frame = 20; frame <= 22; ++frame) publish(history, frame, 44100.f);
	std::vector<float> recent;
	float sampleRate = 0.f;
	const bool ok = history.copyLatest(octavia::ObserveChannel::MasterR, 5, &recent, &sampleRate);
	check(ok && recent.size() == 5 && recent[0] == 0.f && recent[1] == 0.f
		&& recent[2] == valuesFor(20)[1] && recent[4] == valuesFor(22)[1]
		&& sampleRate == 44100.f,
		"legacy Master windows are routed through history with safe startup padding");
}

void testBoundedSnapshotPool() {
	octavia::ObservationHistory history;
	publish(history, 100, 48000.f);
	octavia::ObservationSnapshotPool pool(&history);
	octavia::ObservationSnapshot snapshot;
	std::string error;
	uint64_t firstId = 0;
	for (size_t i = 0; i < octavia::SNAPSHOT_POOL_LIMIT + 3; ++i) {
		const bool created = pool.create(0, 0, 1, "bounded", &snapshot, &error);
		if (i == 0) firstId = snapshot.observation.id;
		if (!created) break;
	}
	check(pool.size() == octavia::SNAPSHOT_POOL_LIMIT && !pool.get(firstId, &snapshot),
		"snapshot pool remains bounded and evicts its oldest completed capture");
}

void testExactTriggerFrame() {
	octavia::ObservationHistory history;
	for (uint64_t frame = 100; frame <= 120; ++frame) publish(history, frame, 48000.f);
	octavia::ObservationSnapshotPool pool(&history);
	octavia::ObservationSnapshot snapshot;
	std::string error;
	check(pool.createAt(110, 3, 4, octavia::observeChannelBit(octavia::ObserveChannel::C),
		"sibyl-exact", &snapshot, &error)
		&& snapshot.observation.triggerFrame == 110
		&& snapshot.observation.startFrame == 107
		&& snapshot.observation.endFrame == 114
		&& snapshot.observation.samples[4].front() == valuesFor(107)[4]
		&& snapshot.observation.samples[4].back() == valuesFor(114)[4],
		"explicit trigger frame freezes exact pre/post-roll history independent of latest frame");
}

void testConcurrentPublicationAndReads() {
	octavia::ObservationHistory history;
	std::atomic<bool> done{false};
	std::atomic<bool> coherent{true};
	std::thread publisher([&] {
		for (uint64_t frame = 1; frame <= 100000; ++frame) publish(history, frame, 48000.f);
		done.store(true, std::memory_order_release);
	});
	std::thread reader([&] {
		while (!done.load(std::memory_order_acquire)) {
			if (!history.hasPublishedFrame()) continue;
			const uint64_t frameNumber = history.publishedFrame();
			octavia::ObservationFrame frame;
			if (!history.readFrame(frameNumber, &frame)) continue;
			for (size_t channel = 0; channel < octavia::OBSERVATION_CHANNELS; ++channel) {
				if (frame.volts[channel] != static_cast<float>(frameNumber * 10 + channel))
					coherent.store(false, std::memory_order_relaxed);
			}
		}
	});
	publisher.join();
	reader.join();
	check(coherent.load(std::memory_order_relaxed),
		"concurrent publication never exposes a mixed or torn six-channel frame");
}

} // namespace

int main() {
	testChannelNames();
	testAlignedHistoryAndConnections();
	testWrapAndSampleRate();
	testPendingPostrollAndPool();
	testLegacyRecentWindow();
	testBoundedSnapshotPool();
	testExactTriggerFrame();
	testConcurrentPublicationAndReads();
	std::cout << "[SUMMARY] octavia_observation_spec: "
		<< (failures ? "failed" : "passed") << "\n";
	return failures ? 1 : 0;
}

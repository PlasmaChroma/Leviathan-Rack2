#include "OctaviaMeasurement.hpp"

#include <atomic>
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

void process(octavia::MasterMeasurement& measurement, uint64_t frame,
		float sampleRate = 100.f, double left = 0.5, double right = -0.25,
		double kLeft = 0.1, double kRight = 0.2) {
	measurement.process(frame, sampleRate, {{left, right}}, {{kLeft, kRight}});
}

void testIdleAndExactDuration() {
	octavia::MasterMeasurement measurement;
	std::atomic<double> atomicProbe{0.0};
	check(atomicProbe.is_lock_free(),
		"published double metrics are lock-free on the target runtime");
	for (uint64_t frame = 0; frame < 100; ++frame) process(measurement, frame);
	check(measurement.state() == octavia::MeasurementState::Idle
		&& measurement.read().measuredFrames == 0,
		"idle processing performs no long-window accumulation");
	uint64_t id = 0; std::string error;
	check(measurement.arm(4, false, &id, &error) && id > 0,
		"duration measurement arms explicitly");
	for (uint64_t frame = 100; frame < 104; ++frame) process(measurement, frame);
	const octavia::MeasurementResult result = measurement.read();
	check(result.state == octavia::MeasurementState::Complete
		&& result.startFrame == 100 && result.endFrame == 103
		&& result.measuredFrames == 4 && result.targetFrames == 4,
		"measurement starts and completes on exact engine-frame boundaries");
	check(std::fabs(result.rawSum[0] - 1.0) < 1e-12
		&& std::fabs(result.rawSum[1] - 0.25) < 1e-12
		&& std::fabs(result.kSum[0] - 0.4) < 1e-12
		&& std::fabs(result.kSum[1] - 0.8) < 1e-12
		&& std::fabs(result.sumLR + 0.5) < 1e-12,
		"triggered session accumulates RMS, K power, and stereo products accurately");
}

void testBlocksClippingAndConflict() {
	octavia::MasterMeasurement measurement;
	uint64_t id = 0; std::string error;
	measurement.arm(35, false, &id, &error);
	uint64_t rejectedId = 0;
	check(!measurement.arm(4, false, &rejectedId, &error) && error == "measurement_busy",
		"a second heavyweight measurement is rejected explicitly");
	for (uint64_t frame = 1; frame <= 35; ++frame)
		process(measurement, frame, 100.f, 1.25, -1.1, 0.1, 0.2);
	const octavia::MeasurementResult result = measurement.read();
	check(result.blockPowers.size() == 3
		&& std::fabs(result.blockPowers[0] - 0.3f) < 1e-6f,
		"triggered measurement owns its 100 ms loudness block history");
	check(result.clipped[0] == 35 && result.clipped[1] == 35
		&& result.peak[0] == 1.25 && result.peak[1] == 1.1,
		"clipping and maximum peaks accumulate only during the session");
}

void testOpenEndedResetAndReplacement() {
	octavia::MasterMeasurement measurement;
	uint64_t first = 0, second = 0; std::string error;
	measurement.arm(0, true, &first, &error);
	for (uint64_t frame = 1; frame <= 2050; ++frame) process(measurement, frame);
	octavia::MeasurementResult active = measurement.read();
	check(active.state == octavia::MeasurementState::Active
		&& active.measuredFrames >= 2048 && active.targetFrames == 0,
		"open-ended legacy reset/read session publishes bounded live progress");
	measurement.arm(2, true, &second, &error);
	process(measurement, 2051);
	process(measurement, 2052);
	const octavia::MeasurementResult replaced = measurement.read();
	check(second != first && replaced.id == second
		&& replaced.state == octavia::MeasurementState::Complete
		&& replaced.measuredFrames == 2,
		"explicit reset replacement starts a fresh session without stale sums");
}

void testSampleRateCancellation() {
	octavia::MasterMeasurement measurement;
	uint64_t id = 0; std::string error;
	measurement.arm(100, false, &id, &error);
	process(measurement, 10, 48000.f);
	process(measurement, 11, 96000.f);
	check(measurement.read().state == octavia::MeasurementState::Cancelled,
		"sample-rate changes cancel rather than mis-time an active session");
}

void testConcurrentReadPublication() {
	octavia::MasterMeasurement measurement;
	uint64_t id = 0; std::string error;
	measurement.arm(100000, false, &id, &error);
	std::atomic<bool> done{false};
	std::atomic<bool> coherent{true};
	std::thread audio([&] {
		for (uint64_t frame = 1; frame <= 100000; ++frame) process(measurement, frame);
		done.store(true, std::memory_order_release);
	});
	std::thread reader([&] {
		while (!done.load(std::memory_order_acquire)) {
			const octavia::MeasurementResult result = measurement.read();
			if (result.measuredFrames > result.targetFrames || result.endFrame < result.startFrame)
				coherent.store(false, std::memory_order_relaxed);
		}
	});
	audio.join(); reader.join();
	check(coherent.load(std::memory_order_relaxed)
		&& measurement.read().state == octavia::MeasurementState::Complete,
		"concurrent status reads remain coherent while audio publishes progress");
}

} // namespace

int main() {
	testIdleAndExactDuration();
	testBlocksClippingAndConflict();
	testOpenEndedResetAndReplacement();
	testSampleRateCancellation();
	testConcurrentReadPublication();
	std::cout << "[SUMMARY] octavia_measurement_spec: "
		<< (failures ? "failed" : "passed") << "\n";
	return failures ? 1 : 0;
}

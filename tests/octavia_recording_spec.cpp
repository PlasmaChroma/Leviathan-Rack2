#include "OctaviaRecording.hpp"

#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

namespace {

int failures = 0;

void check(bool condition, const std::string& name) {
	std::cout << "[" << (condition ? "PASS" : "FAIL") << "] " << name << "\n";
	if (!condition) ++failures;
}

uint16_t le16(const unsigned char* bytes) {
	return static_cast<uint16_t>(bytes[0])
		| static_cast<uint16_t>(bytes[1] << 8);
}

uint32_t le32(const unsigned char* bytes) {
	return static_cast<uint32_t>(bytes[0])
		| (static_cast<uint32_t>(bytes[1]) << 8)
		| (static_cast<uint32_t>(bytes[2]) << 16)
		| (static_cast<uint32_t>(bytes[3]) << 24);
}

void testBoundedCaptureAndExport() {
	octavia::RecordingEngine engine;
	octavia::RecordingStatus status;
	std::string error;
	const uint8_t selected = octavia::observeChannelBit(octavia::ObserveChannel::A)
		| octavia::observeChannelBit(octavia::ObserveChannel::B);
	check(engine.arm(selected, 0.1, 100.f, "A/B exact", "build/tests",
		&status, &error) && status.targetFrames == 10 && status.channelCount == 2,
		"arming preallocates the exact bounded planar frame count");
	const uint64_t id = status.id;
	check(!engine.arm(selected, 0.1, 100.f, "busy", "build/tests",
		&status, &error) && error == "recording_busy",
		"a second recording cannot replace an active audio-thread buffer");

	for (uint64_t frame = 100; frame < 110; ++frame) {
		std::array<float, octavia::OBSERVATION_CHANNELS> volts{{}};
		volts[2] = static_cast<float>(frame) + 0.25f;
		volts[3] = -static_cast<float>(frame) - 0.5f;
		const uint8_t connected = (frame == 105 ? selected & ~(1u << 3) : selected)
			| (1u << 5);
		engine.process(frame, 100.f, volts, connected);
	}
	const bool captured = engine.get(id, &status)
		&& (status.state == octavia::RecordingState::Captured
			|| status.state == octavia::RecordingState::Processing
			|| status.state == octavia::RecordingState::Complete)
		&& status.startFrame == 100 && status.endFrame == 109
		&& status.writtenFrames == 10 && status.allConnectedMask == (1u << 2)
		&& status.anyConnectedMask == selected;
	check(captured,
		"capture preserves exact Rack frame bounds and connection history");

	for (int attempt = 0; attempt < 1000; ++attempt) {
		engine.get(id, &status);
		if (status.state == octavia::RecordingState::Complete
				|| status.state == octavia::RecordingState::Failed) break;
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
	check(status.state == octavia::RecordingState::Complete,
		"dedicated worker exports completed samples off the audio thread");
	std::ifstream wav(status.wavPath.c_str(), std::ios::binary);
	std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(wav)),
		std::istreambuf_iterator<char>());
	const bool header = bytes.size() == 44 + 10 * 2 * sizeof(float)
		&& std::string(reinterpret_cast<const char*>(bytes.data()), 4) == "RIFF"
		&& std::string(reinterpret_cast<const char*>(bytes.data() + 8), 4) == "WAVE"
		&& le16(bytes.data() + 20) == 3 && le16(bytes.data() + 22) == 2
		&& le32(bytes.data() + 24) == 100 && le32(bytes.data() + 40) == 80;
	check(header, "WAV is lossless interleaved IEEE float32 at Rack's sample rate");
	float first[2] = {};
	if (bytes.size() >= 44 + sizeof(first))
		std::memcpy(first, bytes.data() + 44, sizeof(first));
	check(std::fabs(first[0] - 100.25f) < 1e-6f
		&& std::fabs(first[1] + 100.5f) < 1e-6f,
		"WAV stores unscaled Rack volts in requested monitor order");

	std::ifstream metadata(status.metadataPath.c_str());
	const std::string json((std::istreambuf_iterator<char>(metadata)),
		std::istreambuf_iterator<char>());
	check(json.find("\"startFrame\": 100") != std::string::npos
		&& json.find("\"endFrame\": 109") != std::string::npos
		&& json.find("\"sampleFormat\": \"ieee_float32_rack_volts\"") != std::string::npos
		&& json.find("\"channels\": [\"A\", \"B\"]") != std::string::npos,
		"sidecar records frame identity, voltage units, and channel order");
	std::remove(status.wavPath.c_str());
	std::remove(status.metadataPath.c_str());
}

void testSampleRateChangeFailsSafely() {
	octavia::RecordingEngine engine;
	octavia::RecordingStatus status;
	std::string error;
	const uint8_t selected = octavia::observeChannelBit(octavia::ObserveChannel::MasterL);
	check(engine.arm(selected, 0.1, 100.f, "rate-change", "build/tests",
		&status, &error), "sample-rate failure fixture arms");
	const uint64_t id = status.id;
	std::array<float, octavia::OBSERVATION_CHANNELS> volts{{}};
	engine.process(10, 100.f, volts, selected);
	engine.process(11, 200.f, volts, selected);
	for (int attempt = 0; attempt < 100 && engine.busy(); ++attempt)
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	check(engine.get(id, &status) && status.state == octavia::RecordingState::Failed
		&& status.error == "sample_rate_changed" && !engine.busy(),
		"mid-capture sample-rate changes fail instead of mislabeling the WAV");
}

void testEphemeralAnalysisSkipsDisk() {
	octavia::AnalysisEngine analysisEngine;
	octavia::RecordingEngine engine(&analysisEngine);
	octavia::CaptureAnalysisRequest request;
	request.kind = octavia::CaptureAnalysisKind::Group;
	request.group.first = octavia::ObserveChannel::A;
	request.detailed = false;
	octavia::RecordingStatus status;
	std::string error;
	const uint8_t selected = octavia::observeChannelBit(octavia::ObserveChannel::A);
	check(engine.armCapture(selected, 0.1, 100.f, "ephemeral",
		octavia::CaptureDisposition::Analyze, request, "", &status, &error),
		"analysis-only capture arms without an output directory");
	const uint64_t id = status.id;
	for (uint64_t frame = 0; frame < 10; ++frame) {
		std::array<float, octavia::OBSERVATION_CHANNELS> volts{{}};
		volts[2] = static_cast<float>(frame + 1);
		engine.process(frame, 100.f, volts, selected);
	}
	for (int attempt = 0; attempt < 1000; ++attempt) {
		engine.get(id, &status);
		if (status.state == octavia::RecordingState::Complete
				|| status.state == octavia::RecordingState::Failed) break;
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
	const float expectedRms = std::sqrt(38.5f);
	check(status.state == octavia::RecordingState::Complete
		&& status.analysisAvailable
		&& status.analysisKind == octavia::CaptureAnalysisKind::Group
		&& std::fabs(status.groupAnalysis.mono.rms - expectedRms) < 1e-5f,
		"ephemeral capture returns full-window analysis results");
	check(status.wavPath.empty() && status.metadataPath.empty(),
		"analysis-only capture makes no disk commitment");
}

void testAnalyzeAndRecordUsesIdenticalFrames() {
	octavia::AnalysisEngine analysisEngine;
	octavia::RecordingEngine engine(&analysisEngine);
	octavia::CaptureAnalysisRequest request;
	request.kind = octavia::CaptureAnalysisKind::Comparison;
	request.reference.first = octavia::ObserveChannel::A;
	request.target.first = octavia::ObserveChannel::B;
	request.detailed = false;
	octavia::RecordingStatus status;
	std::string error;
	const uint8_t selected = octavia::observeChannelBit(octavia::ObserveChannel::A)
		| octavia::observeChannelBit(octavia::ObserveChannel::B);
	check(engine.armCapture(selected, 0.1, 100.f, "analyze-and-record",
		octavia::CaptureDisposition::AnalyzeAndRecord, request, "build/tests",
		&status, &error), "combined capture arms analysis and archival work together");
	const uint64_t id = status.id;
	for (uint64_t frame = 0; frame < 10; ++frame) {
		std::array<float, octavia::OBSERVATION_CHANNELS> volts{{}};
		volts[2] = static_cast<float>(frame + 1);
		volts[3] = 2.f * volts[2];
		engine.process(frame, 100.f, volts, selected);
	}
	for (int attempt = 0; attempt < 1000; ++attempt) {
		engine.get(id, &status);
		if (status.state == octavia::RecordingState::Complete
				|| status.state == octavia::RecordingState::Failed) break;
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
	check(status.state == octavia::RecordingState::Complete
		&& status.analysisAvailable
		&& std::fabs(status.comparisonAnalysis.rmsDeltaDb - 6.0206f) < 0.001f
		&& !status.wavPath.empty() && !status.metadataPath.empty(),
		"combined mode analyzes and archives the identical captured frames");
	std::remove(status.wavPath.c_str());
	std::remove(status.metadataPath.c_str());
}

} // namespace

int main() {
	testBoundedCaptureAndExport();
	testSampleRateChangeFailsSafely();
	testEphemeralAnalysisSkipsDisk();
	testAnalyzeAndRecordUsesIdenticalFrames();
	std::cout << (failures ? "FAIL" : "PASS") << ": octavia recording contract\n";
	return failures ? 1 : 0;
}

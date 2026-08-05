#include "../src/LongplayerStream.hpp"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

namespace {

struct Result {
	std::string name;
	bool passed = false;
	std::string detail;
};

void writeLe16(std::ofstream& output, unsigned value) {
	const unsigned char bytes[] = {
		static_cast<unsigned char>(value),
		static_cast<unsigned char>(value >> 8)
	};
	output.write(reinterpret_cast<const char*>(bytes), sizeof(bytes));
}

void writeLe32(std::ofstream& output, unsigned value) {
	const unsigned char bytes[] = {
		static_cast<unsigned char>(value),
		static_cast<unsigned char>(value >> 8),
		static_cast<unsigned char>(value >> 16),
		static_cast<unsigned char>(value >> 24)
	};
	output.write(reinterpret_cast<const char*>(bytes), sizeof(bytes));
}

bool writeSparseWav(
	const std::string& path,
	unsigned sampleRate,
	unsigned seconds,
	bool includeTestImpulse) {
	const unsigned frames = sampleRate * seconds;
	const unsigned dataBytes = frames * 2u;
	std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);
	if (!output) return false;
	output.write("RIFF", 4);
	writeLe32(output, 36u + dataBytes);
	output.write("WAVEfmt ", 8);
	writeLe32(output, 16u);
	writeLe16(output, 1u);
	writeLe16(output, 1u);
	writeLe32(output, sampleRate);
	writeLe32(output, sampleRate * 2u);
	writeLe16(output, 2u);
	writeLe16(output, 16u);
	output.write("data", 4);
	writeLe32(output, dataBytes);
	if (includeTestImpulse) {
		writeLe16(output, 16384u);
	}
	if (dataBytes > (includeTestImpulse ? 2u : 0u)) {
		output.seekp(std::streamoff(44u + dataBytes - 1u));
		output.put('\0');
	}
	return bool(output);
}

bool waitForReady(longplayer::Stream& stream, int timeoutMs = 10000) {
	const auto deadline = std::chrono::steady_clock::now()
		+ std::chrono::milliseconds(timeoutMs);
	while (std::chrono::steady_clock::now() < deadline) {
		if (stream.ready()) return true;
		if (!stream.loading() && !stream.error().empty()) return false;
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	return false;
}

bool waitForFrame(
	longplayer::Stream& stream,
	std::uint64_t frame,
	float* left,
	float* right,
	int timeoutMs = 10000) {
	stream.setDesiredFrame(frame, false);
	const auto deadline = std::chrono::steady_clock::now()
		+ std::chrono::milliseconds(timeoutMs);
	while (std::chrono::steady_clock::now() < deadline) {
		if (stream.readFrame(frame, left, right)) return true;
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
	return false;
}

Result wavStreamsAndReads() {
	const std::string path = "/tmp/leviathan_longplayer_short.wav";
	const bool wrote = writeSparseWav(path, 8000u, 2u, true);
	longplayer::Stream stream;
	stream.requestLoad(path);
	const bool ready = waitForReady(stream);
	float left = 0.f;
	float right = 0.f;
	const bool read = ready && waitForFrame(stream, 0u, &left, &right);
	std::remove(path.c_str());
	return {
		"WAV streams from disk and preserves mono duplication",
		wrote && ready && read && std::fabs(left - 0.5f) < 1e-4f
			&& std::fabs(right - left) < 1e-7f,
		"ready=" + std::to_string(ready)
			+ " read=" + std::to_string(read)
			+ " sample=" + std::to_string(left)
	};
}

Result hourWavSeeksWithoutWholeFileDecode() {
	const std::string path = "/tmp/leviathan_longplayer_hour.wav";
	const unsigned sampleRate = 8000u;
	const unsigned seconds = 3600u;
	const bool wrote = writeSparseWav(path, sampleRate, seconds, false);
	longplayer::Stream stream;
	stream.requestLoad(path);
	const bool ready = waitForReady(stream);
	const std::uint64_t target = std::uint64_t(sampleRate) * 55u * 60u;
	float left = 1.f;
	float right = 1.f;
	const bool read = ready && waitForFrame(stream, target, &left, &right);
	const double duration = stream.sampleRate() > 0u
		? double(stream.totalFrames()) / stream.sampleRate()
		: 0.0;
	std::remove(path.c_str());
	return {
		"A one-hour WAV opens and seeks near its end through the bounded cache",
		wrote && ready && read && std::fabs(duration - 3600.0) < 1e-6
			&& left == 0.f && right == 0.f,
		"duration=" + std::to_string(duration)
			+ " seekRead=" + std::to_string(read)
	};
}

Result compressedFormatStreams(const char* label, const std::string& path) {
	longplayer::Stream stream;
	stream.requestLoad(path);
	const bool ready = waitForReady(stream, 20000);
	float left = 0.f;
	float right = 0.f;
	const bool read = ready && waitForFrame(stream, 0u, &left, &right, 10000);
	return {
		std::string(label) + " opens through its streaming decoder",
		ready && read && stream.totalFrames() > 0u && stream.sampleRate() > 0u,
		"ready=" + std::to_string(ready)
			+ " frames=" + std::to_string(stream.totalFrames())
			+ " rate=" + std::to_string(stream.sampleRate())
			+ " error=" + stream.error()
	};
}

} // namespace

int main(int argc, char** argv) {
	if (argc > 1) {
		const Result result = compressedFormatStreams("Requested file", argv[1]);
		std::cout << (result.passed ? "[PASS] " : "[FAIL] ")
			<< result.name << " :: " << result.detail << '\n';
		return result.passed ? 0 : 1;
	}
	const Result results[] = {
		wavStreamsAndReads(),
		hourWavSeeksWithoutWholeFileDecode(),
		compressedFormatStreams(
			"MP3", "Samples/Doorstop/59737__morgantj__doorstopperspring2.mp3")
	};
	int failures = 0;
	for (const Result& result : results) {
		std::cout << (result.passed ? "[PASS] " : "[FAIL] ")
			<< result.name << " :: " << result.detail << '\n';
		if (!result.passed) ++failures;
	}
	std::cout << "[SUMMARY] longplayer_stream_spec: "
		<< (int(sizeof(results) / sizeof(results[0])) - failures)
		<< "/" << int(sizeof(results) / sizeof(results[0])) << " passed\n";
	return failures == 0 ? 0 : 1;
}

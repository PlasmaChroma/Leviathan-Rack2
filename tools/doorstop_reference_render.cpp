#include "../src/ReferenceSpringEngine.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Strike {
	float time = 0.05f;
	float velocity = 0.75f;
};

void writeU16(std::ostream& out, std::uint16_t value) {
	const char bytes[] = {
		char(value & 0xffu),
		char((value >> 8) & 0xffu)
	};
	out.write(bytes, 2);
}

void writeU32(std::ostream& out, std::uint32_t value) {
	const char bytes[] = {
		char(value & 0xffu),
		char((value >> 8) & 0xffu),
		char((value >> 16) & 0xffu),
		char((value >> 24) & 0xffu)
	};
	out.write(bytes, 4);
}

void writeFloatWav(const std::string& path, int sampleRate,
	const std::vector<float>& samples) {
	const std::uint32_t dataBytes =
		std::uint32_t(samples.size() * sizeof(float));
	std::ofstream out(path, std::ios::binary);
	if (!out) throw std::runtime_error("cannot open output WAV: " + path);
	out.write("RIFF", 4);
	writeU32(out, 36u + dataBytes);
	out.write("WAVEfmt ", 8);
	writeU32(out, 16u);
	writeU16(out, 3u); // IEEE float
	writeU16(out, 1u);
	writeU32(out, std::uint32_t(sampleRate));
	writeU32(out, std::uint32_t(sampleRate * int(sizeof(float))));
	writeU16(out, std::uint16_t(sizeof(float)));
	writeU16(out, 32u);
	out.write("data", 4);
	writeU32(out, dataBytes);
	out.write(reinterpret_cast<const char*>(samples.data()), dataBytes);
	if (!out) throw std::runtime_error("failed while writing WAV: " + path);
}

float parseFloat(const char* text, const char* option) {
	char* end = nullptr;
	const float value = std::strtof(text, &end);
	if (!end || *end != '\0') {
		throw std::runtime_error(std::string("invalid value for ") + option);
	}
	return value;
}

std::uint32_t parseSeed(const char* text) {
	char* end = nullptr;
	const unsigned long value = std::strtoul(text, &end, 0);
	if (!end || *end != '\0' || value == 0 || value > 0xfffffffful) {
		throw std::runtime_error("seed must be in [1, 0xffffffff]");
	}
	return std::uint32_t(value);
}

Strike parseStrike(const std::string& text) {
	const std::size_t separator = text.find(':');
	if (separator == std::string::npos) {
		throw std::runtime_error("strike must use TIME:VELOCITY");
	}
	Strike strike;
	strike.time = parseFloat(text.substr(0, separator).c_str(), "--strike");
	strike.velocity =
		parseFloat(text.substr(separator + 1).c_str(), "--strike");
	if (strike.time < 0.f || strike.velocity < -1.f || strike.velocity > 1.f) {
		throw std::runtime_error("strike time/velocity is out of range");
	}
	return strike;
}

void usage(const char* executable) {
	std::cerr
		<< "Usage: " << executable << " OUTPUT.wav [options]\n"
		<< "  --sample-rate HZ    default 48000\n"
		<< "  --duration SECONDS  default 6\n"
		<< "  --velocity VALUE    default 0.75, strike at 0.05 s\n"
		<< "  --strike TIME:VALUE repeatable, replaces the default strike\n"
		<< "  --seed INTEGER      default 305419896 (0x12345678)\n"
		<< "  --break-in VALUE    default 0, range [0, 1]\n";
}

} // namespace

int main(int argc, char** argv) {
	try {
		if (argc < 2) {
			usage(argv[0]);
			return 2;
		}
		const std::string outputPath = argv[1];
		int sampleRate = 48000;
		float duration = 6.f;
		float defaultVelocity = 0.75f;
		float breakIn = 0.f;
		std::uint32_t seed = 0x12345678u;
		std::vector<Strike> strikes;
		for (int i = 2; i < argc; ++i) {
			const std::string option = argv[i];
			if (i + 1 >= argc) {
				throw std::runtime_error("missing value for " + option);
			}
			const char* value = argv[++i];
			if (option == "--sample-rate") {
				sampleRate = int(parseFloat(value, option.c_str()));
			}
			else if (option == "--duration") {
				duration = parseFloat(value, option.c_str());
			}
			else if (option == "--velocity") {
				defaultVelocity = parseFloat(value, option.c_str());
			}
			else if (option == "--seed") {
				seed = parseSeed(value);
			}
			else if (option == "--break-in") {
				breakIn = parseFloat(value, option.c_str());
			}
			else if (option == "--strike") {
				strikes.push_back(parseStrike(value));
			}
			else {
				throw std::runtime_error("unknown option: " + option);
			}
		}
		if (sampleRate < 1000 || duration <= 0.f || breakIn < 0.f
			|| breakIn > 1.f || defaultVelocity < -1.f
			|| defaultVelocity > 1.f) {
			throw std::runtime_error("render option is out of range");
		}
		if (strikes.empty()) strikes.push_back({0.05f, defaultVelocity});
		std::sort(strikes.begin(), strikes.end(),
			[](const Strike& a, const Strike& b) { return a.time < b.time; });

		doorstop::ReferenceSpringEngine engine;
		engine.setSampleRate(float(sampleRate));
		engine.setSpecimenSeed(seed);
		engine.setBreakIn(breakIn);
		const std::size_t sampleCount =
			std::size_t(duration * float(sampleRate));
		std::vector<float> samples(sampleCount, 0.f);
		std::size_t nextStrike = 0;
		for (std::size_t i = 0; i < sampleCount; ++i) {
			const float time = float(i) / float(sampleRate);
			while (nextStrike < strikes.size()
				&& strikes[nextStrike].time <= time) {
				engine.strike(strikes[nextStrike].velocity);
				++nextStrike;
			}
			samples[i] = engine.process(1.f / float(sampleRate)).outputVolts / 5.f;
		}
		writeFloatWav(outputPath, sampleRate, samples);
		std::cout << outputPath << " rate=" << sampleRate
			<< " duration=" << duration
			<< " seed=" << seed
			<< " breakIn=" << breakIn
			<< " strikes=" << strikes.size() << "\n";
		return 0;
	}
	catch (const std::exception& error) {
		std::cerr << "doorstop_reference_render: " << error.what() << "\n";
		return 1;
	}
}

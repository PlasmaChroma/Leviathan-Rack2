#include "../src/PhonexEngine.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>

namespace {

void writeU16(std::ofstream& out, std::uint16_t value) {
	const char bytes[] = {static_cast<char>(value), static_cast<char>(value >> 8)};
	out.write(bytes, 2);
}

void writeU32(std::ofstream& out, std::uint32_t value) {
	const char bytes[] = {static_cast<char>(value), static_cast<char>(value >> 8),
		static_cast<char>(value >> 16), static_cast<char>(value >> 24)};
	out.write(bytes, 4);
}

phonex::LpcSequence makeAuditionSequence() {
	phonex::LpcSequence sequence;
	sequence.frameCount = 70;
	for (std::uint16_t i = 0; i < sequence.frameCount; ++i) {
		auto& frame = sequence.frames[i];
		if (i < 4 || i >= 65 || (i >= 28 && i < 33))
			continue;
		frame.energy = (i < 28 ? 0.58f : 0.48f);
		frame.pitchPeriod10k = i < 28 ? 82.f : 68.f;
		frame.excitation = (i >= 52 && i < 58)
			? phonex::Excitation::Unvoiced : phonex::Excitation::Voiced;
		const float morph = static_cast<float>(i % 16) / 15.f;
		frame.reflection = {{0.76f - 0.18f * morph, -0.62f + 0.21f * morph,
			0.47f, -0.34f, 0.25f, -0.18f, 0.13f, -0.09f, 0.06f, -0.03f}};
	}
	return sequence;
}

} // namespace

int main(int argc, char** argv) {
	const char* path = argc > 1 ? argv[1] : "phonex_phase2.wav";
	constexpr std::uint32_t sampleRate = 48000;
	constexpr std::uint32_t sampleCount = sampleRate * 2;
	std::ofstream out(path, std::ios::binary);
	if (!out) {
		std::cerr << "Could not open output: " << path << '\n';
		return 1;
	}
	out.write("RIFF", 4); writeU32(out, 36 + sampleCount * 2);
	out.write("WAVEfmt ", 8); writeU32(out, 16); writeU16(out, 1); writeU16(out, 1);
	writeU32(out, sampleRate); writeU32(out, sampleRate * 2); writeU16(out, 2); writeU16(out, 16);
	out.write("data", 4); writeU32(out, sampleCount * 2);
	const auto sequence = makeAuditionSequence();
	phonex::Engine engine;
	engine.setSequence(&sequence);
	phonex::EngineControls controls;
	controls.hostSampleRate = static_cast<float>(sampleRate);
	for (std::uint32_t i = 0; i < sampleCount; ++i) {
		const float voltage = engine.process(controls).audio;
		const float normalized = std::max(-1.f, std::min(1.f, voltage / 5.f));
		writeU16(out, static_cast<std::uint16_t>(static_cast<std::int16_t>(normalized * 32767.f)));
	}
	if (!out) return 1;
	std::cout << "Wrote PHONEX Phase 2 audition render: " << path << '\n';
	return 0;
}

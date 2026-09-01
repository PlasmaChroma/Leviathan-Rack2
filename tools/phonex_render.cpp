#include "../src/PhonexEngine.hpp"
#include "../src/PhonexFixtures.hpp"
#include "../src/PhonexPronunciation.hpp"
#include "../src/PhonexRom.hpp"
#include "../src/PhonexSequenceCompiler.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

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

} // namespace

int main(int argc, char** argv) {
	const char* path = argc > 1 ? argv[1] : "phonex_hello.wav";
	phonex::LpcSequence sequence;
	std::string auditionName;
	if (argc > 2 && std::string(argv[2]) == "--fixture") {
		sequence = phonex::makeVoicedFixture(18);
		auditionName = "PHASE 2 VOICED FIXTURE";
	}
	else if (argc > 2 && std::string(argv[2]) == "--text") {
		if (argc < 4) {
			std::cerr << "Usage: phonex_render OUTPUT --text UTTERANCE\n";
			return 1;
		}
		const phonex::TextCompileResult result = phonex::compileText(argv[3], sequence);
		if (result.status != phonex::CompileStatus::Ok) {
			std::cerr << "Could not compile text: " << phonex::compileStatusText(result.status) << '\n';
			return 1;
		}
		auditionName = argv[3];
	}
	else {
		const int phraseIndex = argc > 2 ? std::stoi(argv[2]) : 36;
		if (phraseIndex < 0 || phraseIndex >= static_cast<int>(phonex::kBundledPhraseCount)) {
			std::cerr << "Phrase index must be 0..63\n";
			return 1;
		}
		if (phonex::compileBundledPhrase(static_cast<std::uint8_t>(phraseIndex), sequence)
			!= phonex::CompileStatus::Ok) {
			std::cerr << "Could not compile bundled phrase " << phraseIndex << '\n';
			return 1;
		}
		const phonex::StringView phraseName = phonex::bundledPhraseName(phraseIndex);
		auditionName.assign(phraseName.data(), phraseName.size());
	}
	constexpr std::uint32_t sampleRate = 48000;
	const std::uint32_t sampleCount = sequence.frameCount * 960u + sampleRate / 4u;
	std::ofstream out(path, std::ios::binary);
	if (!out) {
		std::cerr << "Could not open output: " << path << '\n';
		return 1;
	}
	out.write("RIFF", 4); writeU32(out, 36 + sampleCount * 2);
	out.write("WAVEfmt ", 8); writeU32(out, 16); writeU16(out, 1); writeU16(out, 1);
	writeU32(out, sampleRate); writeU32(out, sampleRate * 2); writeU16(out, 2); writeU16(out, 16);
	out.write("data", 4); writeU32(out, sampleCount * 2);
	phonex::Engine engine;
	engine.setSequence(&sequence);
	engine.setReconstructionMode(phonex::ReconstructionMode::Filtered);
	phonex::EngineControls controls;
	controls.hostSampleRate = static_cast<float>(sampleRate);
	for (std::uint32_t i = 0; i < sampleCount; ++i) {
		const float voltage = engine.process(controls).audio;
		const float normalized = std::max(-1.f, std::min(1.f, voltage / 5.f));
		writeU16(out, static_cast<std::uint16_t>(static_cast<std::int16_t>(normalized * 32767.f)));
	}
	if (!out) return 1;
	std::cout << "Wrote PHONEX audition " << auditionName
		<< " (" << sequence.frameCount << " frames): " << path << '\n';
	return 0;
}

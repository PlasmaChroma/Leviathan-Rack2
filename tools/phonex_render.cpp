#include "../src/PhonexEngine.hpp"
#include "../src/PhonexFixtures.hpp"
#include "../src/PhonexPronunciation.hpp"
#include "../src/PhonexRom.hpp"
#include "../src/PhonexSequenceCompiler.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
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

bool parseInt(const char* text, int& value) {
	try {
		std::size_t consumed = 0;
		const std::string input(text ? text : "");
		value = std::stoi(input, &consumed);
		return consumed == input.size();
	}
	catch (const std::exception&) {
		return false;
	}
}

bool parseFloat(const char* text, float& value) {
	try {
		std::size_t consumed = 0;
		const std::string input(text ? text : "");
		value = std::stof(input, &consumed);
		return consumed == input.size();
	}
	catch (const std::exception&) {
		return false;
	}
}

void printUsage() {
	std::cerr
		<< "Usage: phonex_render OUTPUT [INDEX | --fixture | --text TEXT | --phones SCRIPT | --probe NAME]\n"
		<< "       [--sample-rate HZ] [--internal-rate 8000|10000] [--raw]\n"
		<< "       [--output-stage legacy|linear|limited]\n"
		<< "       [--forced-voiced|--forced-unvoiced] [--excite-blend 0..1] [--seed N]\n"
		<< "       [--filter-order 2|4|6] [--probe-frequency HZ]\n"
		<< "Probes: zero-voiced, zero-unvoiced, level-sweep, forced-voiced-unvoiced, reconstruction-sine\n";
}

bool makeProbe(const std::string& name, phonex::LpcSequence& sequence) {
	sequence = {};
	auto append = [&sequence](const phonex::LpcFrame& frame) {
		if (sequence.frameCount >= sequence.frames.size())
			return false;
		sequence.frames[sequence.frameCount++] = frame;
		return true;
	};
	phonex::LpcFrame frame;
	frame.energy = 0.5f;
	frame.pitchPeriod10k = 80.f;
	if (name == "zero-voiced") {
		frame.excitation = phonex::Excitation::Voiced;
		for (int i = 0; i < 24; ++i) append(frame);
	}
	else if (name == "zero-unvoiced" || name == "forced-voiced-unvoiced") {
		frame.excitation = phonex::Excitation::Unvoiced;
		frame.pitchPeriod10k = 0.f;
		for (int i = 0; i < 24; ++i) append(frame);
	}
	else if (name == "level-sweep") {
		frame.excitation = phonex::Excitation::Voiced;
		for (int level = 1; level <= 10; ++level) {
			frame.energy = 0.1f * static_cast<float>(level);
			for (int i = 0; i < 6; ++i) append(frame);
		}
	}
	else if (name == "reconstruction-sine") {
		frame.energy = 1.f;
		frame.excitation = phonex::Excitation::Silence;
		for (int i = 0; i < 24; ++i) append(frame);
	}
	else {
		return false;
	}
	append({});
	return true;
}

} // namespace

int main(int argc, char** argv) {
	if (argc < 2) {
		printUsage();
		return 2;
	}
	const char* path = argv[1];
	phonex::LpcSequence sequence;
	std::string auditionName;
	std::string mode;
	std::string source;
	int phraseIndex = 36;
	int sampleRate = 48000;
	int internalRate = 10000;
	int seed = 0x1ace1;
	float exciteBlend = 0.f;
	float probeFrequency = 1000.f;
	phonex::ReconstructionMode reconstruction = phonex::ReconstructionMode::Filtered;
	phonex::ReconstructionOrder filterOrder = phonex::ReconstructionOrder::SixPole;
	phonex::OutputStage outputStage = phonex::OutputStage::CalibratedLimited;
	phonex::ForcedExcitation forced = phonex::ForcedExcitation::Voiced;

	for (int i = 2; i < argc; ++i) {
		const std::string argument = argv[i];
		auto requireValue = [&](const char* option) -> const char* {
			if (i + 1 >= argc) {
				std::cerr << option << " requires a value\n";
				return nullptr;
			}
			return argv[++i];
		};
		if (argument == "--fixture") mode = "fixture";
		else if (argument == "--text" || argument == "--phones" || argument == "--probe") {
			const char* value = requireValue(argument.c_str());
			if (!value) return 2;
			mode = argument.substr(2);
			source = value;
		}
		else if (argument == "--sample-rate") {
			const char* value = requireValue("--sample-rate");
			if (!value || !parseInt(value, sampleRate)) return 2;
		}
		else if (argument == "--internal-rate") {
			const char* value = requireValue("--internal-rate");
			if (!value || !parseInt(value, internalRate)) return 2;
		}
		else if (argument == "--seed") {
			const char* value = requireValue("--seed");
			if (!value || !parseInt(value, seed)) return 2;
		}
		else if (argument == "--excite-blend") {
			const char* value = requireValue("--excite-blend");
			if (!value || !parseFloat(value, exciteBlend)) return 2;
		}
		else if (argument == "--probe-frequency") {
			const char* value = requireValue("--probe-frequency");
			if (!value || !parseFloat(value, probeFrequency)) return 2;
		}
		else if (argument == "--filter-order") {
			const char* value = requireValue("--filter-order");
			int order = 0;
			if (!value || !parseInt(value, order)) return 2;
			if (order == 2) filterOrder = phonex::ReconstructionOrder::TwoPole;
			else if (order == 4) filterOrder = phonex::ReconstructionOrder::FourPole;
			else if (order == 6) filterOrder = phonex::ReconstructionOrder::SixPole;
			else return 2;
		}
		else if (argument == "--raw") reconstruction = phonex::ReconstructionMode::RawHold;
		else if (argument == "--output-stage") {
			const char* value = requireValue("--output-stage");
			if (!value) return 2;
			const std::string name = value;
			if (name == "legacy") outputStage = phonex::OutputStage::LegacyCurve;
			else if (name == "linear") outputStage = phonex::OutputStage::CalibratedLinear;
			else if (name == "limited") outputStage = phonex::OutputStage::CalibratedLimited;
			else {
				std::cerr << "Unknown output stage: " << name << '\n';
				return 2;
			}
		}
		else if (argument == "--forced-voiced") forced = phonex::ForcedExcitation::Voiced;
		else if (argument == "--forced-unvoiced") forced = phonex::ForcedExcitation::Unvoiced;
		else if (!argument.empty() && argument[0] != '-' && mode.empty()) {
			if (!parseInt(argument.c_str(), phraseIndex)) {
				std::cerr << "Invalid phrase index: " << argument << '\n';
				return 2;
			}
			mode = "bundled";
		}
		else {
			std::cerr << "Unknown argument: " << argument << '\n';
			printUsage();
			return 2;
		}
	}

	if (sampleRate < 8000 || sampleRate > 768000 || (internalRate != 8000 && internalRate != 10000)
		|| exciteBlend < 0.f || exciteBlend > 1.f) {
		std::cerr << "Invalid rate or excitation blend\n";
		return 2;
	}
	if (mode.empty()) mode = "bundled";

	if (mode == "fixture") {
		sequence = phonex::makeVoicedFixture(18);
		auditionName = "PHASE 2 VOICED FIXTURE";
	}
	else if (mode == "text") {
		const phonex::TextCompileResult result = phonex::compileText(source, sequence);
		if (result.status != phonex::CompileStatus::Ok) {
			std::cerr << "Could not compile text: " << phonex::compileStatusText(result.status) << '\n';
			return 1;
		}
		auditionName = source;
	}
	else if (mode == "phones") {
		const phonex::CompileStatus status = phonex::compileDirectPhonemes(source, sequence);
		if (status != phonex::CompileStatus::Ok) {
			std::cerr << "Could not compile phones: " << phonex::compileStatusText(status) << '\n';
			return 1;
		}
		auditionName = source;
	}
	else if (mode == "probe") {
		if (!makeProbe(source, sequence)) {
			std::cerr << "Unknown probe: " << source << '\n';
			return 1;
		}
		auditionName = "PROBE " + source;
		if (source == "forced-voiced-unvoiced") {
			forced = phonex::ForcedExcitation::Voiced;
			exciteBlend = 1.f;
		}
	}
	else {
		if (phraseIndex < 0 || phraseIndex >= static_cast<int>(phonex::kBundledPhraseCount)) {
			std::cerr << "Phrase index must be 0..63\n";
			return 1;
		}
		if (phonex::compileBundledPhrase(static_cast<std::uint8_t>(phraseIndex), sequence)
			!= phonex::CompileStatus::Ok) {
			std::cerr << "Could not compile bundled phrase " << phraseIndex << '\n';
			return 1;
		}
		const phonex::StringView phraseName = phonex::bundledPhraseName(
			static_cast<std::uint8_t>(phraseIndex));
		auditionName.assign(phraseName.data(), phraseName.size());
	}

	const std::uint32_t rate = static_cast<std::uint32_t>(sampleRate);
	const std::uint32_t samplesPerFrame = std::max<std::uint32_t>(1u, rate / 50u);
	const std::uint32_t sampleCount = sequence.frameCount * samplesPerFrame + rate / 4u;
	std::ofstream out(path, std::ios::binary);
	if (!out) {
		std::cerr << "Could not open output: " << path << '\n';
		return 1;
	}
	out.write("RIFF", 4); writeU32(out, 36 + sampleCount * 2);
	out.write("WAVEfmt ", 8); writeU32(out, 16); writeU16(out, 1); writeU16(out, 1);
	writeU32(out, rate); writeU32(out, rate * 2); writeU16(out, 2); writeU16(out, 16);
	out.write("data", 4); writeU32(out, sampleCount * 2);
	phonex::Engine engine;
	engine.setSequence(&sequence);
	engine.setInternalRate(static_cast<float>(internalRate));
	engine.setReconstructionMode(reconstruction);
	engine.setReconstructionOrder(filterOrder);
	engine.setOutputStage(outputStage);
	engine.setSeed(static_cast<std::uint32_t>(seed));
	phonex::EngineControls controls;
	controls.hostSampleRate = static_cast<float>(rate);
	controls.exciteBlend = exciteBlend;
	controls.forcedExcitation = forced;
	for (std::uint32_t i = 0; i < sampleCount; ++i) {
		if (mode == "probe" && source == "reconstruction-sine") {
			controls.externalConnected = true;
			controls.externalExcitation = 2.5f * std::sin(
				2.f * 3.14159265358979323846f * probeFrequency
				* static_cast<float>(i) / static_cast<float>(rate));
		}
		const float voltage = engine.process(controls).audio;
		const float normalized = std::max(-1.f, std::min(1.f, voltage / 5.f));
		writeU16(out, static_cast<std::uint16_t>(
			static_cast<std::int16_t>(normalized * 32767.f)));
	}
	if (!out) return 1;
	std::uint32_t silenceFrames = 0;
	std::uint32_t unvoicedFrames = 0;
	std::uint32_t voicedFrames = 0;
	float maxAbsCoefficient = 0.f;
	for (std::uint16_t i = 0; i < sequence.frameCount; ++i) {
		const phonex::LpcFrame& frame = sequence.frames[i];
		if (frame.excitation == phonex::Excitation::Silence) ++silenceFrames;
		else if (frame.excitation == phonex::Excitation::Unvoiced) ++unvoicedFrames;
		else ++voicedFrames;
		for (float coefficient : frame.reflection)
			maxAbsCoefficient = std::max(maxAbsCoefficient, std::abs(coefficient));
	}
	std::cout << "PHONEX_RENDER\tname=" << auditionName
		<< "\tframes=" << sequence.frameCount
		<< "\tsilence_frames=" << silenceFrames
		<< "\tunvoiced_frames=" << unvoicedFrames
		<< "\tvoiced_frames=" << voicedFrames
		<< "\tmax_abs_k=" << maxAbsCoefficient
		<< "\tsample_rate=" << sampleRate
		<< "\tinternal_rate=" << internalRate
		<< "\treconstruction="
		<< (reconstruction == phonex::ReconstructionMode::Filtered ? "filtered" : "raw")
		<< "\toutput_stage="
		<< (outputStage == phonex::OutputStage::LegacyCurve ? "legacy"
			: outputStage == phonex::OutputStage::CalibratedLinear ? "linear" : "limited")
		<< "\tfilter_order=" << (2 * static_cast<int>(filterOrder))
		<< "\tforced_excitation="
		<< (forced == phonex::ForcedExcitation::Voiced ? "voiced" : "unvoiced")
		<< "\texcite_blend=" << exciteBlend
		<< "\tpath=" << path << '\n';
	return 0;
}

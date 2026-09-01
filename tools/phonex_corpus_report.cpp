#include "../src/PhonexRom.hpp"
#include "../src/PhonexSequenceCompiler.hpp"

#include <iomanip>
#include <iostream>

namespace {

const char* excitationName(phonex::Excitation excitation) {
	switch (excitation) {
		case phonex::Excitation::Silence: return "silence";
		case phonex::Excitation::Unvoiced: return "unvoiced";
		case phonex::Excitation::Voiced: return "voiced";
	}
	return "unknown";
}

} // namespace

int main() {
	std::cout << "phone\tanchor\tduration_frames\texcitation"
		<< "\tsource_energy\ttms_energy\tsource_pitch\ttms_pitch";
	for (int coefficient = 1; coefficient <= phonex::kLpcOrder; ++coefficient)
		std::cout << "\tsource_k" << coefficient << "\ttms_k" << coefficient;
	std::cout << '\n' << std::setprecision(9);
	for (std::size_t phoneIndex = 0; phoneIndex < phonex::kPhoneCount; ++phoneIndex) {
		const auto phone = static_cast<phonex::Phone>(phoneIndex);
		const phonex::PhonePrototype& prototype = phonex::phonePrototype(phone);
		const phonex::StringView symbol = phonex::phoneSymbol(phone);
		for (std::uint8_t anchor = 0; anchor < prototype.anchorCount; ++anchor) {
			const phonex::LpcFrame& source = prototype.anchors[anchor];
			const phonex::LpcFrame quantized = phonex::quantizeTms5100Frame(source);
			std::cout.write(symbol.data(), static_cast<std::streamsize>(symbol.size()));
			std::cout << '\t' << static_cast<unsigned>(anchor)
				<< '\t' << static_cast<unsigned>(prototype.durationFrames)
				<< '\t' << excitationName(source.excitation)
				<< '\t' << source.energy << '\t' << quantized.energy
				<< '\t' << source.pitchPeriod10k << '\t' << quantized.pitchPeriod10k;
			for (int coefficient = 0; coefficient < phonex::kLpcOrder; ++coefficient)
				std::cout << '\t' << source.reflection[coefficient]
					<< '\t' << quantized.reflection[coefficient];
			std::cout << '\n';
		}
	}
	return std::cout ? 0 : 1;
}

#include "PhonexRom.hpp"

#include <algorithm>

#include "PhonexRomData.inc"

namespace phonex {

const PhonePrototype& phonePrototype(Phone phone) {
	std::size_t index = static_cast<std::size_t>(phone);
	if (index >= kGeneratedPhonePrototypes.size())
		index = static_cast<std::size_t>(Phone::SIL);
	return kGeneratedPhonePrototypes[index];
}

StringView phoneSymbol(Phone phone) {
	const std::size_t index = static_cast<std::size_t>(phone);
	return index < kGeneratedPhoneSymbols.size() ? kGeneratedPhoneSymbols[index] : "SIL";
}

bool findPhone(StringView symbol, Phone& phone) {
	for (std::size_t i = 0; i < kGeneratedPhoneSymbols.size(); ++i) {
		if (symbol == kGeneratedPhoneSymbols[i]) {
			phone = static_cast<Phone>(i);
			return true;
		}
	}
	return false;
}

StringView bundledPhraseName(std::uint8_t index) {
	return index < kGeneratedPhraseNames.size() ? kGeneratedPhraseNames[index] : StringView{};
}

StringView bundledPhraseScript(std::uint8_t index) {
	return index < kGeneratedPhraseScripts.size() ? kGeneratedPhraseScripts[index] : StringView{};
}

StringView dictionaryPronunciation(StringView uppercaseWord) {
	for (std::size_t i = 0; i < kGeneratedDictionaryWords.size(); ++i) {
		if (uppercaseWord == kGeneratedDictionaryWords[i])
			return kGeneratedDictionaryScripts[i];
	}
	return {};
}

} // namespace phonex

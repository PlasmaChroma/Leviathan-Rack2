#pragma once

#include "PhonexTypes.hpp"

#include <array>
#include <cstdint>

namespace phonex {

constexpr std::size_t kMaxPhoneAnchors = 3;

struct PhonePrototype {
	std::uint8_t durationFrames = 0;
	std::uint8_t anchorCount = 0;
	std::array<LpcFrame, kMaxPhoneAnchors> anchors{};

	PhonePrototype() = default;
	PhonePrototype(std::uint8_t durationFrames, std::uint8_t anchorCount,
		const std::array<LpcFrame, kMaxPhoneAnchors>& anchors)
		: durationFrames(durationFrames), anchorCount(anchorCount), anchors(anchors) {}
};

const PhonePrototype& phonePrototype(Phone phone);
StringView phoneSymbol(Phone phone);
bool findPhone(StringView symbol, Phone& phone);

constexpr std::size_t kBundledPhraseCount = 64;
StringView bundledPhraseName(std::uint8_t index);
StringView bundledPhraseScript(std::uint8_t index);
StringView dictionaryPronunciation(StringView uppercaseWord);

} // namespace phonex

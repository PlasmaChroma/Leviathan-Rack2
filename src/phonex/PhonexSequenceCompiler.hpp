#pragma once

#include "PhonexRom.hpp"

#include <array>
#include <cstdint>

namespace phonex {

constexpr std::size_t kMaxPhoneTokens = 512;

enum class CompileStatus : std::uint8_t {
	Ok = 0,
	BadPhone,
	TooLong,
	TextTooLong,
	Empty,
};

struct PhoneToken {
	Phone phone = Phone::SIL;
	std::uint8_t stress = 0;
	PhoneToken() = default;
	PhoneToken(Phone phone, std::uint8_t stress) : phone(phone), stress(stress) {}
};

struct PhoneScript {
	std::array<PhoneToken, kMaxPhoneTokens> tokens{};
	std::uint16_t count = 0;
};

CompileStatus parseDirectPhonemes(StringView source, PhoneScript& output);
LpcFrame quantizeTms5100Frame(const LpcFrame& frame);
CompileStatus compilePhoneScript(const PhoneScript& script, LpcSequence& output);
CompileStatus compileDirectPhonemes(StringView source, LpcSequence& output);
CompileStatus compileBundledPhrase(std::uint8_t index, LpcSequence& output);
const char* compileStatusText(CompileStatus status);

} // namespace phonex

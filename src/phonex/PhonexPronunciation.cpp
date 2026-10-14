#include "PhonexPronunciation.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>

namespace phonex {
namespace {

bool appendPhone(PhoneScript& output, Phone phone, std::uint8_t stress = 0) {
	if (output.count >= output.tokens.size())
		return false;
	output.tokens[output.count++] = {phone, stress};
	return true;
}

bool appendScript(PhoneScript& output, StringView script) {
	PhoneScript parsed;
	if (parseDirectPhonemes(script, parsed) != CompileStatus::Ok
		|| output.count + parsed.count > output.tokens.size())
		return false;
	for (std::uint16_t i = 0; i < parsed.count; ++i)
		output.tokens[output.count++] = parsed.tokens[i];
	return true;
}

bool appendDictionary(PhoneScript& output, StringView word) {
	const StringView script = dictionaryPronunciation(word);
	return !script.empty() && appendScript(output, script);
}

bool appendSpelling(PhoneScript& output, StringView word) {
	bool emitted = false;
	for (char character : word) {
		if (character < 'A' || character > 'Z')
			continue;
		if (emitted && !appendPhone(output, Phone::SIL))
			return false;
		const char letter[] = {character, '\0'};
		if (!appendDictionary(output, letter))
			return false;
		emitted = true;
	}
	return emitted;
}

bool appendG2p(PhoneScript& output, StringView word) {
	if (word.empty() || word.size() > 32
		|| word.find('\'') != StringView::npos)
		return appendSpelling(output, word);
	std::size_t ending = word.size();
	enum class Suffix { None, S, Ed } suffix = Suffix::None;
	if (ending > 2 && word[ending - 1] == 'S') {
		suffix = Suffix::S;
		--ending;
	}
	else if (ending > 3 && word.substr(ending - 2) == "ED") {
		suffix = Suffix::Ed;
		ending -= 2;
	}
	auto pair = [&](std::size_t index, const char* text) {
		return index + 1 < ending && word.substr(index, 2) == text;
	};
	for (std::size_t i = 0; i < ending;) {
		Phone first = Phone::SIL;
		Phone second = Phone::SIL;
		bool hasSecond = false;
		if (pair(i, "CH")) first = Phone::CH;
		else if (pair(i, "SH")) first = Phone::SH;
		else if (pair(i, "TH")) first = Phone::TH;
		else if (pair(i, "PH")) first = Phone::F;
		else if (pair(i, "NG")) first = Phone::NG;
		else if (pair(i, "QU")) { first = Phone::K; second = Phone::W; hasSecond = true; }
		else if (pair(i, "CK")) first = Phone::K;
		else if (pair(i, "WH")) first = Phone::W;
		else if (pair(i, "EE") || pair(i, "EA")) first = Phone::IY;
		else if (pair(i, "AI") || pair(i, "AY")) first = Phone::EY;
		else if (pair(i, "OA")) first = Phone::OW;
		else if (pair(i, "OI") || pair(i, "OY")) first = Phone::OY;
		else if (pair(i, "OW") || pair(i, "OU")) first = Phone::AW;
		else if (pair(i, "ER") || pair(i, "IR") || pair(i, "UR")) first = Phone::ER;
		else {
			const char c = word[i];
			const char next = i + 1 < ending ? word[i + 1] : '\0';
			switch (c) {
				case 'A': first = Phone::AE; break;
				case 'B': first = Phone::B; break;
				case 'C': first = (next == 'E' || next == 'I' || next == 'Y') ? Phone::S : Phone::K; break;
				case 'D': first = Phone::D; break;
				case 'E': first = Phone::EH; break;
				case 'F': first = Phone::F; break;
				case 'G': first = (next == 'E' || next == 'I' || next == 'Y') ? Phone::JH : Phone::G; break;
				case 'H': first = Phone::HH; break;
				case 'I': first = Phone::IH; break;
				case 'J': first = Phone::JH; break;
				case 'K': first = Phone::K; break;
				case 'L': first = Phone::L; break;
				case 'M': first = Phone::M; break;
				case 'N': first = Phone::N; break;
				case 'O': first = Phone::AA; break;
				case 'P': first = Phone::P; break;
				case 'Q': first = Phone::K; second = Phone::W; hasSecond = true; break;
				case 'R': first = Phone::R; break;
				case 'S': first = Phone::S; break;
				case 'T': first = Phone::T; break;
				case 'U': first = Phone::AH; break;
				case 'V': first = Phone::V; break;
				case 'W': first = Phone::W; break;
				case 'X': first = Phone::K; second = Phone::S; hasSecond = true; break;
				case 'Y': first = Phone::Y; break;
				case 'Z': first = Phone::Z; break;
				default: return appendSpelling(output, word);
			}
			if (!appendPhone(output, first) || (hasSecond && !appendPhone(output, second)))
				return false;
			++i;
			continue;
		}
		if (!appendPhone(output, first) || (hasSecond && !appendPhone(output, second)))
			return false;
		i += 2;
	}
	if (suffix == Suffix::S) {
		const char previous = word[ending - 1];
		return appendPhone(output, previous == 'P' || previous == 'T' || previous == 'K'
			|| previous == 'F' ? Phone::S : Phone::Z);
	}
	if (suffix == Suffix::Ed) {
		const char previous = word[ending - 1];
		return appendPhone(output, previous == 'P' || previous == 'K'
			|| previous == 'F' || previous == 'S' ? Phone::T : Phone::D);
	}
	return true;
}

bool appendWord(PhoneScript& output, StringView word) {
	const StringView dictionary = dictionaryPronunciation(word);
	return dictionary.empty() ? appendG2p(output, word) : appendScript(output, dictionary);
}

bool appendSeparatedWord(PhoneScript& output, StringView word, bool& hasSpeech) {
	if (hasSpeech && !appendPhone(output, Phone::SIL))
		return false;
	if (!appendWord(output, word))
		return false;
	hasSpeech = true;
	return true;
}

bool appendSmallNumber(PhoneScript& output, unsigned value, bool& hasSpeech) {
	const std::uint16_t startCount = output.count;
	static const std::array<StringView, 20> small {{
		"ZERO", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE",
		"TEN", "ELEVEN", "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN",
		"SEVENTEEN", "EIGHTEEN", "NINETEEN"}};
	static const std::array<StringView, 10> tens {{
		"", "", "TWENTY", "THIRTY", "FORTY", "FIFTY", "SIXTY", "SEVENTY", "EIGHTY", "NINETY"}};
	if (value >= 1000) {
		if (!appendSeparatedWord(output, small[value / 1000], hasSpeech)
			|| !appendSeparatedWord(output, "THOUSAND", hasSpeech)) return false;
		value %= 1000;
	}
	if (value >= 100) {
		if (!appendSeparatedWord(output, small[value / 100], hasSpeech)
			|| !appendSeparatedWord(output, "HUNDRED", hasSpeech)) return false;
		value %= 100;
	}
	if (value >= 20) {
		if (!appendSeparatedWord(output, tens[value / 10], hasSpeech)) return false;
		value %= 10;
	}
	if (value > 0 || output.count == startCount)
		return appendSeparatedWord(output, small[value], hasSpeech);
	return true;
}

bool appendNumber(PhoneScript& output, StringView token, bool& hasSpeech) {
	std::size_t cursor = 0;
	if (token[cursor] == '-') {
		if (!appendSeparatedWord(output, "MINUS", hasSpeech)) return false;
		++cursor;
	}
	const std::size_t dot = token.find('.', cursor);
	const StringView integer = token.substr(cursor, dot == StringView::npos
		? token.size() - cursor : dot - cursor);
	unsigned value = 0;
	bool ordinary = !integer.empty() && integer.size() <= 4;
	for (char c : integer) {
		if (c < '0' || c > '9') return false;
		value = value * 10u + static_cast<unsigned>(c - '0');
	}
	if (ordinary && value <= 9999) {
		if (!appendSmallNumber(output, value, hasSpeech)) return false;
	}
	else {
		static const std::array<StringView, 10> digits {{
			"ZERO", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE"}};
		for (char c : integer)
			if (!appendSeparatedWord(output, digits[c - '0'], hasSpeech)) return false;
	}
	if (dot != StringView::npos) {
		if (!appendSeparatedWord(output, "POINT", hasSpeech)) return false;
		static const std::array<StringView, 10> digits {{
			"ZERO", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE"}};
		for (std::size_t i = dot + 1; i < token.size(); ++i) {
			if (token[i] < '0' || token[i] > '9'
				|| !appendSeparatedWord(output, digits[token[i] - '0'], hasSpeech)) return false;
		}
	}
	return true;
}

void applyFinalContour(LpcSequence& sequence, bool question, bool sentenceEnding) {
	if (!question && !sentenceEnding) return;
	std::array<std::uint16_t, 12> voiced{};
	std::size_t count = 0;
	for (std::uint16_t i = sequence.frameCount; i > 0 && count < voiced.size();) {
		--i;
		if (sequence.frames[i].excitation == Excitation::Voiced)
			voiced[count++] = i;
	}
	for (std::size_t rank = 0; rank < count; ++rank) {
		const float amount = static_cast<float>(count - rank) / static_cast<float>(count);
		sequence.frames[voiced[rank]].pitchPeriod10k *= question
			? (1.f - 0.08f * amount) : (1.f + 0.05f * amount);
	}
}

} // namespace

TextCompileResult compileText(StringView source, LpcSequence& output) {
	TextCompileResult result;
	if (source.size() > kMaxSubmittedTextBytes) {
		result.status = CompileStatus::TextTooLong;
		return result;
	}
	PhoneScript script;
	bool hasSpeech = false;
	bool question = false;
	bool sentenceEnding = false;
	for (std::size_t cursor = 0; cursor < source.size();) {
		const unsigned char raw = static_cast<unsigned char>(source[cursor]);
		if (raw >= 128) {
			result.unsupportedUnicode = true;
			++cursor;
			continue;
		}
		if (std::isspace(raw)) {
			++cursor;
			continue;
		}
		if (source[cursor] == '[') {
			const std::size_t close = source.find(']', cursor + 1);
			if (close == StringView::npos) {
				result.status = CompileStatus::BadPhone;
				return result;
			}
			PhoneScript direct;
			if (parseDirectPhonemes(source.substr(cursor, close - cursor + 1), direct)
				!= CompileStatus::Ok || (hasSpeech && !appendPhone(script, Phone::SIL))
				|| script.count + direct.count > script.tokens.size()) {
				result.status = CompileStatus::BadPhone;
				return result;
			}
			for (std::uint16_t i = 0; i < direct.count; ++i)
				script.tokens[script.count++] = direct.tokens[i];
			hasSpeech = true;
			cursor = close + 1;
			continue;
		}
		if (std::isdigit(raw) || (source[cursor] == '-' && cursor + 1 < source.size()
			&& std::isdigit(static_cast<unsigned char>(source[cursor + 1])))) {
			const std::size_t begin = cursor++;
			while (cursor < source.size() && (std::isdigit(static_cast<unsigned char>(source[cursor]))
				|| source[cursor] == '.')) ++cursor;
			if (!appendNumber(script, source.substr(begin, cursor - begin), hasSpeech)) {
				result.status = CompileStatus::TextTooLong;
				return result;
			}
			continue;
		}
		if (source[cursor] == '-') {
			++cursor;
			continue;
		}
		if (std::isalpha(raw) || source[cursor] == '\'') {
			std::array<char, kMaxSubmittedTextBytes + 1> word{};
			std::size_t length = 0;
			while (cursor < source.size()) {
				const unsigned char c = static_cast<unsigned char>(source[cursor]);
				if (!std::isalpha(c) && source[cursor] != '\'') break;
				word[length++] = static_cast<char>(std::toupper(c));
				++cursor;
			}
			if (!appendSeparatedWord(script, StringView(word.data(), length), hasSpeech)) {
				result.status = CompileStatus::TextTooLong;
				return result;
			}
			continue;
		}
		int silenceFrames = 0;
		if (source[cursor] == ',' || source[cursor] == ';') silenceFrames = 6;
		else if (source[cursor] == '.' || source[cursor] == '!' || source[cursor] == '?') {
			silenceFrames = 12;
			sentenceEnding = true;
			question = source[cursor] == '?';
		}
		for (int i = 0; i < silenceFrames / 2; ++i) {
			if (!appendPhone(script, Phone::SIL)) {
				result.status = CompileStatus::TextTooLong;
				return result;
			}
		}
		++cursor;
	}
	if (!hasSpeech) {
		result.status = CompileStatus::Empty;
		return result;
	}
	result.status = compilePhoneScript(script, output);
	if (result.status == CompileStatus::TooLong)
		result.status = CompileStatus::TextTooLong;
	if (result.status == CompileStatus::Ok) {
		applyFinalContour(output, question, sentenceEnding);
		for (std::uint16_t i = 0; i < output.frameCount; ++i)
			output.frames[i] = quantizeTms5100Frame(output.frames[i]);
	}
	return result;
}

} // namespace phonex

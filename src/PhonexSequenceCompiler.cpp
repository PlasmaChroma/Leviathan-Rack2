#include "PhonexSequenceCompiler.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>

namespace phonex {
namespace {

// TMS5100 parameter reconstruction tables. These are functional chip
// configuration values, not vocabulary or speech-ROM data. They are included
// here as independently represented decoder parameters, never speech content.
constexpr std::array<int, 15> kTmsEnergy {{
	0, 0, 1, 1, 2, 3, 5, 7, 10, 15, 21, 30, 43, 61, 86}};
constexpr std::array<int, 31> kTmsPitch {{
	41, 43, 45, 47, 49, 51, 53, 55, 58, 60, 63, 66, 70, 73, 76, 79,
	83, 87, 90, 94, 99, 103, 107, 112, 118, 123, 129, 134, 140, 147, 153}};
constexpr std::array<int, 32> kTmsK0 {{
	-501, -497, -493, -488, -480, -471, -460, -446, -427, -405, -378,
	-344, -305, -259, -206, -148, -86, -21, 45, 110, 171, 227, 277,
	320, 357, 388, 413, 434, 451, 464, 474, 498}};
constexpr std::array<int, 32> kTmsK1 {{
	-349, -328, -305, -280, -252, -223, -192, -158, -124, -88, -51,
	-14, 23, 60, 97, 133, 167, 199, 230, 259, 286, 310, 333, 354,
	372, 389, 404, 417, 429, 439, 449, 506}};
constexpr std::array<int, 16> kTmsK2 {{
	-397, -365, -327, -282, -229, -170, -104, -36,
	35, 104, 169, 228, 281, 326, 364, 396}};
constexpr std::array<int, 16> kTmsK3 {{
	-369, -334, -293, -245, -191, -131, -67, -1,
	64, 128, 188, 243, 291, 332, 367, 397}};
constexpr std::array<int, 16> kTmsK4 {{
	-319, -286, -250, -211, -168, -122, -74, -25,
	24, 73, 121, 167, 210, 249, 285, 318}};
constexpr std::array<int, 16> kTmsK5 {{
	-290, -252, -209, -163, -114, -62, -9, 44,
	97, 147, 194, 238, 278, 313, 344, 371}};
constexpr std::array<int, 16> kTmsK6 {{
	-291, -256, -216, -174, -128, -80, -31, 19,
	69, 117, 163, 206, 246, 283, 316, 345}};
constexpr std::array<int, 8> kTmsK7 {{
	-218, -133, -38, 59, 152, 235, 305, 361}};
constexpr std::array<int, 8> kTmsK8 {{
	-226, -157, -82, -3, 76, 151, 220, 280}};
constexpr std::array<int, 8> kTmsK9 {{
	-179, -122, -61, 1, 62, 123, 179, 231}};

template <std::size_t N>
int nearestTableValue(float target, const std::array<int, N>& table) {
	int best = table[0];
	float bestDistance = std::abs(target - static_cast<float>(best));
	for (std::size_t i = 1; i < table.size(); ++i) {
		const float distance = std::abs(target - static_cast<float>(table[i]));
		if (distance < bestDistance) {
			best = table[i];
			bestDistance = distance;
		}
	}
	return best;
}

bool isVowel(Phone phone) {
	return phone == Phone::AA || phone == Phone::AE || phone == Phone::AH
		|| phone == Phone::AO || phone == Phone::AW || phone == Phone::AY
		|| phone == Phone::EH || phone == Phone::ER || phone == Phone::EY
		|| phone == Phone::IH || phone == Phone::IY || phone == Phone::OW
		|| phone == Phone::OY || phone == Phone::UH || phone == Phone::UW;
}

bool beginsVoiced(const PhonePrototype& prototype) {
	return prototype.anchorCount > 0
		&& prototype.anchors[0].excitation == Excitation::Voiced;
}

void shapeUnvoicedTowardVowel(LpcFrame& frame, const LpcFrame& vowel,
	float amount, float energyScale) {
	if (frame.excitation != Excitation::Unvoiced
		|| vowel.excitation != Excitation::Voiced)
		return;
	for (int coefficient = 0; coefficient < 4; ++coefficient) {
		frame.reflection[coefficient] += amount
			* (vowel.reflection[coefficient] - frame.reflection[coefficient]);
	}
	frame.energy *= energyScale;
}

LpcFrame interpolate(const LpcFrame& a, const LpcFrame& b, float amount) {
	if (a.excitation != b.excitation)
		return amount < 0.5f ? a : b;
	LpcFrame frame;
	frame.energy = a.energy + (b.energy - a.energy) * amount;
	frame.pitchPeriod10k = a.pitchPeriod10k
		+ (b.pitchPeriod10k - a.pitchPeriod10k) * amount;
	for (int i = 0; i < kLpcOrder; ++i)
		frame.reflection[i] = a.reflection[i]
			+ (b.reflection[i] - a.reflection[i]) * amount;
	frame.excitation = a.excitation;
	return frame;
}

bool appendFrame(LpcSequence& output, const LpcFrame& frame) {
	if (output.frameCount >= output.frames.size())
		return false;
	output.frames[output.frameCount++] = quantizeTms5100Frame(frame);
	return true;
}

} // namespace

LpcFrame quantizeTms5100Frame(const LpcFrame& source) {
	if (source.excitation == Excitation::Silence)
		return {};
	LpcFrame frame = source;
	frame.energy = static_cast<float>(nearestTableValue(
		std::max(0.f, std::min(1.f, source.energy)) * 86.f, kTmsEnergy)) / 86.f;
	if (source.excitation == Excitation::Voiced) {
		frame.pitchPeriod10k = static_cast<float>(nearestTableValue(
			std::max(1.f, source.pitchPeriod10k), kTmsPitch));
	}
	else {
		frame.pitchPeriod10k = 0.f;
	}
	const std::array<const int*, kLpcOrder> tables {{
		kTmsK0.data(), kTmsK1.data(), kTmsK2.data(), kTmsK3.data(),
		kTmsK4.data(), kTmsK5.data(), kTmsK6.data(), kTmsK7.data(),
		kTmsK8.data(), kTmsK9.data(),
	}};
	const std::array<std::size_t, kLpcOrder> sizes {{
		32, 32, 16, 16, 16, 16, 16, 8, 8, 8,
	}};
	const int activeCoefficients = source.excitation == Excitation::Unvoiced ? 4 : 10;
	for (int coefficient = 0; coefficient < kLpcOrder; ++coefficient) {
		if (coefficient >= activeCoefficients) {
			frame.reflection[coefficient] = 0.f;
			continue;
		}
		const float target = source.reflection[coefficient] * 512.f;
		int best = tables[coefficient][0];
		float bestDistance = std::abs(target - static_cast<float>(best));
		for (std::size_t i = 1; i < sizes[coefficient]; ++i) {
			const int candidate = tables[coefficient][i];
			const float distance = std::abs(target - static_cast<float>(candidate));
			if (distance < bestDistance) {
				best = candidate;
				bestDistance = distance;
			}
		}
		frame.reflection[coefficient] = static_cast<float>(best) / 512.f;
	}
	return frame;
}

CompileStatus parseDirectPhonemes(StringView source, PhoneScript& output) {
	output = {};
	std::size_t begin = 0;
	std::size_t end = source.size();
	while (begin < end && std::isspace(static_cast<unsigned char>(source[begin]))) ++begin;
	while (end > begin && std::isspace(static_cast<unsigned char>(source[end - 1]))) --end;
	if (begin == end)
		return CompileStatus::Empty;
	if (source[begin] == '[') {
		if (end - begin < 2 || source[end - 1] != ']')
			return CompileStatus::BadPhone;
		++begin;
		--end;
	}
	for (std::size_t cursor = begin; cursor < end;) {
		while (cursor < end && std::isspace(static_cast<unsigned char>(source[cursor]))) ++cursor;
		if (cursor == end) break;
		const std::size_t tokenBegin = cursor;
		while (cursor < end && !std::isspace(static_cast<unsigned char>(source[cursor]))) ++cursor;
		StringView token = source.substr(tokenBegin, cursor - tokenBegin);
		std::uint8_t stress = 0;
		if (!token.empty() && token.back() >= '0' && token.back() <= '2') {
			stress = static_cast<std::uint8_t>(token.back() - '0');
			token.remove_suffix(1);
		}
		Phone phone;
		if (!findPhone(token, phone) || (stress > 0 && !isVowel(phone))) {
			output = {};
			return CompileStatus::BadPhone;
		}
		if (output.count >= output.tokens.size()) {
			output = {};
			return CompileStatus::TooLong;
		}
		output.tokens[output.count++] = {phone, stress};
	}
	return output.count > 0 ? CompileStatus::Ok : CompileStatus::Empty;
}

CompileStatus compilePhoneScript(const PhoneScript& script, LpcSequence& output) {
	output = {};
	if (script.count == 0)
		return CompileStatus::Empty;
	for (std::uint16_t tokenIndex = 0; tokenIndex < script.count; ++tokenIndex) {
		const PhoneToken token = script.tokens[tokenIndex];
		const PhonePrototype& prototype = phonePrototype(token.phone);
		const PhonePrototype* nextPrototype = tokenIndex + 1 < script.count
			? &phonePrototype(script.tokens[tokenIndex + 1].phone) : nullptr;
		if (prototype.durationFrames == 0 || prototype.anchorCount == 0)
			return CompileStatus::BadPhone;
		if (output.frameCount > 0 && token.phone != Phone::SIL) {
			const LpcFrame& previous = output.frames[output.frameCount - 1];
			const LpcFrame& next = prototype.anchors[0];
			if (previous.excitation == next.excitation && previous.excitation != Excitation::Silence) {
				if (!appendFrame(output, interpolate(previous, next, 0.5f)))
					return CompileStatus::TooLong;
			}
		}
		for (std::uint8_t frameIndex = 0; frameIndex < prototype.durationFrames; ++frameIndex) {
			float anchorPosition = 0.f;
			if (prototype.durationFrames > 1 && prototype.anchorCount > 1) {
				anchorPosition = static_cast<float>(frameIndex)
					* static_cast<float>(prototype.anchorCount - 1)
					/ static_cast<float>(prototype.durationFrames - 1);
			}
			const std::uint8_t a = std::min<std::uint8_t>(
				static_cast<std::uint8_t>(anchorPosition), prototype.anchorCount - 1);
			const std::uint8_t b = std::min<std::uint8_t>(a + 1, prototype.anchorCount - 1);
			LpcFrame frame = interpolate(prototype.anchors[a], prototype.anchors[b],
				anchorPosition - static_cast<float>(a));
			if (nextPrototype && beginsVoiced(*nextPrototype)) {
				const LpcFrame& nextVowel = nextPrototype->anchors[0];
				if (token.phone == Phone::HH) {
					// /h/ is turbulent airflow through the following vowel's tract.
					// Carrying that tract context in K1..K4 is far clearer than a
					// single generic aspiration spectrum.
					shapeUnvoicedTowardVowel(frame, nextVowel, 0.68f, 1.f);
				}
				else if (frameIndex + 1 == prototype.durationFrames) {
					// Use only the final 20 ms of an unvoiced consonant as a quiet
					// coarticulation frame. The consonant body remains untouched.
					shapeUnvoicedTowardVowel(frame, nextVowel, 0.58f, 0.72f);
				}
			}
			if (token.stress == 1) {
				frame.energy = std::min(1.f, frame.energy * 1.10f);
				if (frame.excitation == Excitation::Voiced)
					frame.pitchPeriod10k *= 0.9438743f;
			}
			else if (token.stress == 2) {
				frame.energy = std::min(1.f, frame.energy * 1.05f);
			}
			if (!appendFrame(output, frame))
				return CompileStatus::TooLong;
		}
	}
	return CompileStatus::Ok;
}

CompileStatus compileDirectPhonemes(StringView source, LpcSequence& output) {
	PhoneScript script;
	const CompileStatus status = parseDirectPhonemes(source, script);
	return status == CompileStatus::Ok ? compilePhoneScript(script, output) : status;
}

CompileStatus compileBundledPhrase(std::uint8_t index, LpcSequence& output) {
	if (index >= kBundledPhraseCount) {
		output = {};
		return CompileStatus::BadPhone;
	}
	const CompileStatus status = compileDirectPhonemes(bundledPhraseScript(index), output);
	if (status == CompileStatus::Ok)
		output.phraseId = index;
	return status;
}

const char* compileStatusText(CompileStatus status) {
	switch (status) {
		case CompileStatus::Ok: return "OK";
		case CompileStatus::BadPhone: return "BAD PHONE";
		case CompileStatus::TooLong: return "TOO LONG";
		case CompileStatus::TextTooLong: return "TEXT TOO LONG";
		case CompileStatus::Empty: return "EMPTY";
	}
	return "BAD PHONE";
}

} // namespace phonex

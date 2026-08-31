#pragma once

#include <array>
#include <cstdint>

namespace phonex {

constexpr int kLpcOrder = 10;
constexpr int kMaxFrames = 2048;
constexpr float kSourceFrameSeconds = 0.020f;
constexpr std::uint16_t kNoPhrase = 0xffffu;

enum class Excitation : std::uint8_t {
	Silence = 0,
	Unvoiced,
	Voiced,
};

struct LpcFrame {
	float energy = 0.f;
	float pitchPeriod10k = 0.f;
	std::array<float, kLpcOrder> reflection{};
	Excitation excitation = Excitation::Silence;
};

struct LpcSequence {
	std::array<LpcFrame, kMaxFrames> frames{};
	std::uint16_t frameCount = 0;
	std::uint16_t phraseId = kNoPhrase;
	std::uint32_t generation = 0;

	bool valid() const {
		return frameCount <= frames.size();
	}

	const LpcFrame* frame(std::uint16_t index) const {
		return valid() && index < frameCount ? &frames[index] : nullptr;
	}
};

} // namespace phonex

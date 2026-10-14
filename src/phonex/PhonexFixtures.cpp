#include "PhonexFixtures.hpp"

#include <algorithm>

namespace phonex {
namespace {

std::uint16_t safeCount(std::uint16_t requested) {
	return std::min<std::uint16_t>(requested, kMaxFrames);
}

} // namespace

LpcSequence makeSilenceFixture(std::uint16_t requested) {
	LpcSequence sequence;
	sequence.frameCount = safeCount(requested);
	return sequence;
}

LpcSequence makeVoicedFixture(std::uint16_t requested) {
	LpcSequence sequence;
	sequence.frameCount = safeCount(requested);
	for (std::uint16_t i = 0; i < sequence.frameCount; ++i) {
		auto& frame = sequence.frames[i];
		frame.energy = 0.55f;
		frame.pitchPeriod10k = 80.f;
		frame.reflection = {{0.72f, -0.54f, 0.41f, -0.30f, 0.22f,
			-0.16f, 0.11f, -0.08f, 0.05f, -0.03f}};
		frame.excitation = Excitation::Voiced;
	}
	return sequence;
}

LpcSequence makeUnvoicedFixture(std::uint16_t requested) {
	LpcSequence sequence;
	sequence.frameCount = safeCount(requested);
	for (std::uint16_t i = 0; i < sequence.frameCount; ++i) {
		auto& frame = sequence.frames[i];
		frame.energy = 0.28f;
		frame.reflection = {{-0.31f, 0.48f, -0.38f, 0.25f, -0.17f,
			0.12f, -0.08f, 0.05f, -0.03f, 0.02f}};
		frame.excitation = Excitation::Unvoiced;
	}
	return sequence;
}

} // namespace phonex

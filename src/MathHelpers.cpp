#include "MathHelpers.hpp"

namespace levi_math {
namespace {

std::array<float, detail::AUDIO_TANH_LUT_INTERVALS + 1> makeAudioTanhLut() {
	std::array<float, detail::AUDIO_TANH_LUT_INTERVALS + 1> table {};
	for (int i = 0; i <= detail::AUDIO_TANH_LUT_INTERVALS; ++i) {
		table[i] = std::tanh(float(i) / detail::AUDIO_TANH_LUT_SCALE);
	}
	return table;
}

std::array<float, detail::AUDIO_SINE_LUT_INTERVALS + 1> makeAudioSineLut() {
	std::array<float, detail::AUDIO_SINE_LUT_INTERVALS + 1> table {};
	constexpr float kTwoPi = 6.28318530717958647692f;
	for (int i = 0; i < detail::AUDIO_SINE_LUT_INTERVALS; ++i) {
		table[i] = std::sin(
			kTwoPi * float(i) / float(detail::AUDIO_SINE_LUT_INTERVALS));
	}
	table[detail::AUDIO_SINE_LUT_INTERVALS] = table[0];
	return table;
}

} // namespace

namespace detail {

alignas(64) const std::array<float, AUDIO_TANH_LUT_INTERVALS + 1>
	audioTanhLut = makeAudioTanhLut();

alignas(64) const std::array<float, AUDIO_SINE_LUT_INTERVALS + 1>
	audioSineLut = makeAudioSineLut();

} // namespace detail
} // namespace levi_math

#include "FastMath.hpp"

namespace levi_math {
namespace {

std::array<float, detail::AUDIO_TANH_LUT_INTERVALS + 1> makeAudioTanhLut() {
	std::array<float, detail::AUDIO_TANH_LUT_INTERVALS + 1> table {};
	for (int i = 0; i <= detail::AUDIO_TANH_LUT_INTERVALS; ++i) {
		table[i] = std::tanh(float(i) / detail::AUDIO_TANH_LUT_SCALE);
	}
	return table;
}

} // namespace

namespace detail {

alignas(64) const std::array<float, AUDIO_TANH_LUT_INTERVALS + 1>
	audioTanhLut = makeAudioTanhLut();

} // namespace detail
} // namespace levi_math

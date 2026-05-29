#pragma once

#include "plugin.hpp"

namespace chronomaw {

inline float quantizeChromatic1VOct(float volts) {
	return std::round(volts * 12.f) / 12.f;
}

} // namespace chronomaw


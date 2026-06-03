#pragma once

#include "plugin.hpp"

struct GearKnobInvertDaviesSized : app::SvgKnob {
	GearKnobInvertDaviesSized() {
		minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;

		setSvg(Svg::load(asset::plugin(pluginInstance, "res/icon/gear_knob_invert.svg")));
	}
};

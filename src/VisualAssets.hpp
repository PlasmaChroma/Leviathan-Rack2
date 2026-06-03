#pragma once

#include "plugin.hpp"

struct GearKnobInvertSized : app::SvgKnob {
	GearKnobInvertSized() {
		minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;

		setSvg(Svg::load(asset::plugin(pluginInstance, "res/icon/gear_knob_invert.svg")));
		box.size = Vec(46.f, 46.f);
		if (shadow) {
			shadow->opacity = 0.3f;
			shadow->blurRadius = 10.f;
			shadow->box.pos = Vec(3.f, 3.f);
			shadow->box.size = Vec(40.f, 40.f);
		}
	}
};

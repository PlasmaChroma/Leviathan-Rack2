#pragma once

#include "plugin.hpp"

namespace visual_assets {

std::shared_ptr<window::Svg> loadPluginSvgCached(const char* path);

} // namespace visual_assets

struct GearKnobInvertSized : app::SvgKnob {
	struct ActiveRingWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;

		void draw(const DrawArgs& args) override;
	};

	ActiveRingWidget* activeRing = nullptr;

	GearKnobInvertSized();
	void draw(const DrawArgs& args) override;
	void onChange(const ChangeEvent& e) override;

	float normalizedParamValue();
};

struct ClockworkGearKnob : GearKnobInvertSized {
	struct CogwheelWidget : TransparentWidget {
		std::shared_ptr<window::Svg> svg;
		Vec center;
		float diameterPx = 1.f;
		float angleRad = 0.f;

		void draw(const DrawArgs& args) override;
	};

	CogwheelWidget* primaryCogwheel = nullptr;
	CogwheelWidget* secondaryCogwheel = nullptr;

	ClockworkGearKnob();
	void draw(const DrawArgs& args) override;
	void onChange(const ChangeEvent& e) override;

	void updateCogwheelGeometry();
};

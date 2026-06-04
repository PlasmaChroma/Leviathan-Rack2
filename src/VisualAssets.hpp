#pragma once

#include "plugin.hpp"

struct GearKnobInvertSized : app::SvgKnob {
	GearKnobInvertSized() {
		minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;

		setSvg(Svg::load(asset::plugin(pluginInstance, "res/icon/gear_knob_invert.svg")));
		box.size = Vec(46.f, 46.f);
		if (shadow) {
			shadow->opacity = 0.f;
		}
	}
};

struct CogwheelBackedGearKnobInvertSized : GearKnobInvertSized {
	std::shared_ptr<window::Svg> cogwheelSvg;

	CogwheelBackedGearKnobInvertSized() {
		try {
			cogwheelSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/cogwheel_small.svg"));
		}
		catch (const std::exception& e) {
			WARN("Failed to load cogwheel-backed gear knob SVG: %s", e.what());
			cogwheelSvg.reset();
		}
	}

	void draw(const DrawArgs& args) override {
		drawCogwheel(args);
		GearKnobInvertSized::draw(args);
	}

	void drawCogwheel(const DrawArgs& args) {
		if (!cogwheelSvg) return;
		const Vec svgSize = cogwheelSvg->getSize();
		if (svgSize.x <= 1.f || svgSize.y <= 1.f) return;

		const float cogwheelDiameterPx = 17.f;
		const float scale = cogwheelDiameterPx / std::max(svgSize.x, svgSize.y);
		const float valueNorm = normalizedParamValue();
		const float knobAngle = crossfade(minAngle, maxAngle, valueNorm);
		const Vec cogwheelPos = box.size.mult(0.5f);
		const Vec cogwheelCenter = cogwheelPos.plus(Vec(0.5f * cogwheelDiameterPx, 0.5f * cogwheelDiameterPx));

		nvgSave(args.vg);
		nvgTranslate(args.vg, cogwheelCenter.x, cogwheelCenter.y);
		nvgRotate(args.vg, -knobAngle);
		nvgScale(args.vg, scale, scale);
		nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
		cogwheelSvg->draw(args.vg);
		nvgRestore(args.vg);
	}

	float normalizedParamValue() {
		engine::ParamQuantity* pq = getParamQuantity();
		if (!pq) return 0.5f;
		const float minValue = pq->getMinValue();
		const float maxValue = pq->getMaxValue();
		const float range = maxValue - minValue;
		if (range <= 1e-6f) return 0.5f;
		return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
	}
};

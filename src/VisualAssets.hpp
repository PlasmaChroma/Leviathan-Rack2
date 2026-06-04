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
	std::shared_ptr<window::Svg> secondaryCogwheelSvg;

	CogwheelBackedGearKnobInvertSized() {
		try {
			cogwheelSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/cogwheel_amythyst.svg"));
		}
		catch (const std::exception& e) {
			WARN("Failed to load cogwheel-backed gear knob SVG: %s", e.what());
			cogwheelSvg.reset();
		}
		try {
			secondaryCogwheelSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/cogwheel_grandidierite.svg"));
		}
		catch (const std::exception& e) {
			WARN("Failed to load secondary cogwheel-backed gear knob SVG: %s", e.what());
			secondaryCogwheelSvg.reset();
		}
	}

	void draw(const DrawArgs& args) override {
		drawCogwheel(args);
		GearKnobInvertSized::draw(args);
	}

	void drawCogwheel(const DrawArgs& args) {
		const float primaryDiameterPx = 17.f;
		const float secondaryDiameterPx = primaryDiameterPx * 0.5f;
		const float valueNorm = normalizedParamValue();
		const float knobAngle = crossfade(minAngle, maxAngle, valueNorm);
		const Vec primaryPos = box.size.mult(0.5f);
		const Vec primaryCenter = primaryPos.plus(Vec(0.5f * primaryDiameterPx, 0.5f * primaryDiameterPx));

		drawCogwheelSvg(args, cogwheelSvg, primaryCenter, primaryDiameterPx, -knobAngle);

		const float centerDistancePx = 0.5f * (primaryDiameterPx + secondaryDiameterPx) - 0.45f;
		const Vec secondaryCenter = primaryCenter.plus(Vec(
			-0.8338858f * centerDistancePx,
			0.551937f * centerDistancePx));
		const float secondaryGearRatio = primaryDiameterPx / secondaryDiameterPx;
		drawCogwheelSvg(args, secondaryCogwheelSvg, secondaryCenter, secondaryDiameterPx, knobAngle * secondaryGearRatio);
	}

	void drawCogwheelSvg(const DrawArgs& args, const std::shared_ptr<window::Svg>& svg, const Vec& center, float diameterPx, float angleRad) {
		if (!svg) return;
		const Vec svgSize = svg->getSize();
		if (svgSize.x <= 1.f || svgSize.y <= 1.f) return;

		const float scale = diameterPx / std::max(svgSize.x, svgSize.y);
		nvgSave(args.vg);
		nvgTranslate(args.vg, center.x, center.y);
		nvgRotate(args.vg, angleRad);
		nvgScale(args.vg, scale, scale);
		nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
		svg->draw(args.vg);
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

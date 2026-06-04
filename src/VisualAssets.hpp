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

	void draw(const DrawArgs& args) override {
		app::SvgKnob::draw(args);
		drawActiveRing(args);
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

	void drawActiveRing(const DrawArgs& args) {
		const float valueNorm = normalizedParamValue();
		const float knobAngle = crossfade(minAngle, maxAngle, valueNorm);
		const float assetScale = 46.f / 56.f;
		const Vec center = Vec(28.f, 28.f).mult(assetScale);
		const float ringRadius = 20.75f * assetScale;
		const float ringWidth = 3.4f * assetScale;
		const float startAngle = -0.5f * M_PI + minAngle;
		const float endAngle = -0.5f * M_PI + maxAngle;
		const float activeAngle = -0.5f * M_PI + knobAngle;

		nvgSave(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, ringRadius, startAngle, endAngle, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(5, 4, 3, 210));
		nvgStrokeWidth(args.vg, ringWidth);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, ringRadius, startAngle, activeAngle, NVG_CW);
		NVGpaint activePaint = nvgLinearGradient(args.vg,
			center.x - ringRadius, center.y,
			center.x + ringRadius, center.y,
			nvgRGBA(255, 184, 24, 235),
			nvgRGBA(255, 252, 95, 250));
		nvgStrokePaint(args.vg, activePaint);
		nvgStrokeWidth(args.vg, ringWidth);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, ringRadius - 0.5f * ringWidth, startAngle, endAngle, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(255, 244, 154, 80));
		nvgStrokeWidth(args.vg, 0.55f * assetScale);
		nvgStroke(args.vg);

		nvgRestore(args.vg);
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
			-0.9636305f * centerDistancePx,
			0.2672384f * centerDistancePx));
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

};

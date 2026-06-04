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
		drawActivePointer(args);
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

	void drawActivePointer(const DrawArgs& args) {
		const float valueNorm = normalizedParamValue();
		const float knobAngle = crossfade(minAngle, maxAngle, valueNorm);
		const float assetScale = 46.f / 56.f;
		const Vec center = Vec(28.f, 28.f).mult(assetScale);
		const float tipY = 7.25f * assetScale;
		const float baseY = 28.f * assetScale;
		const float tipHalfWidth = 1.85f * assetScale;
		const float midHalfWidth = 2.55f * assetScale;
		const float baseHalfWidth = 2.9f * assetScale;
		const float activeTopY = crossfade(baseY, tipY, valueNorm);
		const float activeHalfWidth = crossfade(baseHalfWidth, tipHalfWidth, valueNorm);

		nvgSave(args.vg);
		nvgTranslate(args.vg, center.x, center.y);
		nvgRotate(args.vg, knobAngle);
		nvgTranslate(args.vg, -center.x, -center.y);

		NVGpaint inactivePaint = nvgLinearGradient(args.vg,
			center.x - baseHalfWidth, tipY,
			center.x + baseHalfWidth, baseY,
			nvgRGBA(40, 30, 16, 255),
			nvgRGBA(5, 4, 3, 255));
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, center.x - baseHalfWidth, baseY);
		nvgBezierTo(args.vg, center.x - midHalfWidth, 20.8f * assetScale, center.x - 2.15f * assetScale, 13.7f * assetScale, center.x - tipHalfWidth, tipY);
		nvgLineTo(args.vg, center.x + tipHalfWidth, tipY);
		nvgBezierTo(args.vg, center.x + 2.15f * assetScale, 13.7f * assetScale, center.x + midHalfWidth, 20.8f * assetScale, center.x + baseHalfWidth, baseY);
		nvgBezierTo(args.vg, center.x + 0.75f * assetScale, 28.45f * assetScale, center.x - 0.75f * assetScale, 28.45f * assetScale, center.x - baseHalfWidth, baseY);
		nvgFillPaint(args.vg, inactivePaint);
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(255, 218, 88, 170));
		nvgStrokeWidth(args.vg, 0.72f);
		nvgStroke(args.vg);

		NVGpaint activePaint = nvgLinearGradient(args.vg,
			center.x, baseY,
			center.x, tipY,
			nvgRGBA(255, 204, 34, 235),
			nvgRGBA(255, 252, 112, 250));
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, center.x - baseHalfWidth, baseY);
		nvgBezierTo(args.vg, center.x - activeHalfWidth, crossfade(baseY, activeTopY, 0.35f), center.x - activeHalfWidth, crossfade(baseY, activeTopY, 0.7f), center.x - activeHalfWidth, activeTopY);
		nvgLineTo(args.vg, center.x + activeHalfWidth, activeTopY);
		nvgBezierTo(args.vg, center.x + activeHalfWidth, crossfade(baseY, activeTopY, 0.7f), center.x + activeHalfWidth, crossfade(baseY, activeTopY, 0.35f), center.x + baseHalfWidth, baseY);
		nvgBezierTo(args.vg, center.x + 0.75f * assetScale, 28.45f * assetScale, center.x - 0.75f * assetScale, 28.45f * assetScale, center.x - baseHalfWidth, baseY);
		nvgFillPaint(args.vg, activePaint);
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, center.x, baseY - 1.4f * assetScale);
		nvgLineTo(args.vg, center.x, activeTopY + 0.7f * assetScale);
		nvgStrokeColor(args.vg, nvgRGBA(255, 252, 120, 165));
		nvgStrokeWidth(args.vg, 0.52f);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgCircle(args.vg, center.x, center.y, baseHalfWidth);
		nvgFillColor(args.vg, nvgRGBA(3, 2, 1, 255));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, center.x, center.y, 1.45f * assetScale);
		nvgFillColor(args.vg, nvgRGBA(255, 228, 54, 185));
		nvgFill(args.vg);

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

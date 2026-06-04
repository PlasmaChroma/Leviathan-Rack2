#pragma once

#include "plugin.hpp"
#include <map>
#include <string>

namespace visual_assets {

inline std::shared_ptr<window::Svg> loadPluginSvgCached(const char* path) {
	static std::map<std::string, std::shared_ptr<window::Svg>> cache;
	const std::string key = path ? path : "";
	auto it = cache.find(key);
	if (it != cache.end()) {
		return it->second;
	}
	std::shared_ptr<window::Svg> svg = Svg::load(asset::plugin(pluginInstance, key));
	cache[key] = svg;
	return svg;
}

} // namespace visual_assets

struct GearKnobInvertSized : app::SvgKnob {
	struct ActiveRingWidget : TransparentWidget {
		float minAngle = -0.83f * M_PI;
		float maxAngle = 0.83f * M_PI;
		float valueNorm = 0.5f;

		void draw(const DrawArgs& args) override {
			const float knobAngle = crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
			const float assetScale = 46.f / 56.f;
			const Vec center = Vec(28.f, 28.f).mult(assetScale);
			const float ringRadius = 18.9f * assetScale;
			const float ringWidth = 5.0f * assetScale;
			const float activeRingWidth = 3.2f * assetScale;
			const float startAngle = -0.5f * M_PI + minAngle;
			const float endAngle = -0.5f * M_PI + maxAngle;
			const float activeAngle = -0.5f * M_PI + knobAngle;

			nvgSave(args.vg);

			nvgBeginPath(args.vg);
			nvgArc(args.vg, center.x, center.y, ringRadius, startAngle, endAngle, NVG_CW);
			nvgStrokeColor(args.vg, nvgRGBA(2, 1, 1, 230));
			nvgStrokeWidth(args.vg, ringWidth);
			nvgLineCap(args.vg, NVG_ROUND);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgArc(args.vg, center.x, center.y, ringRadius, startAngle, activeAngle, NVG_CW);
			NVGpaint activePaint = nvgLinearGradient(args.vg,
				center.x - ringRadius, center.y,
				center.x + ringRadius, center.y,
				nvgRGBA(255, 218, 42, 248),
				nvgRGBA(255, 250, 205, 255));
			nvgStrokePaint(args.vg, activePaint);
			nvgStrokeWidth(args.vg, activeRingWidth);
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

	ActiveRingWidget* activeRing = nullptr;

	GearKnobInvertSized() {
		minAngle = -0.83 * M_PI;
		maxAngle = 0.83 * M_PI;

		setSvg(visual_assets::loadPluginSvgCached("res/icon/gear_knob_invert.svg"));
		box.size = Vec(46.f, 46.f);
		if (shadow) {
			shadow->opacity = 0.f;
		}
		activeRing = new ActiveRingWidget();
		activeRing->box.size = box.size;
		activeRing->minAngle = minAngle;
		activeRing->maxAngle = maxAngle;
		activeRing->valueNorm = normalizedParamValue();
		fb->addChild(activeRing);
	}

	void draw(const DrawArgs& args) override {
		app::SvgKnob::draw(args);
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

	void onChange(const ChangeEvent& e) override {
		app::SvgKnob::onChange(e);
		if (activeRing) {
			activeRing->valueNorm = normalizedParamValue();
		}
		if (fb) {
			fb->setDirty();
		}
	}
};

struct ClockworkGearKnob : GearKnobInvertSized {
	struct CogwheelWidget : TransparentWidget {
		std::shared_ptr<window::Svg> svg;
		Vec center;
		float diameterPx = 1.f;
		float angleRad = 0.f;

		void draw(const DrawArgs& args) override {
			if (!svg) return;
			const Vec svgSize = svg->getSize();
			if (svgSize.x <= 1.f || svgSize.y <= 1.f || diameterPx <= 0.f) return;

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

	CogwheelWidget* primaryCogwheel = nullptr;
	CogwheelWidget* secondaryCogwheel = nullptr;

	ClockworkGearKnob() {
		primaryCogwheel = new CogwheelWidget();
		secondaryCogwheel = new CogwheelWidget();
		primaryCogwheel->box.size = box.size;
		secondaryCogwheel->box.size = box.size;
		try {
			primaryCogwheel->svg = visual_assets::loadPluginSvgCached("res/icon/cogwheel_amythyst.svg");
		}
		catch (const std::exception& e) {
			WARN("Failed to load cogwheel-backed gear knob SVG: %s", e.what());
			primaryCogwheel->svg.reset();
		}
		try {
			secondaryCogwheel->svg = visual_assets::loadPluginSvgCached("res/icon/cogwheel_grandidierite.svg");
		}
		catch (const std::exception& e) {
			WARN("Failed to load secondary cogwheel-backed gear knob SVG: %s", e.what());
			secondaryCogwheel->svg.reset();
		}
		updateCogwheelGeometry();
		fb->addChildBelow(primaryCogwheel, tw);
		fb->addChildBelow(secondaryCogwheel, tw);
	}

	void draw(const DrawArgs& args) override {
		GearKnobInvertSized::draw(args);
	}

	void onChange(const ChangeEvent& e) override {
		GearKnobInvertSized::onChange(e);
		updateCogwheelGeometry();
		if (fb) {
			fb->setDirty();
		}
	}

	void updateCogwheelGeometry() {
		if (!primaryCogwheel || !secondaryCogwheel) return;
		const float primaryDiameterPx = 17.f;
		const float secondaryDiameterPx = primaryDiameterPx * 0.5f;
		const float valueNorm = normalizedParamValue();
		const float knobAngle = crossfade(minAngle, maxAngle, valueNorm);
		const Vec cogwheelOffset(0.f, -1.25f);
		const Vec primaryPos = box.size.mult(0.5f).plus(cogwheelOffset);
		const Vec primaryCenter = primaryPos.plus(Vec(0.5f * primaryDiameterPx, 0.5f * primaryDiameterPx));

		primaryCogwheel->center = primaryCenter;
		primaryCogwheel->diameterPx = primaryDiameterPx;
		primaryCogwheel->angleRad = -knobAngle;

		const float centerDistancePx = 0.5f * (primaryDiameterPx + secondaryDiameterPx) - 0.45f;
		const Vec secondaryCenter = primaryCenter.plus(Vec(
			-0.9636305f * centerDistancePx,
			0.2672384f * centerDistancePx));
		const float secondaryGearRatio = primaryDiameterPx / secondaryDiameterPx;
		secondaryCogwheel->center = secondaryCenter;
		secondaryCogwheel->diameterPx = secondaryDiameterPx;
		secondaryCogwheel->angleRad = knobAngle * secondaryGearRatio;
	}

};

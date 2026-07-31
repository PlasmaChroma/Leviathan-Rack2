#include "VisualAssets.hpp"

#include <algorithm>
#include <cmath>

namespace {

NVGcolor blendHaloColor(NVGcolor a, NVGcolor b, float t) {
	t = clamp(t, 0.f, 1.f);
	NVGcolor out;
	out.r = crossfade(a.r, b.r, t);
	out.g = crossfade(a.g, b.g, t);
	out.b = crossfade(a.b, b.b, t);
	out.a = crossfade(a.a, b.a, t);
	return out;
}

// Halo bloom/reflection "segments" have no gaps. Adjacent segments with the
// same color can therefore be emitted as one arc without changing the image.
// Only the segment under the value cursor needs its own blended color.
template <typename DrawArc>
void drawCoalescedHaloSegments(
	float startAngle,
	float endAngle,
	float activeAngle,
	NVGcolor activeColor,
	NVGcolor inactiveColor,
	DrawArc&& drawArc) {
	constexpr int segmentCount = 16;
	const float step = (endAngle - startAngle) / float(segmentCount);
	const float position = clamp((activeAngle - startAngle) / std::max(step, 1e-6f), 0.f, float(segmentCount));
	const int completedSegments = std::min(segmentCount, int(position));
	const float completedEnd = startAngle + step * float(completedSegments);

	if (completedSegments > 0) {
		drawArc(startAngle, completedEnd, activeColor);
	}

	float inactiveStart = completedEnd;
	if (completedSegments < segmentCount) {
		const float partial = position - float(completedSegments);
		if (partial > 0.f) {
			const float partialEnd = completedEnd + step;
			drawArc(completedEnd, partialEnd, blendHaloColor(inactiveColor, activeColor, partial));
			inactiveStart = partialEnd;
		}
	}

	if (inactiveStart < endAngle) {
		drawArc(inactiveStart, endAngle, inactiveColor);
	}
}

} // namespace

void LeviathanHaloKnob2::GlowArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float mainRadius = diameterPx * (18.15f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float bloomRaw = clamp(settings::haloBrightness, 0.f, 1.5f);
	const float bloomLow = bloomRaw + 2.8f * bloomRaw * (1.f - bloomRaw);
	const float bloomRamp = clamp((bloomRaw - 0.50f) / 0.50f, 0.f, 1.f);
	const float bloom = bloomLow * (1.0f + 1.40f * bloomRamp * bloomRamp);
	if (bloom <= 0.001f) return;

	auto bloomColor = [&](NVGcolor color) {
		color.a *= bloom;
		return color;
	};

	nvgSave(args.vg);

	auto drawGlowStroke = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor color) {
		if (a1 <= a0) return;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto drawSegmentedGlow = [&](float widthPx, NVGcolor cyan, NVGcolor purple) {
		drawCoalescedHaloSegments(startAngle, endAngle, activeAngle, cyan, purple,
			[&](float a0, float a1, NVGcolor color) {
				drawGlowStroke(a0, a1, mainRadius, widthPx, color);
			});
	};

	if (foreground) {
		drawSegmentedGlow(std::max(2.2f, 2.7f * scale), bloomColor(config.foregroundOuterActiveColor), bloomColor(config.foregroundOuterInactiveColor));
		drawSegmentedGlow(std::max(1.2f, 1.6f * scale), bloomColor(config.foregroundInnerActiveColor), bloomColor(config.foregroundInnerInactiveColor));
	}
	else {
		drawSegmentedGlow(std::max(5.8f, 6.4f * scale), bloomColor(config.backgroundOuterActiveColor), bloomColor(config.backgroundOuterInactiveColor));
		drawSegmentedGlow(std::max(3.8f, 4.6f * scale), bloomColor(config.backgroundMidActiveColor), bloomColor(config.backgroundMidInactiveColor));
		drawSegmentedGlow(std::max(2.4f, 3.0f * scale), bloomColor(config.backgroundInnerActiveColor), bloomColor(config.backgroundInnerInactiveColor));
	}

	nvgRestore(args.vg);
}

void LeviathanHaloKnob2::LightArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float mainRadius = diameterPx * (18.15f / 46.f);
	const float mainWidth = std::max(1.35f, diameterPx * (1.85f / 46.f));
	const float segmentWidth = mainWidth + 0.95f * scale;
	const float segmentRadius = mainRadius - 0.5f * (segmentWidth - mainWidth);
	const float guideRadius = diameterPx * (20.70f / 46.f);
	const float guideWidth = std::max(0.28f, diameterPx * (0.42f / 46.f));
	const float bloomRaw = clamp(settings::haloBrightness, 0.f, 1.5f);
	const float bloomLow = bloomRaw + 2.8f * bloomRaw * (1.f - bloomRaw);
	const float bloomRamp = clamp((bloomRaw - 0.50f) / 0.50f, 0.f, 1.f);
	const float bloom = bloomLow * (1.0f + 1.40f * bloomRamp * bloomRamp);

	auto bloomColor = [&](NVGcolor color) {
		color.a *= bloom;
		return color;
	};

	nvgSave(args.vg);

	auto drawArcBand = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor color) {
		if (a1 <= a0) return;
		const float halfWidthPx = 0.5f * widthPx;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx + halfWidthPx, a0, a1, NVG_CW);
		nvgArc(args.vg, center.x, center.y, radiusPx - halfWidthPx, a1, a0, NVG_CCW);
		nvgClosePath(args.vg);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);
	};

	auto drawGuideArc = [&](float radiusPx, float widthPx, NVGcolor color) {
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, startAngle, endAngle, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto drawPartialGuideArc = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor color) {
		if (a1 <= a0) return;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto drawSegmentBand = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor fill, NVGcolor innerHighlight) {
		if (a1 <= a0) return;
		const float halfWidthPx = 0.5f * widthPx;

		drawArcBand(a0, a1, radiusPx, widthPx + 0.48f * scale, nvgRGBA(0, 0, 4, 218));

		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx + halfWidthPx, a0, a1, NVG_CW);
		nvgArc(args.vg, center.x, center.y, radiusPx - halfWidthPx, a1, a0, NVG_CCW);
		nvgClosePath(args.vg);

		NVGpaint segmentPaint = nvgLinearGradient(args.vg,
			center.x + std::cos(a0) * radiusPx,
			center.y + std::sin(a0) * radiusPx,
			center.x + std::cos(a1) * radiusPx,
			center.y + std::sin(a1) * radiusPx,
			fill,
			innerHighlight);
		nvgFillPaint(args.vg, segmentPaint);
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx + halfWidthPx * 0.84f, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(0, 1, 7, 172));
		nvgStrokeWidth(args.vg, std::max(0.13f, widthPx * 0.09f));
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);

		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx - halfWidthPx * 0.52f, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, innerHighlight);
		nvgStrokeWidth(args.vg, std::max(0.16f, widthPx * 0.12f));
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto drawSegmentedValueArc = [&]() {
		const int segmentCount = 16;
		const float aStart = startAngle;
		const float aEnd = endAngle;
		const float total = aEnd - aStart;
		const float gap = std::max(0.010f, total * 0.009f);
		const float step = total / float(segmentCount);
		const NVGcolor litCore = config.activeColor;
		const NVGcolor litHot = config.activeHighlightColor;
		const NVGcolor unlitCore = config.inactiveColor;
		const NVGcolor unlitHot = config.inactiveHighlightColor;
		for (int i = 0; i < segmentCount; ++i) {
			const float s0 = aStart + step * float(i) + 0.5f * gap;
			const float s1 = aStart + step * float(i + 1) - 0.5f * gap;
			if (activeAngle <= s0) {
				drawSegmentBand(s0, s1, segmentRadius, segmentWidth, unlitCore, unlitHot);
			}
			else if (activeAngle >= s1) {
				drawSegmentBand(s0, s1, segmentRadius, segmentWidth, litCore, litHot);
			}
			else {
				const float segmentProgress = (activeAngle - s0) / std::max(1e-6f, s1 - s0);
				drawSegmentBand(
					s0,
					s1,
					segmentRadius,
					segmentWidth,
					blendHaloColor(unlitCore, litCore, segmentProgress),
					blendHaloColor(unlitHot, litHot, segmentProgress));
			}
		}
	};

	auto drawSegmentedReflection = [&](float radiusPx, float widthPx, NVGcolor cyan, NVGcolor purple) {
		drawCoalescedHaloSegments(startAngle, endAngle, activeAngle, cyan, purple,
			[&](float a0, float a1, NVGcolor color) {
				drawPartialGuideArc(a0, a1, radiusPx, widthPx, color);
			});
	};

	auto drawTerminator = [&](float angle, float direction) {
		const float terminatorSweep = 0.055f;
		const float a0 = angle + std::min(0.f, direction) * terminatorSweep;
		const float a1 = angle + std::max(0.f, direction) * terminatorSweep;
		drawArcBand(a0, a1, segmentRadius, segmentWidth + 1.15f * scale, nvgRGBA(0, 1, 8, 230));
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, segmentRadius - segmentWidth * 0.30f, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(155, 170, 190, 48));
		nvgStrokeWidth(args.vg, std::max(0.16f, 0.22f * scale));
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	const float dipRadius = mainRadius - mainWidth * 1.03f - 0.46f * scale;
	drawArcBand(startAngle, endAngle, mainRadius, mainWidth + 0.92f * scale, nvgRGBA(0, 0, 4, 248));
	drawArcBand(startAngle, endAngle, dipRadius, std::max(0.55f, 0.82f * scale), nvgRGBA(0, 1, 8, 216));
	drawGuideArc(guideRadius, guideWidth, bloomConfig.guideOuterColor);
	drawGuideArc(guideRadius - 0.20f * scale, std::max(0.18f, 0.24f * scale), bloomConfig.guideMidColor);
	drawGuideArc(mainRadius - mainWidth * 0.78f, std::max(0.16f, 0.20f * scale), bloomConfig.guideInnerColor);
	if (bloom > 0.001f) {
		drawSegmentedReflection(dipRadius - 0.18f * scale, std::max(0.28f, 0.38f * scale), bloomColor(bloomConfig.reflectionOuterActiveColor), bloomColor(bloomConfig.reflectionOuterInactiveColor));
		drawSegmentedReflection(dipRadius - 0.52f * scale, std::max(0.12f, 0.17f * scale), bloomColor(bloomConfig.reflectionInnerActiveColor), bloomColor(bloomConfig.reflectionInnerInactiveColor));
	}
	drawGuideArc(dipRadius + 0.46f * scale, std::max(0.15f, 0.22f * scale), nvgRGBA(0, 0, 4, 172));

	drawSegmentedValueArc();
	drawTerminator(startAngle, 1.f);
	drawTerminator(endAngle, -1.f);

	NVGpaint capShadow = nvgRadialGradient(
		args.vg,
		center.x,
		center.y + diameterPx * 0.045f,
		diameterPx * (11.0f / 46.f),
		diameterPx * (16.2f / 46.f),
		nvgRGBA(0, 0, 0, 0),
		nvgRGBA(0, 0, 0, 76));
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, diameterPx * (16.8f / 46.f));
	nvgFillPaint(args.vg, capShadow);
	nvgFill(args.vg);

	nvgRestore(args.vg);
}

void LeviathanHaloKnob2::CapReflectionWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float rimRadius = diameterPx * (14.62f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float bloomRaw = clamp(settings::haloBrightness, 0.f, 1.5f);
	const float bloomLow = bloomRaw + 2.8f * bloomRaw * (1.f - bloomRaw);
	const float bloomRamp = clamp((bloomRaw - 0.50f) / 0.50f, 0.f, 1.f);
	const float bloom = bloomLow * (1.0f + 1.40f * bloomRamp * bloomRamp);
	if (bloom <= 0.001f) return;

	auto bloomColor = [&](NVGcolor color) {
		color.a *= bloom;
		return color;
	};

	auto strokeArc = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor color) {
		if (a1 <= a0) return;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};

	auto strokeSegmentedReflection = [&](float radiusPx, float widthPx, NVGcolor cyan, NVGcolor purple) {
		drawCoalescedHaloSegments(startAngle, endAngle, activeAngle, cyan, purple,
			[&](float a0, float a1, NVGcolor color) {
				strokeArc(a0, a1, radiusPx, widthPx, color);
			});
	};

	nvgSave(args.vg);

	strokeSegmentedReflection(rimRadius, std::max(0.30f, 0.42f * scale), bloomColor(config.capReflectionOuterActiveColor), bloomColor(config.capReflectionOuterInactiveColor));
	strokeSegmentedReflection(rimRadius - 0.34f * scale, std::max(0.12f, 0.17f * scale), bloomColor(config.capReflectionInnerActiveColor), bloomColor(config.capReflectionInnerInactiveColor));

	nvgRestore(args.vg);
}

LeviathanHaloKnob2::LeviathanHaloKnob2() : LeviathanHaloKnob2(Config()) {
}

LeviathanHaloKnob2::Config LeviathanHaloKnob2::brightOrangeConfig() {
	Config config;
	config.ledArc.activeColor = nvgRGBA(255, 184, 0, 255);
	config.ledArc.activeHighlightColor = nvgRGBA(255, 232, 82, 232);
	config.ledArc.inactiveColor = nvgRGBA(158, 58, 16, 216);
	config.ledArc.inactiveHighlightColor = nvgRGBA(220, 94, 30, 168);
	config.bloom.backgroundOuterActiveColor = nvgRGBA(255, 148, 0, 50);
	config.bloom.backgroundOuterInactiveColor = nvgRGBA(130, 42, 10, 30);
	config.bloom.backgroundMidActiveColor = nvgRGBA(255, 172, 0, 80);
	config.bloom.backgroundMidInactiveColor = nvgRGBA(160, 50, 12, 50);
	config.bloom.backgroundInnerActiveColor = nvgRGBA(255, 204, 20, 122);
	config.bloom.backgroundInnerInactiveColor = nvgRGBA(204, 68, 16, 72);
	config.bloom.foregroundOuterActiveColor = nvgRGBA(255, 214, 34, 74);
	config.bloom.foregroundOuterInactiveColor = nvgRGBA(206, 72, 18, 44);
	config.bloom.foregroundInnerActiveColor = nvgRGBA(255, 244, 118, 62);
	config.bloom.foregroundInnerInactiveColor = nvgRGBA(236, 104, 34, 32);
	config.bloom.reflectionOuterActiveColor = nvgRGBA(255, 174, 0, 70);
	config.bloom.reflectionOuterInactiveColor = nvgRGBA(144, 44, 10, 68);
	config.bloom.reflectionInnerActiveColor = nvgRGBA(255, 224, 36, 62);
	config.bloom.reflectionInnerInactiveColor = nvgRGBA(218, 86, 22, 48);
	config.bloom.guideOuterColor = nvgRGBA(255, 210, 38, 84);
	config.bloom.guideMidColor = nvgRGBA(186, 58, 14, 58);
	config.bloom.guideInnerColor = nvgRGBA(255, 238, 98, 68);
	config.bloom.capReflectionOuterActiveColor = nvgRGBA(255, 188, 0, 96);
	config.bloom.capReflectionOuterInactiveColor = nvgRGBA(188, 62, 16, 82);
	config.bloom.capReflectionInnerActiveColor = nvgRGBA(255, 240, 108, 72);
	config.bloom.capReflectionInnerInactiveColor = nvgRGBA(236, 106, 36, 54);
	return config;
}

LeviathanHaloKnob2::LeviathanHaloKnob2(Config config) : config(config) {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	std::shared_ptr<window::Svg> backSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnob2Back.svg");
	centerNormalSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnobCenter.svg");
	centerLitSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnobCenterLit.svg");
	app::SvgKnob::setSvg(backSvg);
	box.size = Vec(46.f, 46.f);
	if (fb) {
		fb->box.size = box.size;
	}
	if (sw) {
		sw->hide();
	}
	if (shadow) {
		shadow->opacity = 0.f;
	}
	lastBloomAmount = settings::haloBrightness;

	backLayer = new EclipseKnob::SvgLayer();
	backLayer->setSvg(backSvg);
	backLayer->box.size = box.size;
	backLayer->minAngle = minAngle;
	backLayer->maxAngle = maxAngle;
	backLayer->valueNorm = normalizedParamValue();
	backLayer->rotateWithValue = false;
	fb->addChild(backLayer);

	glowArc = new GlowArcWidget();
	glowArc->box.size = box.size;
	glowArc->minAngle = minAngle;
	glowArc->maxAngle = maxAngle;
	glowArc->valueNorm = normalizedParamValue();
	glowArc->config = this->config.bloom;
	fb->addChild(glowArc);

	lightArc = new LightArcWidget();
	lightArc->box.size = box.size;
	lightArc->minAngle = minAngle;
	lightArc->maxAngle = maxAngle;
	lightArc->valueNorm = normalizedParamValue();
	lightArc->config = this->config.ledArc;
	lightArc->bloomConfig = this->config.bloom;
	fb->addChild(lightArc);

	foregroundGlowArc = new GlowArcWidget();
	foregroundGlowArc->box.size = box.size;
	foregroundGlowArc->minAngle = minAngle;
	foregroundGlowArc->maxAngle = maxAngle;
	foregroundGlowArc->valueNorm = normalizedParamValue();
	foregroundGlowArc->foreground = true;
	foregroundGlowArc->config = this->config.bloom;
	fb->addChild(foregroundGlowArc);

	centerLayer = new EclipseKnob::SvgLayer();
	centerLayer->setSvg(centerNormalSvg);
	centerLayer->box.size = box.size;
	centerLayer->minAngle = minAngle;
	centerLayer->maxAngle = maxAngle;
	centerLayer->valueNorm = normalizedParamValue();
	centerLayer->rotateWithValue = true;
	fb->addChild(centerLayer);

	capReflection = new CapReflectionWidget();
	capReflection->box.size = box.size;
	capReflection->minAngle = minAngle;
	capReflection->maxAngle = maxAngle;
	capReflection->valueNorm = normalizedParamValue();
	capReflection->config = this->config.bloom;
	fb->addChild(capReflection);

}

void LeviathanHaloKnob2::updateCenterSvg() {
	const bool shouldLight = hovered || dragging;
	if (centerLit == shouldLight) {
		return;
	}
	centerLit = shouldLight;
	if (centerLayer) {
		centerLayer->setSvg(centerLit ? centerLitSvg : centerNormalSvg);
		centerLayer->box.size = box.size;
		centerLayer->valueNorm = normalizedParamValue();
	}
	if (fb) {
		fb->setDirty();
	}
}

void LeviathanHaloKnob2::step() {
	app::SvgKnob::step();
	const float bloomAmount = settings::haloBrightness;
	if (std::fabs(bloomAmount - lastBloomAmount) > 1e-4f) {
		lastBloomAmount = bloomAmount;
		if (fb) {
			fb->setDirty();
		}
	}
}

void LeviathanHaloKnob2::onEnter(const event::Enter& e) {
	hovered = true;
	updateCenterSvg();
	app::SvgKnob::onEnter(e);
}

void LeviathanHaloKnob2::onLeave(const event::Leave& e) {
	hovered = false;
	updateCenterSvg();
	app::SvgKnob::onLeave(e);
}

void LeviathanHaloKnob2::onDragStart(const event::DragStart& e) {
	dragging = true;
	updateCenterSvg();
	app::SvgKnob::onDragStart(e);
}

void LeviathanHaloKnob2::onDragEnd(const event::DragEnd& e) {
	dragging = false;
	updateCenterSvg();
	app::SvgKnob::onDragEnd(e);
}

void LeviathanHaloKnob2::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	const float valueNorm = normalizedParamValue();
	if (backLayer) {
		backLayer->valueNorm = valueNorm;
	}
	if (centerLayer) {
		centerLayer->valueNorm = valueNorm;
	}
	if (capReflection) {
		capReflection->valueNorm = valueNorm;
	}
	if (glowArc) {
		glowArc->valueNorm = valueNorm;
	}
	if (foregroundGlowArc) {
		foregroundGlowArc->valueNorm = valueNorm;
	}
	if (lightArc) {
		lightArc->valueNorm = valueNorm;
	}
	if (fb) {
		fb->setDirty();
	}
}

float LeviathanHaloKnob2::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float maxValue = pq->getMaxValue();
	const float range = maxValue - minValue;
	if (range <= 1e-6f) return 0.5f;
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}

#include "BifurxSpectrumNanoVG.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/PlasmaConduit.hpp"

namespace bifurx {


float BifurxSpectrumWidget::getTopLabelReservedWidth(const DrawArgs& args, float fontSize) {
	const int fontHandle = (APP && APP->window && APP->window->uiFont) ? APP->window->uiFont->handle : -1;
	if (fontHandle == cachedTopLabelFontHandle &&
		std::isfinite(cachedTopLabelFontSize) &&
		std::fabs(cachedTopLabelFontSize - fontSize) <= 1e-5f &&
		cachedTopLabelReservedWidth > 0.f) {
		return cachedTopLabelReservedWidth;
	}

	auto compactSignedLabel = [](float value, char* out, size_t outSize) {
		std::snprintf(out, outSize, "%+.1f", value);
	};
	auto measureTopLabelWidthForValue = [&](float db) {
		char sampleValue[12];
		compactSignedLabel(db, sampleValue, sizeof(sampleValue));
		char sampleLabel[24];
		std::snprintf(sampleLabel, sizeof(sampleLabel), "%5s dBFS", sampleValue);
		return nvgTextBounds(args.vg, 0.f, 0.f, sampleLabel, nullptr, nullptr);
	};

	float topLabelReservedWidth = 0.f;
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(kDisplayTopDbfsFloor));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(-10.f));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(-1.f));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(kDisplayTopDbfsCeiling));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(kDisplayTopDynamicCeilingDbfs));

	cachedTopLabelFontHandle = fontHandle;
	cachedTopLabelFontSize = fontSize;
	cachedTopLabelReservedWidth = topLabelReservedWidth;
	return topLabelReservedWidth;
}

void BifurxSpectrumWidget::step() {
	using PerfClock = std::chrono::steady_clock;
	const bool debugEnabled = isDragonKingDebugEnabled();
	const bool perfLoggingActive = debugEnabled && module && module->perfDebugLogging.load(std::memory_order_relaxed);
	const bool measurePerf = debugEnabled;
	const PerfClock::time_point perfStepStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
	Widget::step();
	if (!module) {
		const bool hadPreview = state.hasPreview;
		initializeStaticPreviewStateIfNeeded();
		if (!hadPreview && framebuffer) {
			framebuffer->dirty = true;
		}
		return;
	}
	if (!presentationActive) return;
	if (module->renderMode != Bifurx::RENDER_NANOVG) return;

	bool dirty = false;

	const bool showModuleResponseOverlayNow = module->showModuleResponseOverlay.load(std::memory_order_relaxed);
	if (showModuleResponseOverlayNow != lastShowModuleResponseOverlay) {
		lastShowModuleResponseOverlay = showModuleResponseOverlayNow;
		dirty = true;
	}
	const int colorSchemeNow = int(module->colorScheme);
	if (colorSchemeNow != lastColorScheme) {
		lastColorScheme = colorSchemeNow;
		dirty = true;
	}
	const bool threeColorFftGradientNow = module->threeColorFftGradient.load(std::memory_order_relaxed);
	if (threeColorFftGradientNow != lastThreeColorFftGradient) {
		lastThreeColorFftGradient = threeColorFftGradientNow;
		dirty = true;
	}

	float uiFrameSec = 1.f / 60.f;
	if (APP && APP->window) {
		const float frameSec = float(APP->window->getLastFrameDuration());
		if (std::isfinite(frameSec) && frameSec > 0.f) {
			uiFrameSec = clamp(frameSec, 1.f / 240.f, 1.f / 20.f);
		}
	}

	const BifurxRenderTickResult tick = runRenderTick(uiFrameSec);
	if (tick.curvePrepUs > 0.f) {
		lastCurvePrepUs = tick.curvePrepUs;
	}
	if (tick.overlayPrepUs > 0.f) {
		lastOverlayPrepUs = tick.overlayPrepUs;
	}
	if (tick.contentChanged) {
		dirty = true;
	}


	if (dirty && framebuffer) framebuffer->setDirty();

	if (measurePerf) {
		const float stepMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
			PerfClock::now() - perfStepStart).count()) * 1e-6f;
		lastStepMsEma = (lastStepMsEma > 0.f) ? (lastStepMsEma + (stepMs - lastStepMsEma) * 0.18f) : stepMs;
	}

	if (perfLoggingActive) {
		const uint64_t stepNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfStepStart).count();
		uiStepCount++; uiStepNs += stepNs; uiStepMaxNs = std::max(uiStepMaxNs, stepNs);
	}
}

void BifurxSpectrumWidget::draw(const DrawArgs& args) {
	if (!state.hasCurve) return;
	const float w = box.size.x, h = box.size.y;
	if (!(w > 0.f && h > 0.f)) return;
	using PerfClock = std::chrono::steady_clock;
	const bool debugEnabled = isDragonKingDebugEnabled();
	const bool perfLoggingActive = debugEnabled && module && module->perfDebugLogging.load(std::memory_order_relaxed);
	const bool measurePerf = debugEnabled;
	const PerfClock::time_point perfDrawStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
	PerfClock::time_point perfSectionStart = perfDrawStart;
	auto recordDrawSection = [&](uint64_t& count, uint64_t& totalNs) {
		if (!perfLoggingActive) return;
		const uint64_t ns = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfSectionStart).count();
		count++; totalNs += ns; perfSectionStart = PerfClock::now();
	};
	const float padY = std::max(4.f, h * 0.035f), plotX = 0.f, usableW = std::max(1.f, w - plotX);
	const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight, spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
	bottomY = spectrumBottomY;
	const float displayMaxDbfs = state.displayTopDbfs, displayMinDbfs = displayMaxDbfs - kDisplayDbfsSpan;
	auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
	updateCurveXCache(plotX, usableW);
	const bool displayOnlyMode = isBifurxDisplayOnlyMode(state.displayedPreviewState.mode);
	
	if (!displayOnlyMode) {
		calculateRefinedCurvePoints(&refinedPoints, w, h);
	}
	else {
		refinedPoints.clear();
	}

	recordDrawSection(uiDrawSetupCount, uiDrawSetupNs);

	nvgSave(args.vg);
	const float clipInset = 0.8f; nvgScissor(args.vg, clipInset, clipInset, std::max(0.f, w - 2.f * clipInset), std::max(0.f, h - 2.f * clipInset));
	nvgSave(args.vg); nvgScissor(args.vg, plotX, 0.f, usableW, std::max(1.f, spectrumBottomY));
	auto spectrumYForDbfs = [&](float dbfs) { return rescale(clamp(dbfs, displayMinDbfs, displayMaxDbfs), displayMinDbfs, displayMaxDbfs, spectrumBottomY, spectrumTopY); };
	const float topLabelFontSize = std::max(7.f, h * 0.05f);
	nvgFontSize(args.vg, topLabelFontSize); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
	char topLabel[32]; std::snprintf(topLabel, sizeof(topLabel), "%+5.1f dBFS", displayMaxDbfs);
	const float topLabelReservedWidth = getTopLabelReservedWidth(args, topLabelFontSize);
	nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP); nvgText(args.vg, 1.5f + topLabelReservedWidth, 1.f, topLabel, nullptr);
	recordDrawSection(uiDrawBackgroundCount, uiDrawBackgroundNs);

	const BifurxColors palette = BifurxColors::get(
		module ? module->colorScheme : Bifurx::SCHEME_DEFAULT,
		module ? module->threeColorFftGradient.load(std::memory_order_relaxed) : false);
	const NVGcolor expectedPurple = palette.low;
	const NVGcolor expectedCyan = palette.high;
	const NVGcolor expectedWhite = palette.white;
	
	BifurxMarkerLayout layout;
	if (!displayOnlyMode) {
		getCachedMarkerLayout(&layout, w, h);
	}

	recordDrawSection(uiDrawExpectedCount, uiDrawExpectedNs);

	if (state.hasOverlay) {
		const bool showModuleResponse = !displayOnlyMode && module && module->showModuleResponseOverlay.load(std::memory_order_relaxed);
		const float displayOnlyShapeControl = module ? clamp(module->params[Bifurx::FM_AMT_PARAM].getValue(), -1.f, 1.f) : 0.f;
		for (int i = 0; i < kCurvePointCount - 1; ++i) {
			const float avgD = 0.5f * (state.overlayModuleDb[i] + state.overlayModuleDb[i + 1]);
			const float avgO = 0.5f * (state.overlayOutputDbfs[i] + state.overlayOutputDbfs[i + 1]), energy = levi_math::clamp01(rescale(avgO, displayMinDbfs, displayMaxDbfs, 0.f, 1.f));
			if (energy <= 0.005f) continue;
			NVGcolor fill;
			if (displayOnlyMode) {
				fill = mixColor(expectedPurple, expectedCyan, displayOnlyColorTone(energy, displayOnlyShapeControl));
			}
			else {
				const float posA = levi_math::clamp01(avgD / 18.f), negA = levi_math::clamp01(-avgD / 18.f);
				NVGcolor tint = expectedWhite; if (posA > 0.f) tint = mixColor(tint, expectedCyan, levi_math::clamp01(posA * 1.40f)); if (negA > 0.f) tint = mixColor(tint, expectedPurple, levi_math::clamp01(negA * 1.25f));
				fill = mixColor(expectedWhite, tint, 0.55f + 0.45f * energy);
			}
			fill.a = 1.f;
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, curveX[i] - 0.45f, spectrumYForDbfs(state.overlayOutputDbfs[i])); nvgLineTo(args.vg, curveX[i + 1] + 0.45f, spectrumYForDbfs(state.overlayOutputDbfs[i + 1]));
			nvgLineTo(args.vg, curveX[i + 1] + 0.45f, spectrumBottomY); nvgLineTo(args.vg, curveX[i] - 0.45f, spectrumBottomY); nvgClosePath(args.vg); nvgFillColor(args.vg, fill); nvgFill(args.vg);
		}
		if (showModuleResponse) {
			nvgBeginPath(args.vg); for (int i = 0; i < kCurvePointCount; ++i) { float y = responseYForDb(state.overlayModuleDb[i]); if (i == 0) nvgMoveTo(args.vg, curveX[i], y); else nvgLineTo(args.vg, curveX[i], y); }
			NVGcolor ml = mixColor(expectedWhite, expectedCyan, 0.35f); ml.a = 0.95f; nvgStrokeWidth(args.vg, 1.4f); nvgStrokeColor(args.vg, ml); nvgStroke(args.vg);
		}
		recordDrawSection(uiDrawOverlayCount, uiDrawOverlayNs);
	}

	auto drawRefinedCurvePath = [&]() {
		nvgBeginPath(args.vg);
		for (int i = 0; i < (int)refinedPoints.size(); ++i) {
			if (i == 0) nvgMoveTo(args.vg, w * refinedPoints[i].x01, refinedPoints[i].y);
			else nvgLineTo(args.vg, w * refinedPoints[i].x01, refinedPoints[i].y);
		}
	};
	nvgLineJoin(args.vg, NVG_ROUND);
	nvgLineCap(args.vg, NVG_ROUND);
	if (!displayOnlyMode) {
		drawRefinedCurvePath();
		nvgStrokeColor(args.vg, nvgRGBA(6, 8, 12, 210));
		nvgStrokeWidth(args.vg, 2.2f);
		nvgStroke(args.vg);
		drawRefinedCurvePath();
		nvgStrokeColor(args.vg, nvgRGBA(235, 204, 128, 244));
		nvgStrokeWidth(args.vg, 1.7f);
		nvgStroke(args.vg);
	}
	lastDrawVertexCount = uint64_t(refinedPoints.size());
	recordDrawSection(uiDrawCurveCount, uiDrawCurveNs);
	// Peak guides are foreground marker geometry. Keep them after the opaque FFT
	// fill and response curve so the spectrum cannot paint over them.
	if (!displayOnlyMode) {
		for (int i = 0; i < 2; i++) {
			if (!layout.markers[i].visible) continue;
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, spectrumBottomY); nvgLineTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker);
			nvgStrokeColor(args.vg, nvgRGBA(6, 8, 12, 210)); nvgStrokeWidth(args.vg, 2.2f); nvgStroke(args.vg);
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, spectrumBottomY); nvgLineTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker);
			nvgStrokeColor(args.vg, nvgRGBA(235, 204, 128, 244)); nvgStrokeWidth(args.vg, 1.7f); nvgStroke(args.vg);
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker + kPeakMarkerFillRadius + 0.45f); nvgLineTo(args.vg, layout.markers[i].x, layout.guideYBottom);
			nvgStrokeColor(args.vg, nvgRGBA(6, 8, 12, 210)); nvgStrokeWidth(args.vg, 2.2f); nvgStroke(args.vg);
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker + kPeakMarkerFillRadius + 0.45f); nvgLineTo(args.vg, layout.markers[i].x, layout.guideYBottom);
			nvgStrokeColor(args.vg, nvgRGBA(235, 204, 128, 244)); nvgStrokeWidth(args.vg, 1.7f); nvgStroke(args.vg);
		}
	}
	nvgRestore(args.vg);

	if (!displayOnlyMode) {
		for (int i = 0; i < 2; ++i) {
			if (!layout.markers[i].visible) continue;
			nvgBeginPath(args.vg); nvgCircle(args.vg, layout.markers[i].x, layout.markers[i].yMarker, kPeakMarkerFillRadius); nvgFillColor(args.vg, nvgRGBA(252, 255, 255, 244)); nvgFill(args.vg);
			nvgBeginPath(args.vg); nvgCircle(args.vg, layout.markers[i].x, layout.markers[i].yMarker, kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius); nvgStrokeColor(args.vg, nvgRGBA(8, 10, 14, 220)); nvgStrokeWidth(args.vg, kPeakMarkerOutlineStrokeWidth); nvgStroke(args.vg);
		}
		nvgFontSize(args.vg, layout.labelFontSize); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		for (int i = 0; i < 2; ++i) { if (!layout.markers[i].visible) continue; nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240)); nvgText(args.vg, layout.labelX[i], layout.labelY + 0.75f, layout.markers[i].label, nullptr); nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250)); nvgText(args.vg, layout.labelX[i], layout.labelY, layout.markers[i].label, nullptr); }
	}
	nvgResetScissor(args.vg); nvgRestore(args.vg);
	recordDrawSection(uiDrawMarkersCount, uiDrawMarkersNs);
	if (measurePerf) {
		lastDrawNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfDrawStart).count();
		const float drawMs = std::max(0.f, float(double(lastDrawNs) * 1e-6));
		lastDrawMsEma = (lastDrawMsEma > 0.f) ? (lastDrawMsEma + (drawMs - lastDrawMsEma) * 0.18f) : drawMs;
		if (perfLoggingActive) {
			uiDrawCount++; uiDrawNs += lastDrawNs; uiDrawMaxNs = std::max(uiDrawMaxNs, lastDrawNs);
		}
	}
}

} // namespace bifurx

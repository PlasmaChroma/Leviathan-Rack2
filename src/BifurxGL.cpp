#include "Bifurx.hpp"
#include <nanovg_gl.h>

namespace bifurx {

struct BifurxSpectrumGLWidget final : widget::OpenGlWidget, BifurxSpectrumBase {
	struct GlVertex {
		float x, y;
		float r, g, b, a;
	};

	int cachedTopLabelFontHandle = -1;
	float cachedTopLabelFontSize = NAN;
	float cachedTopLabelReservedWidth = 0.f;

	BifurxSpectrumGLWidget() : BifurxSpectrumBase() {
	}

	~BifurxSpectrumGLWidget() {
	}

	float getTopLabelReservedWidth(const DrawArgs& args, float fontSize) {
		const int fontHandle = (APP && APP->window && APP->window->uiFont) ? APP->window->uiFont->handle : -1;
		if (fontHandle == cachedTopLabelFontHandle &&
			std::isfinite(cachedTopLabelFontSize) &&
			std::fabs(cachedTopLabelFontSize - fontSize) <= 1e-5f &&
			cachedTopLabelReservedWidth > 0.f) {
			return cachedTopLabelReservedWidth;
		}

		auto measureTopLabelWidthForValue = [&](float db) {
			char sampleLabel[32];
			std::snprintf(sampleLabel, sizeof(sampleLabel), "%+5.1f dBFS", db);
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

	void step() override {
		OpenGlWidget::step();
		if (!module) return;
		syncBase();
		
		float uiFrameSec = 1.f / 60.f;
		if (APP && APP->window) {
			const float frameSec = float(APP->window->getLastFrameDuration());
			if (std::isfinite(frameSec) && frameSec > 0.f) {
				uiFrameSec = clamp(frameSec, 1.f / 240.f, 1.f / 20.f);
			}
		}
		updateAnimation(uiFrameSec);
	}

	void drawFramebuffer() override {
		if (!module || module->renderMode != Bifurx::RENDER_OPENGL) return;

		Vec fbSize = getFramebufferSize();
		glViewport(0, 0, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));
		
		// 1. CLEAR: Use pure transparent black
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		const float w = box.size.x, h = box.size.y;
		if (!(w > 0.f && h > 0.f)) return;

		// 2. PROJECTION: Ensure we use the full widget area
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, w, h, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_SCISSOR_TEST); // NanoVG might have left a scissor active

		const float padY = std::max(4.f, h * 0.035f);
		const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
		const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
		
		// Use current state (which includes animation/smoothing)
		const float displayMaxDbfs = state.displayTopDbfs;
		const float displayMinDbfs = displayMaxDbfs - kDisplayDbfsSpan;

		auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
		auto spectrumYForDbfs = [&](float dbfs) { return rescale(clamp(dbfs, displayMinDbfs, displayMaxDbfs), displayMinDbfs, displayMaxDbfs, spectrumBottomY, spectrumTopY); };

		std::vector<GlVertex> vertices;
		vertices.reserve(kCurvePointCount * 12);

		// 3. RENDER FILL (FFT Overlay)
		std::vector<GlVertex> fillTopVertices; // For softening the edge
		if (state.hasOverlay) {
			fillTopVertices.reserve(kCurvePointCount);
			for (int i = 0; i < kCurvePointCount - 1; i++) {
				const float avgD = 0.5f * (state.overlayModuleDb[i] + state.overlayModuleDb[i + 1]), avgO = 0.5f * (state.overlayOutputDbfs[i] + state.overlayOutputDbfs[i + 1]);
				const float energy = clamp01(rescale(avgO, displayMinDbfs, displayMaxDbfs, 0.f, 1.f));
				if (energy <= 0.005f) continue;
				
				float posA = clamp01(avgD / 18.f), negA = clamp01(-avgD / 18.f);
				NVGcolor expectedWhite = nvgRGB(206, 210, 216);
				NVGcolor expectedCyan = nvgRGB(28, 204, 217);
				NVGcolor expectedPurple = nvgRGB(122, 92, 255);
				NVGcolor tint = expectedWhite; 
				if (posA > 0.f) tint = mixColor(tint, expectedCyan, clamp01(posA * 1.40f)); 
				if (negA > 0.f) tint = mixColor(tint, expectedPurple, clamp01(negA * 1.25f));
				NVGcolor fill = mixColor(expectedWhite, tint, 0.55f + 0.45f * energy);
				
				float x0 = w * (float(i) / float(kCurvePointCount - 1));
				float x1 = w * (float(i + 1) / float(kCurvePointCount - 1));
				float y0 = spectrumYForDbfs(state.overlayOutputDbfs[i]);
				float y1 = spectrumYForDbfs(state.overlayOutputDbfs[i + 1]);

				// Fill triangles
				vertices.push_back({x0, y0, fill.r, fill.g, fill.b, 1.0f});
				vertices.push_back({x1, y1, fill.r, fill.g, fill.b, 1.0f});
				vertices.push_back({x0, spectrumBottomY, fill.r, fill.g, fill.b, 1.0f});
				
				vertices.push_back({x1, y1, fill.r, fill.g, fill.b, 1.0f});
				vertices.push_back({x1, spectrumBottomY, fill.r, fill.g, fill.b, 1.0f});
				vertices.push_back({x0, spectrumBottomY, fill.r, fill.g, fill.b, 1.0f});

				// Top edge line vertices for AA
				if (i == 0) fillTopVertices.push_back({x0, y0, fill.r, fill.g, fill.b, 1.0f});
				fillTopVertices.push_back({x1, y1, fill.r, fill.g, fill.b, 1.0f});
			}
		}

		// 4. RENDER CURVE (Response Line)
		std::vector<GlVertex> curveVertices;
		curveVertices.reserve(kCurvePointCount);
		NVGcolor curveColor = nvgRGBA(255, 248, 208, 244);
		for (int i = 0; i < kCurvePointCount; i++) {
			float x = w * (float(i) / float(kCurvePointCount - 1));
			float y = responseYForDb(state.curveDb[i]);
			curveVertices.push_back({x, y, curveColor.r, curveColor.g, curveColor.b, curveColor.a});
		}

		// 5. RENDER OVERLAY MODULE CURVE (Cyan Line)
		std::vector<GlVertex> cyanVertices;
		if (state.hasOverlay) {
			cyanVertices.reserve(kCurvePointCount);
			NVGcolor cyanColor = nvgRGB(28, 204, 217);
			cyanColor.a = 0.95f;
			for (int i = 0; i < kCurvePointCount; i++) {
				float x = w * (float(i) / float(kCurvePointCount - 1));
				float y = responseYForDb(state.overlayModuleDb[i]);
				cyanVertices.push_back({x, y, cyanColor.r, cyanColor.g, cyanColor.b, cyanColor.a});
			}
		}

		if (vertices.empty() && curveVertices.empty() && cyanVertices.empty()) return;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		// Draw Fill
		if (!vertices.empty()) {
			glVertexPointer(2, GL_FLOAT, sizeof(GlVertex), &vertices[0].x);
			glColorPointer(4, GL_FLOAT, sizeof(GlVertex), &vertices[0].r);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			
			// Draw softening top edge for the fill
			if (!fillTopVertices.empty()) {
				glLineWidth(1.0f);
				glVertexPointer(2, GL_FLOAT, sizeof(GlVertex), &fillTopVertices[0].x);
				glColorPointer(4, GL_FLOAT, sizeof(GlVertex), &fillTopVertices[0].r);
				glDrawArrays(GL_LINE_STRIP, 0, fillTopVertices.size());
			}
		}

		// Draw Cyan Curve
		if (!cyanVertices.empty()) {
			glLineWidth(1.8f);
			glVertexPointer(2, GL_FLOAT, sizeof(GlVertex), &cyanVertices[0].x);
			glColorPointer(4, GL_FLOAT, sizeof(GlVertex), &cyanVertices[0].r);
			glDrawArrays(GL_LINE_STRIP, 0, cyanVertices.size());
		}

		// Draw Main Curve
		if (!curveVertices.empty()) {
			glLineWidth(2.2f);
			glVertexPointer(2, GL_FLOAT, sizeof(GlVertex), &curveVertices[0].x);
			glColorPointer(4, GL_FLOAT, sizeof(GlVertex), &curveVertices[0].r);
			glDrawArrays(GL_LINE_STRIP, 0, curveVertices.size());
		}

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_LINE_SMOOTH);
	}

	void draw(const DrawArgs& args) override {
		widget::OpenGlWidget::draw(args);
	}

	void drawNanoVG(const DrawArgs& args) override {
		if (!module || module->renderMode != Bifurx::RENDER_OPENGL) return;
		if (!state.hasPreview) return;
		
		const float w = box.size.x, h = box.size.y;
		const float padY = std::max(4.f, h * 0.035f);
		const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
		const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
		const float minHz = 10.f, maxHz = std::min(20000.f, 0.46f * state.previewState.sampleRate);
		const float markerOuterRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
		const float markerBottomLaneY = spectrumBottomY - markerOuterRadius - kPeakMarkerBottomLanePadding;
		const BifurxPreviewModel model = makePreviewModel(state.previewState);
		
		auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
		auto curveYAtX01 = [&](float x01) {
			const float curveIndex = clamp(x01, 0.f, 1.f) * float(kCurvePointCount - 1);
			const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1), i1 = std::min(i0 + 1, kCurvePointCount - 1);
			return responseYForDb(mixf(state.curveDb[i0], state.curveDb[i1], curveIndex - float(i0)));
		};

		struct PeakMarker { float x = 0.f; float yCurve = 0.f; float yMarker = 0.f; float hz = 0.f; bool visible = false; char label[16] = {}; };
		auto buildMarkerAtFrequency = [&](int mIdx, float targetHz) {
			PeakMarker m; const auto anchor = displayAnchorForMarker(mIdx, targetHz, minHz, maxHz);
			const float mX = w * anchor.x01; if (mX < markerOuterRadius + kPeakMarkerEdgePadding || mX > w - markerOuterRadius - kPeakMarkerEdgePadding) { m.visible = false; return m; }
			m.x = mX; m.yCurve = curveYAtX01(anchor.x01); const float mMinY = spectrumTopY + markerOuterRadius + kPeakMarkerEdgePadding, mMaxY = spectrumBottomY - markerOuterRadius - kPeakMarkerEdgePadding;
			m.yMarker = (state.previewState.mode == 3) ? markerBottomLaneY : clamp(m.yCurve, mMinY, mMaxY);
			m.hz = std::max(anchor.hz, 1e-6f); m.visible = true; formatFrequencyLabel(m.hz, m.label, sizeof(m.label)); return m;
		};
		PeakMarker pks[2] = { buildMarkerAtFrequency(0, model.markerFreqA), buildMarkerAtFrequency(1, model.markerFreqB) };
		float lX[2] = { pks[0].x, pks[1].x }; const float lM = std::max(18.f, w * 0.08f), minLS = std::max(30.f, w * 0.18f), mnX = lM, mxX = w - lM;
		if (pks[0].visible && pks[1].visible) {
			const int li = (lX[0] <= lX[1]) ? 0 : 1, ri = 1 - li;
			float lx = clamp(lX[li], mnX, mxX), rx = clamp(lX[ri], mnX, mxX), nd = std::min(minLS, std::max(0.f, mxX - mnX)) - (rx - lx);
			if (nd > 0.f) { float ml = std::min(0.5f * nd, lx - mnX), mr = std::min(0.5f * nd, mxX - rx); lx -= ml; rx += mr; nd -= (ml + mr); if (nd > 0.f) { float el = std::min(nd, lx - mnX); lx -= el; nd -= el; } if (nd > 0.f) rx += std::min(nd, mxX - rx); }
			lX[li] = lx; lX[ri] = rx;
		} else { for (int i = 0; i < 2; ++i) if (pks[i].visible) lX[i] = clamp(lX[i], mnX, mxX); }
		const float fS = std::max(7.f, h * 0.055f), lTY = labelBandTop + 0.5f * labelBandHeight, gYB = clamp(labelBandTop + std::min(2.1f, 0.18f * labelBandHeight), labelBandTop + 0.2f, lTY - 0.5f * fS - 0.6f);
		for (int i = 0; i < 2; ++i) {
			if (!pks[i].visible) continue;
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, pks[i].x, pks[i].yMarker + kPeakMarkerFillRadius + 0.45f); nvgLineTo(args.vg, pks[i].x, gYB); nvgStrokeColor(args.vg, nvgRGBA(252, 236, 176, 170)); nvgStrokeWidth(args.vg, 1.1f); nvgStroke(args.vg);
			nvgBeginPath(args.vg); nvgCircle(args.vg, pks[i].x, pks[i].yMarker, kPeakMarkerFillRadius); nvgFillColor(args.vg, nvgRGBA(252, 255, 255, 244)); nvgFill(args.vg);
			nvgBeginPath(args.vg); nvgCircle(args.vg, pks[i].x, pks[i].yMarker, kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius); nvgStrokeColor(args.vg, nvgRGBA(8, 10, 14, 220)); nvgStrokeWidth(args.vg, kPeakMarkerOutlineStrokeWidth); nvgStroke(args.vg);
		}
		nvgFontSize(args.vg, fS); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		for (int i = 0; i < 2; ++i) { if (!pks[i].visible) continue; nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240)); nvgText(args.vg, lX[i], lTY + 0.75f, pks[i].label, nullptr); nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250)); nvgText(args.vg, lX[i], lTY, pks[i].label, nullptr); }

		// Top Label
		const float topLabelFontSize = std::max(7.f, h * 0.05f);
		nvgFontSize(args.vg, topLabelFontSize); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		char topLabel[32]; std::snprintf(topLabel, sizeof(topLabel), "%+5.1f dBFS", state.displayTopDbfs);
		const float topLabelReservedWidth = getTopLabelReservedWidth(args, topLabelFontSize);
		nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP); nvgText(args.vg, 1.5f + topLabelReservedWidth, 1.f, topLabel, nullptr);
	}
};

Widget* createGlSpectrumDisplay(Bifurx* module, math::Rect rectMm) {
	BifurxSpectrumGLWidget* w = new BifurxSpectrumGLWidget();
	w->module = module;
	w->box.pos = mm2px(rectMm.pos);
	w->box.size = mm2px(rectMm.size);
	return w;
}

} // namespace bifurx

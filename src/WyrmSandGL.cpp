#include "Wyrm.hpp"
#include "WyrmSand.hpp"

#include <array>
#include <chrono>
#include <nanovg_gl.h>

struct WyrmSandGlWidget final : widget::OpenGlWidget {
	Wyrm* module = nullptr;
	std::shared_ptr<WyrmSand> sand;
	GLuint texture = 0;
	int textureW = 0;
	int textureH = 0;
	uint64_t uploadedRevision = 0;
	double lastUiPhaseUpdateSec = -1.0;

	void advanceUiSlitherPhase() {
		if (!module) return;
		const double nowSec = system::getTime();
		if (!std::isfinite(nowSec)) return;
		if (lastUiPhaseUpdateSec < 0.0 || !std::isfinite(lastUiPhaseUpdateSec)) {
			lastUiPhaseUpdateSec = nowSec;
			return;
		}
		const float elapsed = clamp(float(nowSec - lastUiPhaseUpdateSec), 0.f, 0.25f);
		lastUiPhaseUpdateSec = nowSec;
		float phase = module->uiSlitherPhase.load(std::memory_order_relaxed);
		const float speedFactor = module->displaySlitherSpeedFactor.load(std::memory_order_relaxed);
		phase = wrap01(phase + 0.65f * speedFactor * elapsed);
		module->uiSlitherPhase.store(phase, std::memory_order_relaxed);
	}

	Vec currentLocalMousePos() const {
		if (!APP || !APP->scene || !APP->scene->rack) {
			return Vec();
		}
		const Vec widgetRackPos = const_cast<WyrmSandGlWidget*>(this)->getRelativeOffset(Vec(), APP->scene->rack);
		return APP->scene->rack->getMousePos().minus(widgetRackPos);
	}

	static int indexFromX(float x, int count, float sizeX) {
		const float inset = 2.2f;
		const float drawWidth = std::max(1.f, sizeX - 2.f * inset);
		const float dx = drawWidth / float(std::max(count, 1));
		const float column = (x - inset) / std::max(dx, 1e-6f);
		return clamp(int(std::floor(column)), 0, count - 1);
	}

	static void computeRockClearance(Vec size, float* clearance, float* phaseClearance) {
		const float maxBodyStrokePx = 4.f;
		const float maxRockStrokePx = 2.2f;
		const float pixelClearance = 0.5f * maxBodyStrokePx + 0.5f * maxRockStrokePx + 0.75f;
		const float valueClearance = (size.y > 1.f) ? (2.f * pixelClearance / size.y) : kWyrmRockClearance;
		*clearance = std::max(kWyrmRockClearance, valueClearance / std::max(kWyrmRockValueScale, 1e-4f));
		*phaseClearance = (size.x > 1.f) ? (pixelClearance / std::max(1.f, size.x - 4.4f)) : 0.f;
	}

	static float displayWaveValueAtIndex(Wyrm* module, int index, Vec size) {
		const float amount = clamp01(module->displaySlitherAmount.load(std::memory_order_relaxed));
		const float travelPhase = module->uiSlitherPhase.load(std::memory_order_relaxed);
		const float phase = (float(index) + 0.5f) / float(std::max(1, module->pointCount));
		float clearance = 0.f;
		float phaseClearance = 0.f;
		computeRockClearance(size, &clearance, &phaseClearance);
		const float baseRaw = module->getWavePoint(index);
		const float base = module->resolveAgainstRocks(baseRaw, baseRaw, phase, clearance, phaseClearance);
		const float slither = (amount > 1e-5f) ? slitherOffset(phase, travelPhase, amount) : 0.f;
		return module->resolveAgainstRocks(base, base + slither, phase, clearance, phaseClearance);
	}

	static Vec bodyPointForPhase(Wyrm* module, const std::array<float, kWyrmPointCountMax>& points, float phase, Vec size) {
		const float amount = clamp01(module->displaySlitherAmount.load(std::memory_order_relaxed));
		const float travelPhase = module->uiSlitherPhase.load(std::memory_order_relaxed);
		const float raw = catmullPeriodic(points, module->pointCount, phase);
		float clearance = 0.f;
		float phaseClearance = 0.f;
		computeRockClearance(size, &clearance, &phaseClearance);
		const float base = module->resolveAgainstRocks(raw, raw, phase, clearance, phaseClearance);
		const float slither = (amount > 1e-5f) ? slitherOffset(phase, travelPhase, amount) : 0.f;
		const float value = module->resolveAgainstRocks(base, base + slither, phase, clearance, phaseClearance);
		const float x = 2.2f + phase * std::max(1.f, size.x - 4.4f);
		const float y = (0.5f - 0.5f * clamp(value, -1.f, 1.f)) * size.y;
		return Vec(x, y);
	}

	void drawWaveColumnsGl(Vec size) {
		if (!module || module->pointCount <= 0 || module->sandViewEnabled.load(std::memory_order_relaxed)) {
			return;
		}
		const int count = module->pointCount;
		const float inset = 2.2f;
		const float drawWidth = std::max(1.f, size.x - 2.f * inset);
		const float dx = drawWidth / float(std::max(1, count));
		const float graphColumnWidth = std::min(2.0f, dx);
		const float midY = 0.5f * size.y;
		const NVGcolor c = nvgRGBA(34, 27, 70, 196);
		glColor4f(c.r, c.g, c.b, c.a);
		glBegin(GL_QUADS);
		for (int i = 0; i < count; ++i) {
			const float v = displayWaveValueAtIndex(module, i, size);
			const float y = (0.5f - 0.5f * v) * size.y;
			const float x = inset + (float(i) + 0.5f) * dx;
			const float yTop = std::min(midY, y);
			const float yBottom = std::max(midY, y);
			const float x0 = x - 0.5f * graphColumnWidth;
			const float x1 = x + 0.5f * graphColumnWidth;
			glVertex2f(x0, yTop);
			glVertex2f(x1, yTop);
			glVertex2f(x1, yBottom);
			glVertex2f(x0, yBottom);
		}
		glEnd();
	}

	void drawHoverGuidesGl(Vec size) {
		if (!module || module->pointCount <= 0) {
			return;
		}
		const Vec mouseLocal = currentLocalMousePos();
		const bool mouseInside = (mouseLocal.x >= 0.f && mouseLocal.x <= size.x && mouseLocal.y >= 0.f && mouseLocal.y <= size.y);
		if (!mouseInside) {
			return;
		}

		const int count = module->pointCount;
		const int hoverIdx = indexFromX(mouseLocal.x, count, size.x);
		const float inset = 2.2f;
		const float drawWidth = std::max(1.f, size.x - 2.f * inset);
		const float dx = drawWidth / float(std::max(count, 1));
		const float x0 = inset + float(hoverIdx) * dx;
		const float x1 = x0 + dx;
		const float graphColumnWidth = std::min(2.0f, dx);
		const float guideX = 0.5f * (x0 + x1);
		const float guideY = clamp(mouseLocal.y, 0.f, size.y);

		const NVGcolor band = nvgRGBA(28, 204, 217, 72);
		glColor4f(band.r, band.g, band.b, band.a);
		glBegin(GL_QUADS);
		glVertex2f(x0, 0.f);
		glVertex2f(x1, 0.f);
		glVertex2f(x1, size.y);
		glVertex2f(x0, size.y);
		glEnd();

		const NVGcolor col = nvgRGBA(28, 204, 217, 238);
		glColor4f(col.r, col.g, col.b, col.a);
		glBegin(GL_QUADS);
		glVertex2f(guideX - 0.5f * graphColumnWidth, 0.f);
		glVertex2f(guideX + 0.5f * graphColumnWidth, 0.f);
		glVertex2f(guideX + 0.5f * graphColumnWidth, size.y);
		glVertex2f(guideX - 0.5f * graphColumnWidth, size.y);
		glEnd();

		const NVGcolor line = nvgRGBA(186, 154, 92, 96);
		glColor4f(line.r, line.g, line.b, line.a);
		const float lineH = 1.4f;
		const float y0 = clamp(guideY - 0.5f * lineH, 0.f, size.y);
		const float y1 = clamp(guideY + 0.5f * lineH, 0.f, size.y);
		glBegin(GL_QUADS);
		glVertex2f(0.f, y0);
		glVertex2f(size.x, y0);
		glVertex2f(size.x, y1);
		glVertex2f(0.f, y1);
		glEnd();
	}

	static void drawBodyStrip(const std::vector<Vec>& pts, float widthPx, const NVGcolor& color) {
		if (pts.size() < 2) return;
		auto safeNormalize = [](const Vec& v, const Vec& fallback) -> Vec {
			const float len = std::sqrt(v.x * v.x + v.y * v.y);
			if (len < 1e-4f) return fallback;
			return v.div(len);
		};
		auto segmentNormal = [&](size_t a, size_t b) -> Vec {
			const Vec t = safeNormalize(pts[b].minus(pts[a]), Vec(1.f, 0.f));
			return Vec(-t.y, t.x);
		};
		auto joinOffset = [&](size_t i, float halfW) -> Vec {
			if (i == 0) return segmentNormal(0, 1).mult(halfW);
			if (i + 1 >= pts.size()) return segmentNormal(pts.size() - 2, pts.size() - 1).mult(halfW);
			const Vec tPrev = safeNormalize(pts[i].minus(pts[i - 1]), Vec(1.f, 0.f));
			const Vec tNext = safeNormalize(pts[i + 1].minus(pts[i]), tPrev);
			const Vec nPrev = Vec(-tPrev.y, tPrev.x);
			const Vec nNext = Vec(-tNext.y, tNext.x);
			const float turnDot = tPrev.x * tNext.x + tPrev.y * tNext.y;
			const Vec j = safeNormalize(nPrev.plus(nNext), nNext);
			const float denom = std::max(0.80f, std::abs(j.x * nNext.x + j.y * nNext.y));
			const float miter = std::min(1.18f, 1.f / denom);
			const Vec miterOff = j.mult(halfW * miter);
			const Vec bevelOff = nNext.mult(halfW);
			const float t = clamp((turnDot - 0.12f) / 0.18f, 0.f, 1.f);
			return bevelOff.mult(1.f - t).plus(miterOff.mult(t));
		};
		const float halfW = 0.5f * widthPx;
		std::vector<Vec> off(pts.size());
		for (size_t i = 0; i < pts.size(); ++i) off[i] = joinOffset(i, halfW);

		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_QUADS);
		for (size_t i = 0; i + 1 < pts.size(); ++i) {
			const Vec aL = pts[i].minus(off[i]);
			const Vec aR = pts[i].plus(off[i]);
			const Vec bL = pts[i + 1].minus(off[i + 1]);
			const Vec bR = pts[i + 1].plus(off[i + 1]);
			glVertex2f(aL.x, aL.y);
			glVertex2f(aR.x, aR.y);
			glVertex2f(bR.x, bR.y);
			glVertex2f(bL.x, bL.y);
		}
		glEnd();
	}

	static void drawBodyStripFeather(const std::vector<Vec>& pts, float widthPx, float featherPx, const NVGcolor& color, float edgeAlphaScale) {
		if (pts.size() < 2 || featherPx <= 1e-4f) return;
		auto safeNormalize = [](const Vec& v, const Vec& fallback) -> Vec {
			const float len = std::sqrt(v.x * v.x + v.y * v.y);
			if (len < 1e-4f) return fallback;
			return v.div(len);
		};
		auto segmentNormal = [&](size_t a, size_t b) -> Vec {
			const Vec t = safeNormalize(pts[b].minus(pts[a]), Vec(1.f, 0.f));
			return Vec(-t.y, t.x);
		};
		auto joinOffset = [&](size_t i, float halfW) -> Vec {
			if (i == 0) return segmentNormal(0, 1).mult(halfW);
			if (i + 1 >= pts.size()) return segmentNormal(pts.size() - 2, pts.size() - 1).mult(halfW);
			const Vec tPrev = safeNormalize(pts[i].minus(pts[i - 1]), Vec(1.f, 0.f));
			const Vec tNext = safeNormalize(pts[i + 1].minus(pts[i]), tPrev);
			const Vec nPrev = Vec(-tPrev.y, tPrev.x);
			const Vec nNext = Vec(-tNext.y, tNext.x);
			const float turnDot = tPrev.x * tNext.x + tPrev.y * tNext.y;
			const Vec j = safeNormalize(nPrev.plus(nNext), nNext);
			const float denom = std::max(0.80f, std::abs(j.x * nNext.x + j.y * nNext.y));
			const float miter = std::min(1.18f, 1.f / denom);
			const Vec miterOff = j.mult(halfW * miter);
			const Vec bevelOff = nNext.mult(halfW);
			const float t = clamp((turnDot - 0.12f) / 0.18f, 0.f, 1.f);
			return bevelOff.mult(1.f - t).plus(miterOff.mult(t));
		};
		const float edgeA = clamp(color.a * edgeAlphaScale, 0.f, 1.f);
		const float innerW = 0.5f * widthPx;
		const float outerW = innerW + featherPx;
		std::vector<Vec> innerOff(pts.size());
		std::vector<Vec> outerOff(pts.size());
		for (size_t i = 0; i < pts.size(); ++i) {
			innerOff[i] = joinOffset(i, innerW);
			outerOff[i] = joinOffset(i, outerW);
		}

		// Left feather quads.
		glBegin(GL_QUADS);
		for (size_t i = 0; i + 1 < pts.size(); ++i) {
			const Vec aI = pts[i].minus(innerOff[i]);
			const Vec bI = pts[i + 1].minus(innerOff[i + 1]);
			const Vec aO = pts[i].minus(outerOff[i]);
			const Vec bO = pts[i + 1].minus(outerOff[i + 1]);
			glColor4f(color.r, color.g, color.b, edgeA);
			glVertex2f(aI.x, aI.y);
			glColor4f(color.r, color.g, color.b, edgeA);
			glVertex2f(bI.x, bI.y);
			glColor4f(color.r, color.g, color.b, 0.f);
			glVertex2f(bO.x, bO.y);
			glColor4f(color.r, color.g, color.b, 0.f);
			glVertex2f(aO.x, aO.y);
		}
		glEnd();

		// Right feather quads.
		glBegin(GL_QUADS);
		for (size_t i = 0; i + 1 < pts.size(); ++i) {
			const Vec aI = pts[i].plus(innerOff[i]);
			const Vec bI = pts[i + 1].plus(innerOff[i + 1]);
			const Vec aO = pts[i].plus(outerOff[i]);
			const Vec bO = pts[i + 1].plus(outerOff[i + 1]);
			glColor4f(color.r, color.g, color.b, edgeA);
			glVertex2f(aI.x, aI.y);
			glColor4f(color.r, color.g, color.b, edgeA);
			glVertex2f(bI.x, bI.y);
			glColor4f(color.r, color.g, color.b, 0.f);
			glVertex2f(bO.x, bO.y);
			glColor4f(color.r, color.g, color.b, 0.f);
			glVertex2f(aO.x, aO.y);
		}
		glEnd();
	}

	void drawBodyGl(Vec size) {
		if (!module || module->pointCount < 2) return;
		const int baseSampleCount = std::max(module->pointCount, std::min(768, std::max(128, module->pointCount * 4)));
		const float zoom = std::max(1.f, getAbsoluteZoom());
		const float drawWidth = std::max(1.f, size.x - 4.4f);
		// Keep roughly <= 1px phase step in screen-space at high zoom to avoid visible strip aliasing.
		const int zoomSampleTarget = int(std::ceil(drawWidth * zoom));
		const int sampleCount = clamp(std::max(baseSampleCount, zoomSampleTarget), module->pointCount, 4096);
		std::array<float, kWyrmPointCountMax> bodyPoints {};
		for (int i = 0; i < module->pointCount; ++i) {
			bodyPoints[i] = module->getWavePoint(i);
		}
		std::vector<Vec> samples;
		samples.reserve(size_t(sampleCount));
		for (int i = 0; i < sampleCount; ++i) {
			const float phase = (float(i) + 0.5f) / float(sampleCount);
			samples.push_back(bodyPointForPhase(module, bodyPoints, phase, size));
		}

		drawBodyStrip(samples, 3.75f, nvgRGBAf(74.f / 255.f, 54.f / 255.f, 24.f / 255.f, 205.f / 255.f));
		drawBodyStripFeather(samples, 3.75f, 0.66f, nvgRGBAf(74.f / 255.f, 54.f / 255.f, 24.f / 255.f, 205.f / 255.f), 0.40f);
		drawBodyStripFeather(samples, 3.75f, 1.35f, nvgRGBAf(74.f / 255.f, 54.f / 255.f, 24.f / 255.f, 205.f / 255.f), 0.11f);
		drawBodyStrip(samples, 2.45f, nvgRGBAf(167.f / 255.f, 132.f / 255.f, 72.f / 255.f, 230.f / 255.f));
		drawBodyStripFeather(samples, 2.45f, 0.52f, nvgRGBAf(167.f / 255.f, 132.f / 255.f, 72.f / 255.f, 230.f / 255.f), 0.32f);
		drawBodyStrip(samples, 1.08f, nvgRGBAf(246.f / 255.f, 215.f / 255.f, 136.f / 255.f, 225.f / 255.f));
		drawBodyStripFeather(samples, 1.08f, 0.38f, nvgRGBAf(246.f / 255.f, 215.f / 255.f, 136.f / 255.f, 225.f / 255.f), 0.24f);
	}

	~WyrmSandGlWidget() override {
		if (texture != 0) {
			glDeleteTextures(1, &texture);
			texture = 0;
		}
	}

	void step() override {
		if (!module) {
			return;
		}
		const int mode = module->renderMode.load(std::memory_order_relaxed);
		const bool renderGl = (mode == WYRM_RENDER_OPENGL || mode == WYRM_RENDER_OPENGL_SHDR);
		visible = renderGl;
		if (!visible) {
			module->perfSandGlUs.store(0.f, std::memory_order_relaxed);
			return;
		}
		advanceUiSlitherPhase();
		setDirty();
		OpenGlWidget::step();
	}

	void drawFramebuffer() override {
		using PerfClock = std::chrono::steady_clock;
		const PerfClock::time_point perfStart = PerfClock::now();
		Vec fbSize = getFramebufferSize();
		glViewport(0, 0, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (!module || !sand) {
			if (module) {
				module->perfSandGlUs.store(0.f, std::memory_order_relaxed);
			}
			return;
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0, box.size.x, box.size.y, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		if (module->sandViewEnabled.load(std::memory_order_relaxed)) {
			const int detailSetting = module->sandDetail.load(std::memory_order_relaxed);
			sand->ensureImageRaster(box.size, detailSetting);
			const unsigned char* pixels = sand->imageData();
			const int imageW = sand->imageWidth();
			const int imageH = sand->imageHeight();
			if (pixels && imageW > 0 && imageH > 0) {
				if (texture == 0) {
					glGenTextures(1, &texture);
					textureW = 0;
					textureH = 0;
					uploadedRevision = 0;
				}
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				const uint64_t revision = sand->imageDataRevision();
				if (textureW != imageW || textureH != imageH) {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageW, imageH, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
					textureW = imageW;
					textureH = imageH;
					uploadedRevision = revision;
				}
				else if (uploadedRevision != revision) {
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imageW, imageH, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
					uploadedRevision = revision;
				}
				glEnable(GL_TEXTURE_2D);
				glColor4f(1.f, 1.f, 1.f, 1.f);
				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
				glTexCoord2f(1.f, 0.f); glVertex2f(box.size.x, 0.f);
				glTexCoord2f(0.f, 1.f); glVertex2f(0.f, box.size.y);
				glTexCoord2f(1.f, 1.f); glVertex2f(box.size.x, box.size.y);
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}
		}

		drawHoverGuidesGl(box.size);
		drawWaveColumnsGl(box.size);
		drawBodyGl(box.size);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		const float sandGlUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
			PerfClock::now() - perfStart).count()) * 0.001f;
		module->perfSandGlUs.store(sandGlUs, std::memory_order_relaxed);
	}
};

Widget* createWyrmSandGlWidget(Wyrm* module, std::shared_ptr<WyrmSand> sandState) {
	auto* w = new WyrmSandGlWidget();
	w->module = module;
	w->sand = sandState ? sandState : std::make_shared<WyrmSand>();
	return w;
}

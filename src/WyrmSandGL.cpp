#include "Wyrm.hpp"
#include "WyrmSand.hpp"
#include "GlLifecycleUtils.hpp"

#include <array>
#include <chrono>
#include <string>
#include <nanovg_gl.h>

struct WyrmSandGlWidget final : widget::OpenGlWidget {
	Wyrm* module = nullptr;
	std::shared_ptr<WyrmSand> sand;
	GLuint texture = 0;
	int textureW = 0;
	int textureH = 0;
	uint64_t uploadedRevision = 0;
	GLuint waveColumnTexture = 0;
	int waveColumnTextureW = 0;
	int waveColumnTextureH = 0;
	int waveColumnTextureCount = -1;
	double lastUiPhaseUpdateSec = -1.0;
	GLuint bodyShaderProgram = 0;
	GLint bodyShaderSoftnessLoc = -1;
	bool bodyShaderInitAttempted = false;
	bool bodyShaderReady = false;
	GLuint bodyRtFbo = 0;
	GLuint bodyRtTex = 0;
	int bodyRtW = 0;
	int bodyRtH = 0;
	std::vector<Vec> cachedBodySamples;
	uint32_t cachedBodyWaveVersion = 0;
	int cachedBodyRockStateIndex = -1;
	int cachedBodyPointCount = -1;
	int cachedBodySampleCount = -1;
	Vec cachedBodySize = Vec(-1.f, -1.f);
	float cachedBodySlitherPhase = -1.f;
	float cachedBodySlitherAmount = -1.f;
	bool cachedBodySamplesValid = false;

	void resetTextureState() {
		texture = 0;
		textureW = 0;
		textureH = 0;
		uploadedRevision = 0;
	}

	void resetWaveColumnTextureState() {
		waveColumnTexture = 0;
		waveColumnTextureW = 0;
		waveColumnTextureH = 0;
		waveColumnTextureCount = -1;
	}

	void resetBodyShaderState() {
		bodyShaderProgram = 0;
		bodyShaderSoftnessLoc = -1;
		bodyShaderInitAttempted = false;
		bodyShaderReady = false;
	}

	void resetBodyRenderTargetState() {
		bodyRtFbo = 0;
		bodyRtTex = 0;
		bodyRtW = 0;
		bodyRtH = 0;
	}

	void validateGlResourcesForCurrentContext() {
		if (texture != 0 && !glIsTexture(texture)) {
			resetTextureState();
		}
		if (waveColumnTexture != 0 && !glIsTexture(waveColumnTexture)) {
			resetWaveColumnTextureState();
		}
		if (bodyShaderReady && (bodyShaderProgram == 0 || !glIsProgram(bodyShaderProgram))) {
			resetBodyShaderState();
		}
		if ((bodyRtTex != 0 || bodyRtFbo != 0) &&
			!gl_lifecycle::isValidTextureFramebufferPair(bodyRtTex, bodyRtFbo)) {
			resetBodyRenderTargetState();
		}
	}

	static GLuint compileShader(GLenum type, const char* src) {
		GLuint shader = glCreateShader(type);
		if (!shader) return 0;
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);
		GLint ok = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
		if (ok != GL_TRUE) {
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}

	void ensureBodyShader() {
		if (bodyShaderInitAttempted) return;
		bodyShaderInitAttempted = true;
		static const char* kVs = R"GLSL(
			#version 120
			varying vec4 vColor;
			varying vec2 vUv;
			void main() {
				gl_Position = ftransform();
				vColor = gl_Color;
				vUv = gl_MultiTexCoord0.xy;
			}
		)GLSL";
		static const char* kFs = R"GLSL(
			#version 120
			varying vec4 vColor;
			varying vec2 vUv;
			uniform float uSoftness;
			void main() {
				float edge = abs(vUv.x * 2.0 - 1.0);
				float inner = clamp(1.0 - uSoftness, 0.0, 1.0);
				float aa = 1.0 - smoothstep(inner, 1.0, edge);
				gl_FragColor = vec4(vColor.rgb, vColor.a * aa);
			}
		)GLSL";
		GLuint vs = compileShader(GL_VERTEX_SHADER, kVs);
		GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFs);
		if (!vs || !fs) {
			if (vs) glDeleteShader(vs);
			if (fs) glDeleteShader(fs);
			return;
		}
		bodyShaderProgram = glCreateProgram();
		if (!bodyShaderProgram) {
			glDeleteShader(vs);
			glDeleteShader(fs);
			return;
		}
		glAttachShader(bodyShaderProgram, vs);
		glAttachShader(bodyShaderProgram, fs);
		glLinkProgram(bodyShaderProgram);
		glDeleteShader(vs);
		glDeleteShader(fs);
		GLint linkOk = GL_FALSE;
		glGetProgramiv(bodyShaderProgram, GL_LINK_STATUS, &linkOk);
		if (linkOk != GL_TRUE) {
			glDeleteProgram(bodyShaderProgram);
			bodyShaderProgram = 0;
			return;
		}
		bodyShaderSoftnessLoc = glGetUniformLocation(bodyShaderProgram, "uSoftness");
		bodyShaderReady = (bodyShaderSoftnessLoc >= 0);
		if (!bodyShaderReady) {
			glDeleteProgram(bodyShaderProgram);
			bodyShaderProgram = 0;
		}
	}

	void ensureBodyRenderTarget(int w, int h) {
		w = std::max(1, w);
		h = std::max(1, h);
		if (bodyRtTex != 0 && bodyRtW == w && bodyRtH == h && bodyRtFbo != 0) {
			return;
		}
		GLint previousFbo = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFbo);
		if (bodyRtFbo != 0) {
			glDeleteFramebuffers(1, &bodyRtFbo);
			bodyRtFbo = 0;
		}
		if (bodyRtTex != 0) {
			glDeleteTextures(1, &bodyRtTex);
			bodyRtTex = 0;
		}
		glGenTextures(1, &bodyRtTex);
		glBindTexture(GL_TEXTURE_2D, bodyRtTex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(1, &bodyRtFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, bodyRtFbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bodyRtTex, 0);
		const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		glBindFramebuffer(GL_FRAMEBUFFER, GLuint(previousFbo));
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			glDeleteFramebuffers(1, &bodyRtFbo);
			bodyRtFbo = 0;
			glDeleteTextures(1, &bodyRtTex);
			bodyRtTex = 0;
			bodyRtW = 0;
			bodyRtH = 0;
			return;
		}
		bodyRtW = w;
		bodyRtH = h;
	}

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
		phase = levi_math::wrap01(phase + 0.65f * speedFactor * elapsed);
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

	static NVGcolor mixColor(NVGcolor a, NVGcolor b, float t) {
		t = levi_math::clamp01(t);
		return nvgRGBAf(
			a.r + (b.r - a.r) * t,
			a.g + (b.g - a.g) * t,
			a.b + (b.b - a.b) * t,
			a.a + (b.a - a.a) * t
		);
	}

	static void compositeOver(const NVGcolor& src, float* dst) {
		const float outA = src.a + dst[3] * (1.f - src.a);
		if (outA <= 1e-6f) {
			dst[0] = dst[1] = dst[2] = dst[3] = 0.f;
			return;
		}
		const float outR = src.r * src.a + dst[0] * dst[3] * (1.f - src.a);
		const float outG = src.g * src.a + dst[1] * dst[3] * (1.f - src.a);
		const float outB = src.b * src.a + dst[2] * dst[3] * (1.f - src.a);
		dst[0] = outR / outA;
		dst[1] = outG / outA;
		dst[2] = outB / outA;
		dst[3] = outA;
	}

	void ensureWaveColumnTexture(Vec size, int count) {
		const int w = std::max(1, int(std::ceil(size.x)));
		const int h = std::max(1, int(std::ceil(size.y)));
		count = std::max(1, count);
		if (waveColumnTexture != 0 &&
			waveColumnTextureW == w &&
			waveColumnTextureH == h &&
			waveColumnTextureCount == count) {
			return;
		}

		const float inset = 2.2f;
		const float drawWidth = std::max(1.f, size.x - 2.f * inset);
		const float dx = drawWidth / float(count);
		const float graphColumnWidth = std::min(2.0f, dx);
		const float midY = 0.5f * size.y;
		const NVGcolor posNear = nvgRGBA(28, 204, 217, 46);
		const NVGcolor posFar = nvgRGBA(42, 228, 255, 152);
		const NVGcolor negNear = nvgRGBA(115, 72, 224, 50);
		const NVGcolor negFar = nvgRGBA(150, 92, 255, 162);
		const NVGcolor posShade = nvgRGBA(0, 56, 72, 132);
		const NVGcolor negShade = nvgRGBA(40, 24, 112, 92);
		std::vector<unsigned char> pixels(size_t(w) * size_t(h) * 4u, 0u);

		for (int py = 0; py < h; ++py) {
			const float y = std::min(size.y, float(py) + 0.5f);
			const bool positive = y < midY;
			const float t = positive
				? levi_math::clamp01((midY - y) / std::max(midY, 1.f))
				: levi_math::clamp01((y - midY) / std::max(size.y - midY, 1.f));
			const NVGcolor base = positive ? mixColor(posNear, posFar, t) : mixColor(negNear, negFar, t);
			for (int px = 0; px < w; ++px) {
				const float x = std::min(size.x, float(px) + 0.5f);
				const float columnF = (x - inset) / std::max(dx, 1e-6f);
				const int column = int(std::floor(columnF));
				if (column < 0 || column >= count) {
					continue;
				}
				const float centerX = inset + (float(column) + 0.5f) * dx;
				if (std::fabs(x - centerX) > 0.5f * graphColumnWidth) {
					continue;
				}

				float out[4] = {0.f, 0.f, 0.f, 0.f};
				compositeOver(base, out);
				if ((column & 1) != 0) {
					compositeOver(positive ? posShade : negShade, out);
				}
				const size_t offset = (size_t(py) * size_t(w) + size_t(px)) * 4u;
				pixels[offset + 0u] = uint8_t(clamp(int(std::lround(out[0] * 255.f)), 0, 255));
				pixels[offset + 1u] = uint8_t(clamp(int(std::lround(out[1] * 255.f)), 0, 255));
				pixels[offset + 2u] = uint8_t(clamp(int(std::lround(out[2] * 255.f)), 0, 255));
				pixels[offset + 3u] = uint8_t(clamp(int(std::lround(out[3] * 255.f)), 0, 255));
			}
		}

		if (waveColumnTexture == 0) {
			glGenTextures(1, &waveColumnTexture);
		}
		glBindTexture(GL_TEXTURE_2D, waveColumnTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
		waveColumnTextureW = w;
		waveColumnTextureH = h;
		waveColumnTextureCount = count;
	}

	static float displayWaveValueAtIndex(Wyrm* module, int index, Vec size) {
		const float amount = levi_math::clamp01(module->displaySlitherAmount.load(std::memory_order_relaxed));
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
		const float amount = levi_math::clamp01(module->displaySlitherAmount.load(std::memory_order_relaxed));
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
		const float midY = 0.5f * size.y;
		ensureWaveColumnTexture(size, count);
		if (waveColumnTexture == 0) {
			return;
		}
		glBindTexture(GL_TEXTURE_2D, waveColumnTexture);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.f, 1.f, 1.f, 1.f);
		glBegin(GL_QUADS);
		for (int i = 0; i < count; ++i) {
			const float v = displayWaveValueAtIndex(module, i, size);
			const float y = (0.5f - 0.5f * v) * size.y;
			const float x = inset + (float(i) + 0.5f) * dx;
			const float yTop = std::min(midY, y);
			const float yBottom = std::max(midY, y);
			const float x0 = std::max(0.f, x - 0.5f * dx);
			const float x1 = std::min(size.x, x + 0.5f * dx);
			const float u0 = x0 / std::max(size.x, 1.f);
			const float u1 = x1 / std::max(size.x, 1.f);
			const float v0 = yTop / std::max(size.y, 1.f);
			const float v1 = yBottom / std::max(size.y, 1.f);
			glTexCoord2f(u0, v0);
			glVertex2f(x0, yTop);
			glTexCoord2f(u1, v0);
			glVertex2f(x1, yTop);
			glTexCoord2f(u1, v1);
			glVertex2f(x1, yBottom);
			glTexCoord2f(u0, v1);
			glVertex2f(x0, yBottom);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
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

	static std::vector<Vec> computeBodyJoinOffsets(const std::vector<Vec>& pts, float halfW) {
		std::vector<Vec> off;
		if (pts.size() < 2 || halfW <= 1e-5f) {
			return off;
		}
		off.resize(pts.size());
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
			Vec off = bevelOff.mult(1.f - t).plus(miterOff.mult(t));
			// Corner safety: if blended join points against either adjacent normal, force bevel.
			if (off.x * nPrev.x + off.y * nPrev.y <= 0.02f * halfW ||
				off.x * nNext.x + off.y * nNext.y <= 0.02f * halfW) {
				off = bevelOff;
			}
			const float prevLen = pts[i].minus(pts[i - 1]).norm();
			const float nextLen = pts[i + 1].minus(pts[i]).norm();
			const float localLen = std::max(1e-4f, std::min(prevLen, nextLen));
			const float offLen = off.norm();
			const float maxOff = std::max(halfW, 0.42f * localLen);
			if (offLen > maxOff && offLen > 1e-4f) {
				off = off.mult(maxOff / offLen);
			}
			return off;
		};
		for (size_t i = 0; i < pts.size(); ++i) off[i] = joinOffset(i, halfW);
		return off;
	}

	static void drawBodyStripWithOffsets(const std::vector<Vec>& pts, const std::vector<Vec>& off, float halfW, const NVGcolor& color, bool shaderPath) {
		if (pts.size() < 2 || off.size() != pts.size()) return;
		glColor4f(color.r, color.g, color.b, color.a);
		if (shaderPath) {
			glBegin(GL_TRIANGLE_STRIP);
			for (size_t i = 0; i < pts.size(); ++i) {
				const Vec l = pts[i].minus(off[i]);
				const Vec r = pts[i].plus(off[i]);
				glTexCoord2f(0.f, 0.f);
				glVertex2f(l.x, l.y);
				glTexCoord2f(1.f, 0.f);
				glVertex2f(r.x, r.y);
			}
			glEnd();
			return;
		}

		glBegin(GL_QUADS);
		for (size_t i = 0; i + 1 < pts.size(); ++i) {
			Vec seg = pts[i + 1].minus(pts[i]);
			const float segLen = seg.norm();
			if (segLen < 1e-4f) continue;
			const Vec t = seg.div(segLen);
			const float extend = std::min(0.55f, 0.22f * halfW + 0.10f);
			const Vec ext = t.mult(extend);

			const Vec aL = pts[i].minus(off[i]);
			const Vec aR = pts[i].plus(off[i]);
			const Vec bL = pts[i + 1].minus(off[i + 1]);
			const Vec bR = pts[i + 1].plus(off[i + 1]);
			const Vec aLe = aL.minus(ext);
			const Vec aRe = aR.minus(ext);
			const Vec bLe = bL.plus(ext);
			const Vec bRe = bR.plus(ext);
			if (shaderPath) glTexCoord2f(0.f, 0.f);
			glVertex2f(aLe.x, aLe.y);
			if (shaderPath) glTexCoord2f(1.f, 0.f);
			glVertex2f(aRe.x, aRe.y);
			if (shaderPath) glTexCoord2f(1.f, 1.f);
			glVertex2f(bRe.x, bRe.y);
			if (shaderPath) glTexCoord2f(0.f, 1.f);
			glVertex2f(bLe.x, bLe.y);
			}
			glEnd();
		}
	static void drawBodyStrip(const std::vector<Vec>& pts, float widthPx, const NVGcolor& color, bool shaderPath) {
		if (pts.size() < 2) return;
		const float halfW = 0.5f * widthPx;
		const std::vector<Vec> off = computeBodyJoinOffsets(pts, halfW);
		drawBodyStripWithOffsets(pts, off, halfW, color, shaderPath);
	}

	static void drawBodyStripFeatherWithOffsets(const std::vector<Vec>& pts,
	                                            const std::vector<Vec>& innerOff,
	                                            const std::vector<Vec>& outerOff,
	                                            float innerW,
	                                            const NVGcolor& color,
	                                            float edgeAlphaScale,
	                                            bool shaderPath) {
		if (pts.size() < 2 || innerOff.size() != pts.size() || outerOff.size() != pts.size() || innerW <= 1e-5f) return;
		const float edgeA = clamp(color.a * edgeAlphaScale, 0.f, 1.f);

		// Left feather quads.
		glBegin(GL_QUADS);
		for (size_t i = 0; i + 1 < pts.size(); ++i) {
			Vec seg = pts[i + 1].minus(pts[i]);
			const float segLen = seg.norm();
			if (segLen < 1e-4f) continue;
			const Vec t = seg.div(segLen);
			const float extend = std::min(0.50f, 0.18f * innerW + 0.08f);
			const Vec ext = t.mult(extend);

			const Vec aI = pts[i].minus(innerOff[i]);
			const Vec bI = pts[i + 1].minus(innerOff[i + 1]);
			const Vec aO = pts[i].minus(outerOff[i]);
			const Vec bO = pts[i + 1].minus(outerOff[i + 1]);
			const Vec aIe = aI.minus(ext);
			const Vec bIe = bI.plus(ext);
			const Vec aOe = aO.minus(ext);
			const Vec bOe = bO.plus(ext);
			glColor4f(color.r, color.g, color.b, edgeA);
			if (shaderPath) glTexCoord2f(0.f, 0.f);
			glVertex2f(aIe.x, aIe.y);
			glColor4f(color.r, color.g, color.b, edgeA);
			if (shaderPath) glTexCoord2f(0.f, 1.f);
			glVertex2f(bIe.x, bIe.y);
			glColor4f(color.r, color.g, color.b, 0.f);
			if (shaderPath) glTexCoord2f(1.f, 1.f);
			glVertex2f(bOe.x, bOe.y);
			glColor4f(color.r, color.g, color.b, 0.f);
			if (shaderPath) glTexCoord2f(1.f, 0.f);
			glVertex2f(aOe.x, aOe.y);
		}
		glEnd();

		// Right feather quads.
		glBegin(GL_QUADS);
		for (size_t i = 0; i + 1 < pts.size(); ++i) {
			Vec seg = pts[i + 1].minus(pts[i]);
			const float segLen = seg.norm();
			if (segLen < 1e-4f) continue;
			const Vec t = seg.div(segLen);
			const float extend = std::min(0.50f, 0.18f * innerW + 0.08f);
			const Vec ext = t.mult(extend);

			const Vec aI = pts[i].plus(innerOff[i]);
			const Vec bI = pts[i + 1].plus(innerOff[i + 1]);
			const Vec aO = pts[i].plus(outerOff[i]);
			const Vec bO = pts[i + 1].plus(outerOff[i + 1]);
			const Vec aIe = aI.minus(ext);
			const Vec bIe = bI.plus(ext);
			const Vec aOe = aO.minus(ext);
			const Vec bOe = bO.plus(ext);
			glColor4f(color.r, color.g, color.b, edgeA);
			if (shaderPath) glTexCoord2f(0.f, 0.f);
			glVertex2f(aIe.x, aIe.y);
			glColor4f(color.r, color.g, color.b, edgeA);
			if (shaderPath) glTexCoord2f(0.f, 1.f);
			glVertex2f(bIe.x, bIe.y);
			glColor4f(color.r, color.g, color.b, 0.f);
			if (shaderPath) glTexCoord2f(1.f, 1.f);
			glVertex2f(bOe.x, bOe.y);
			glColor4f(color.r, color.g, color.b, 0.f);
			if (shaderPath) glTexCoord2f(1.f, 0.f);
			glVertex2f(aOe.x, aOe.y);
			}
			glEnd();
	}

	static void drawBodyStripFeather(const std::vector<Vec>& pts, float widthPx, float featherPx, const NVGcolor& color, float edgeAlphaScale, bool shaderPath) {
		if (pts.size() < 2 || featherPx <= 1e-4f) return;
		const float innerW = 0.5f * widthPx;
		const float outerW = innerW + featherPx;
		const std::vector<Vec> innerOff = computeBodyJoinOffsets(pts, innerW);
		const std::vector<Vec> outerOff = computeBodyJoinOffsets(pts, outerW);
		drawBodyStripFeatherWithOffsets(pts, innerOff, outerOff, innerW, color, edgeAlphaScale, shaderPath);
	}

	const std::vector<Vec>& ensureBodySamples(Vec size, int sampleCount) {
		if (!module || module->pointCount < 2 || sampleCount <= 0) {
			cachedBodySamples.clear();
			cachedBodySamplesValid = false;
			return cachedBodySamples;
		}
		const uint32_t waveVersion = module->waveVersion.load(std::memory_order_acquire);
		const int rockStateIndex = module->activeRockStateIndex.load(std::memory_order_acquire);
		const float slitherAmount = levi_math::clamp01(module->displaySlitherAmount.load(std::memory_order_relaxed));
		const float slitherPhase = module->uiSlitherPhase.load(std::memory_order_relaxed);
		const bool cacheValid =
			cachedBodySamplesValid &&
			cachedBodyWaveVersion == waveVersion &&
			cachedBodyRockStateIndex == rockStateIndex &&
			cachedBodyPointCount == module->pointCount &&
			cachedBodySampleCount == sampleCount &&
			std::fabs(cachedBodySize.x - size.x) <= 1e-4f &&
			std::fabs(cachedBodySize.y - size.y) <= 1e-4f &&
			std::fabs(cachedBodySlitherPhase - slitherPhase) <= 1e-6f &&
			std::fabs(cachedBodySlitherAmount - slitherAmount) <= 1e-6f;
		if (cacheValid) {
			module->perfBodySampleCacheHits.fetch_add(1u, std::memory_order_relaxed);
			return cachedBodySamples;
		}
		module->perfBodySampleCacheMisses.fetch_add(1u, std::memory_order_relaxed);
		std::array<float, kWyrmPointCountMax> bodyPoints {};
		for (int i = 0; i < module->pointCount; ++i) {
			bodyPoints[i] = module->getWavePoint(i);
		}
		cachedBodySamples.clear();
		cachedBodySamples.reserve(size_t(sampleCount));
		for (int i = 0; i < sampleCount; ++i) {
			const float phase = (float(i) + 0.5f) / float(sampleCount);
			cachedBodySamples.push_back(bodyPointForPhase(module, bodyPoints, phase, size));
		}
		cachedBodyWaveVersion = waveVersion;
		cachedBodyRockStateIndex = rockStateIndex;
		cachedBodyPointCount = module->pointCount;
		cachedBodySampleCount = sampleCount;
		cachedBodySize = size;
		cachedBodySlitherPhase = slitherPhase;
		cachedBodySlitherAmount = slitherAmount;
		cachedBodySamplesValid = true;
		return cachedBodySamples;
	}

	void drawBodyGl(Vec size, bool shaderPath, bool includeBase, bool includeGlow) {
		if (!module || module->pointCount < 2) return;
		const int baseSampleCount = std::max(module->pointCount, std::min(1536, std::max(256, module->pointCount * 8)));
		const float zoom = std::max(1.f, getAbsoluteZoom());
		const float drawWidth = std::max(1.f, size.x - 4.4f);
		// Keep roughly <= 1px phase step in screen-space at high zoom to avoid visible strip aliasing.
		const float shdrSampleScale = shaderPath ? 2.05f : 1.75f;
		const int zoomSampleTarget = int(std::ceil(drawWidth * zoom * shdrSampleScale));
		const int sampleCount = clamp(std::max(baseSampleCount, zoomSampleTarget), module->pointCount, 8192);
		const std::vector<Vec>& samples = ensureBodySamples(size, sampleCount);
		if (samples.size() < 2u) return;
		std::vector<std::pair<float, std::vector<Vec>>> offsetCache;
		auto getOffsets = [&](float halfW) -> const std::vector<Vec>& {
			for (auto& e : offsetCache) {
				if (std::fabs(e.first - halfW) <= 1e-6f) {
					return e.second;
				}
			}
			offsetCache.emplace_back(halfW, computeBodyJoinOffsets(samples, halfW));
			return offsetCache.back().second;
		};

		if (includeBase) {
			if (shaderPath) {
				// SHDR: rely on fragment AA softness and avoid extra feather geometry passes.
				drawBodyStrip(samples, 4.20f, nvgRGBAf(74.f / 255.f, 54.f / 255.f, 24.f / 255.f, 184.f / 255.f), true);
				drawBodyStrip(samples, 2.54f, nvgRGBAf(167.f / 255.f, 132.f / 255.f, 72.f / 255.f, 212.f / 255.f), true);
				drawBodyStrip(samples, 1.12f, nvgRGBAf(246.f / 255.f, 215.f / 255.f, 136.f / 255.f, 224.f / 255.f), true);
			}
			else {
				const float wOuter = 3.75f;
				const float hOuter = 0.5f * wOuter;
				const std::vector<Vec>& oOuter = getOffsets(hOuter);
				drawBodyStripWithOffsets(samples, oOuter, hOuter, nvgRGBAf(74.f / 255.f, 54.f / 255.f, 24.f / 255.f, 205.f / 255.f), false);
				drawBodyStripFeatherWithOffsets(samples, oOuter, getOffsets(hOuter + 0.66f), hOuter, nvgRGBAf(74.f / 255.f, 54.f / 255.f, 24.f / 255.f, 205.f / 255.f), 0.40f, false);
				drawBodyStripFeatherWithOffsets(samples, oOuter, getOffsets(hOuter + 1.35f), hOuter, nvgRGBAf(74.f / 255.f, 54.f / 255.f, 24.f / 255.f, 205.f / 255.f), 0.11f, false);
				const float wMid = 2.45f;
				const float hMid = 0.5f * wMid;
				const std::vector<Vec>& oMid = getOffsets(hMid);
				drawBodyStripWithOffsets(samples, oMid, hMid, nvgRGBAf(167.f / 255.f, 132.f / 255.f, 72.f / 255.f, 230.f / 255.f), false);
				drawBodyStripFeatherWithOffsets(samples, oMid, getOffsets(hMid + 0.52f), hMid, nvgRGBAf(167.f / 255.f, 132.f / 255.f, 72.f / 255.f, 230.f / 255.f), 0.32f, false);
				const float wInner = 1.08f;
				const float hInner = 0.5f * wInner;
				const std::vector<Vec>& oInner = getOffsets(hInner);
				drawBodyStripWithOffsets(samples, oInner, hInner, nvgRGBAf(246.f / 255.f, 215.f / 255.f, 136.f / 255.f, 225.f / 255.f), false);
				drawBodyStripFeatherWithOffsets(samples, oInner, getOffsets(hInner + 0.38f), hInner, nvgRGBAf(246.f / 255.f, 215.f / 255.f, 136.f / 255.f, 225.f / 255.f), 0.24f, false);
			}
		}
		if (includeGlow) {
			const float wGlow = 5.10f;
			const float hGlow = 0.5f * wGlow;
			const std::vector<Vec>& oGlow = getOffsets(hGlow);
			drawBodyStripWithOffsets(samples, oGlow, hGlow, nvgRGBAf(186.f / 255.f, 154.f / 255.f, 92.f / 255.f, 44.f / 255.f), true);
			drawBodyStripFeatherWithOffsets(samples, oGlow, getOffsets(hGlow + 1.45f), hGlow, nvgRGBAf(186.f / 255.f, 154.f / 255.f, 92.f / 255.f, 44.f / 255.f), 0.34f, false);
		}
	}

	void drawBodyMaskGl(Vec size) {
		if (!module || module->pointCount < 2) return;
		const int baseSampleCount = std::max(module->pointCount, std::min(1536, std::max(256, module->pointCount * 8)));
		const float zoom = std::max(1.f, getAbsoluteZoom());
		const float drawWidth = std::max(1.f, size.x - 4.4f);
		const int zoomSampleTarget = int(std::ceil(drawWidth * zoom * 1.75f));
		const int sampleCount = clamp(std::max(baseSampleCount, zoomSampleTarget), module->pointCount, 8192);
		const std::vector<Vec>& samples = ensureBodySamples(size, sampleCount);
		if (samples.size() < 2u) return;
		std::vector<std::pair<float, std::vector<Vec>>> offsetCache;
		auto getOffsets = [&](float halfW) -> const std::vector<Vec>& {
			for (auto& e : offsetCache) {
				if (std::fabs(e.first - halfW) <= 1e-6f) {
					return e.second;
				}
			}
			offsetCache.emplace_back(halfW, computeBodyJoinOffsets(samples, halfW));
			return offsetCache.back().second;
		};

		// Phase 5B refined: soft alpha mask (core + graded fringe), not binary coverage.
		// This preserves contour detail better when compositing back to panel space.
		const float w0 = 3.15f;
		const float h0 = 0.5f * w0;
		const std::vector<Vec>& o0 = getOffsets(h0);
		drawBodyStripWithOffsets(samples, o0, h0, nvgRGBAf(1.f, 1.f, 1.f, 0.88f), false);
		const float w1 = 1.85f;
		const float h1 = 0.5f * w1;
		const std::vector<Vec>& o1 = getOffsets(h1);
		drawBodyStripWithOffsets(samples, o1, h1, nvgRGBAf(1.f, 1.f, 1.f, 0.74f), false);
		const float w2 = 1.18f;
		const float h2 = 0.5f * w2;
		drawBodyStripWithOffsets(samples, getOffsets(h2), h2, nvgRGBAf(1.f, 1.f, 1.f, 1.00f), false);
		drawBodyStripFeatherWithOffsets(samples, o0, getOffsets(h0 + 0.82f), h0, nvgRGBAf(1.f, 1.f, 1.f, 0.44f), 0.78f, false);
		drawBodyStripFeatherWithOffsets(samples, o1, getOffsets(h1 + 0.52f), h1, nvgRGBAf(1.f, 1.f, 1.f, 0.40f), 0.72f, false);
		drawBodyStripFeatherWithOffsets(samples, o0, getOffsets(h0 + 1.45f), h0, nvgRGBAf(1.f, 1.f, 1.f, 0.20f), 0.58f, false);
	}

	~WyrmSandGlWidget() override {
		// DAW plugin editors can destroy/recreate their GL context around the
		// Rack UI. Avoid driver calls from widget teardown; resources are
		// reclaimed by the editor/context owner.
		resetBodyRenderTargetState();
		resetBodyShaderState();
		resetWaveColumnTextureState();
		resetTextureState();
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
		const bool measurePerf = module && isDragonKingDebugEnabled();
		const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		Vec fbSize = getFramebufferSize();
		glViewport(0, 0, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));
		if (isExtraGlValidationEnabled()) {
			validateGlResourcesForCurrentContext();
		}
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
		const int mode = module->renderMode.load(std::memory_order_relaxed);
		const bool useShdr = (mode == WYRM_RENDER_OPENGL_SHDR);
		if (useShdr) {
			ensureBodyShader();
		}

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

		drawWaveColumnsGl(box.size);
		drawHoverGuidesGl(box.size);
		const bool shaderPath = useShdr && bodyShaderReady;
		// SHDR now draws direct shader-softened strips. Keep RT path disabled.
		const bool useBodyRt = false;
		// Legacy RT sizing retained for quick rollback.
		const float rtScale = useShdr ? 1.25f : 1.0f;
		const int rtW = std::max(1, int(std::lround(box.size.x * rtScale)));
		const int rtH = std::max(1, int(std::lround(box.size.y * rtScale)));
		if (useBodyRt) {
			ensureBodyRenderTarget(rtW, rtH);
		}
		if (useBodyRt && bodyRtFbo != 0 && bodyRtTex != 0) {
			GLint previousFbo = 0;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFbo);
			const GLboolean scissorWasEnabled = glIsEnabled(GL_SCISSOR_TEST);
			const GLboolean cullWasEnabled = glIsEnabled(GL_CULL_FACE);
			if (scissorWasEnabled) glDisable(GL_SCISSOR_TEST);
			if (cullWasEnabled) glDisable(GL_CULL_FACE);

			glBindFramebuffer(GL_FRAMEBUFFER, bodyRtFbo);
			glViewport(0, 0, bodyRtW, bodyRtH);
			glClearColor(0.f, 0.f, 0.f, 0.f);
			glClear(GL_COLOR_BUFFER_BIT);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0.0, bodyRtW, bodyRtH, 0.0, -1.0, 1.0);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1.f, 1.f, 1.f, 1.f);
			drawBodyMaskGl(Vec(float(bodyRtW), float(bodyRtH)));
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glBindFramebuffer(GL_FRAMEBUFFER, GLuint(previousFbo));
			glViewport(0, 0, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, bodyRtTex);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Composite color layers from the mask in main framebuffer.
			glColor4f(74.f / 255.f, 54.f / 255.f, 24.f / 255.f, 0.68f);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.f, 1.f); glVertex2f(0.f, 0.f);
			glTexCoord2f(1.f, 1.f); glVertex2f(box.size.x, 0.f);
			glTexCoord2f(0.f, 0.f); glVertex2f(0.f, box.size.y);
			glTexCoord2f(1.f, 0.f); glVertex2f(box.size.x, box.size.y);
			glEnd();

			glColor4f(167.f / 255.f, 132.f / 255.f, 72.f / 255.f, 0.66f);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.f, 1.f); glVertex2f(0.f, 0.f);
			glTexCoord2f(1.f, 1.f); glVertex2f(box.size.x, 0.f);
			glTexCoord2f(0.f, 0.f); glVertex2f(0.f, box.size.y);
			glTexCoord2f(1.f, 0.f); glVertex2f(box.size.x, box.size.y);
			glEnd();

			glColor4f(246.f / 255.f, 215.f / 255.f, 136.f / 255.f, 0.72f);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.f, 1.f); glVertex2f(0.f, 0.f);
			glTexCoord2f(1.f, 1.f); glVertex2f(box.size.x, 0.f);
			glTexCoord2f(0.f, 0.f); glVertex2f(0.f, box.size.y);
			glTexCoord2f(1.f, 0.f); glVertex2f(box.size.x, box.size.y);
			glEnd();
			if (useShdr) {
				// SHDR polish taps.
				glColor4f(246.f / 255.f, 215.f / 255.f, 136.f / 255.f, 0.13f);
				const float dx = 0.6f / std::max(1.f, box.size.x);
				const float dy = 0.6f / std::max(1.f, box.size.y);
				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0.f + dx, 1.f - dy); glVertex2f(0.f, 0.f);
				glTexCoord2f(1.f + dx, 1.f - dy); glVertex2f(box.size.x, 0.f);
				glTexCoord2f(0.f + dx, 0.f - dy); glVertex2f(0.f, box.size.y);
				glTexCoord2f(1.f + dx, 0.f - dy); glVertex2f(box.size.x, box.size.y);
				glEnd();
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (scissorWasEnabled) glEnable(GL_SCISSOR_TEST);
			if (cullWasEnabled) glEnable(GL_CULL_FACE);
		}
		else {
			if (shaderPath) {
				glUseProgram(bodyShaderProgram);
				glUniform1f(bodyShaderSoftnessLoc, 0.205f);
			}
			drawBodyGl(box.size, shaderPath, true, false);
			if (shaderPath) {
				glUseProgram(0);
			}
		}

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		if (measurePerf) {
			const float sandGlUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - perfStart).count()) * 0.001f;
			module->perfSandGlUs.store(sandGlUs, std::memory_order_relaxed);
		}
	}
};

Widget* createWyrmSandGlWidget(Wyrm* module, std::shared_ptr<WyrmSand> sandState) {
	auto* w = new WyrmSandGlWidget();
	w->module = module;
	w->sand = sandState ? sandState : std::make_shared<WyrmSand>();
	return w;
}

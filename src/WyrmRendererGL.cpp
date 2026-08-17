#include "Wyrm.hpp"
#include "WyrmRenderGeometry.hpp"

#include <array>
#include <chrono>
#include <string>
#include <nanovg_gl.h>

struct WyrmGlRendererWidget final : widget::OpenGlWidget {
	struct BodyStripVertex {
		float x;
		float y;
		float u;
		float v;
	};

	Wyrm* module = nullptr;
	std::shared_ptr<wyrm_render::DisplayGeometryCache> geometryCache;
	GLuint waveColumnTexture = 0;
	int waveColumnTextureW = 0;
	int waveColumnTextureH = 0;
	int waveColumnTextureCount = -1;
	GLuint bodyShaderProgram = 0;
	GLint bodyShaderSoftnessLoc = -1;
	GLint bodyShaderMiddleRatioLoc = -1;
	GLint bodyShaderCoreRatioLoc = -1;
	GLint bodyShaderOuterColorLoc = -1;
	GLint bodyShaderMiddleColorLoc = -1;
	GLint bodyShaderCoreColorLoc = -1;
	bool bodyShaderInitAttempted = false;
	bool bodyShaderReady = false;
	bool redrawStateInitialized = false;
	int lastRenderMode = -1;
	uint32_t lastWaveVersion = 0;
	int lastRockStateIndex = -1;
	int lastPointCount = -1;
	bool lastEnvelopeMode = false;
	float lastSlitherPhase = -1.f;
	float lastSlitherAmount = -1.f;
	Vec lastDrawSize = Vec(-1.f, -1.f);
	float lastAbsoluteZoom = -1.f;
	uint64_t bodyStripGeometryRevision = 0;
	bool bodyStripShaderPath = false;
	std::vector<BodyStripVertex> shaderBodyStripVertices;
	std::array<std::vector<BodyStripVertex>, 3> fallbackBodyStripVertices;

	void resetWaveColumnTextureState() {
		waveColumnTexture = 0;
		waveColumnTextureW = 0;
		waveColumnTextureH = 0;
		waveColumnTextureCount = -1;
	}

	void resetBodyShaderState() {
		bodyShaderProgram = 0;
		bodyShaderSoftnessLoc = -1;
		bodyShaderMiddleRatioLoc = -1;
		bodyShaderCoreRatioLoc = -1;
		bodyShaderOuterColorLoc = -1;
		bodyShaderMiddleColorLoc = -1;
		bodyShaderCoreColorLoc = -1;
		bodyShaderInitAttempted = false;
		bodyShaderReady = false;
	}

	void validateGlResourcesForCurrentContext() {
		if (waveColumnTexture != 0 && !glIsTexture(waveColumnTexture)) {
			resetWaveColumnTextureState();
		}
		if (bodyShaderReady && (bodyShaderProgram == 0 || !glIsProgram(bodyShaderProgram))) {
			resetBodyShaderState();
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
			varying float vSide;
			void main() {
				gl_Position = ftransform();
				vSide = gl_MultiTexCoord0.x * 2.0 - 1.0;
			}
		)GLSL";
		static const char* kFs = R"GLSL(
			#version 120
			varying float vSide;
			uniform float uSoftness;
			uniform float uMiddleRatio;
			uniform float uCoreRatio;
			uniform vec4 uOuterColor;
			uniform vec4 uMiddleColor;
			uniform vec4 uCoreColor;
			vec4 over(vec4 dst, vec4 src) {
				float outA = src.a + dst.a * (1.0 - src.a);
				vec3 outPremul = src.rgb * src.a + dst.rgb * dst.a * (1.0 - src.a);
				return vec4(outA > 0.00001 ? outPremul / outA : vec3(0.0), outA);
			}
			void main() {
				float d = abs(vSide);
				float outerMask = 1.0 - smoothstep(1.0 - uSoftness, 1.0, d);
				float middleSoftness = uSoftness * uMiddleRatio;
				float coreSoftness = uSoftness * uCoreRatio;
				float middleMask = 1.0 - smoothstep(uMiddleRatio - middleSoftness, uMiddleRatio, d);
				float coreMask = 1.0 - smoothstep(uCoreRatio - coreSoftness, uCoreRatio, d);
				vec4 color = vec4(uOuterColor.rgb, uOuterColor.a * outerMask);
				color = over(color, vec4(uMiddleColor.rgb, uMiddleColor.a * middleMask));
				color = over(color, vec4(uCoreColor.rgb, uCoreColor.a * coreMask));
				gl_FragColor = color;
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
		bodyShaderMiddleRatioLoc = glGetUniformLocation(bodyShaderProgram, "uMiddleRatio");
		bodyShaderCoreRatioLoc = glGetUniformLocation(bodyShaderProgram, "uCoreRatio");
		bodyShaderOuterColorLoc = glGetUniformLocation(bodyShaderProgram, "uOuterColor");
		bodyShaderMiddleColorLoc = glGetUniformLocation(bodyShaderProgram, "uMiddleColor");
		bodyShaderCoreColorLoc = glGetUniformLocation(bodyShaderProgram, "uCoreColor");
		bodyShaderReady = bodyShaderSoftnessLoc >= 0
			&& bodyShaderMiddleRatioLoc >= 0 && bodyShaderCoreRatioLoc >= 0
			&& bodyShaderOuterColorLoc >= 0 && bodyShaderMiddleColorLoc >= 0
			&& bodyShaderCoreColorLoc >= 0;
		if (!bodyShaderReady) {
			glDeleteProgram(bodyShaderProgram);
			bodyShaderProgram = 0;
		}
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

	void drawWaveColumnsGl(Vec size, bool shaderPath) {
		if (!module || module->pointCount <= 0) {
			return;
		}
		const int count = module->pointCount;
		if (!geometryCache) {
			geometryCache = std::make_shared<wyrm_render::DisplayGeometryCache>();
		}
		geometryCache->ensure(module, size, bodySampleCountForSize(size, shaderPath),
			wyrm_render::DisplayGeometryRequirement::PointsOnly);
		const std::vector<Vec>& body = geometryCache->points;
		if (body.size() < 2u) {
			return;
		}
		auto displayValueAtPhase = [&](float phase) {
			const float sampleIndex = clamp(phase, 0.f, 1.f) * float(body.size()) - 0.5f;
			float y = body.front().y;
			if (sampleIndex >= float(body.size() - 1u)) {
				y = body.back().y;
			}
			else if (sampleIndex > 0.f) {
				const int i0 = clamp(int(std::floor(sampleIndex)), 0, int(body.size()) - 2);
				const float t = sampleIndex - float(i0);
				y = body[size_t(i0)].y + (body[size_t(i0 + 1)].y - body[size_t(i0)].y) * t;
			}
			return clamp(1.f - 2.f * y / std::max(size.y, 1.f), -1.f, 1.f);
		};
		const float inset = 2.2f;
		const float drawWidth = std::max(1.f, size.x - 2.f * inset);
		const float dx = drawWidth / float(std::max(1, count));
		const float midY = 0.5f * size.y;
		const bool envelopeVisual = module->envelopeMode.load(std::memory_order_relaxed);
		ensureWaveColumnTexture(size, count);
		if (waveColumnTexture == 0) {
			return;
		}
		glBindTexture(GL_TEXTURE_2D, waveColumnTexture);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.f, 1.f, 1.f, 1.f);
		glBegin(GL_QUADS);
		for (int i = 0; i < count; ++i) {
			const float phase = (float(i) + 0.5f) / float(std::max(1, count));
			const float v = displayValueAtPhase(phase);
			const float y = (0.5f - 0.5f * v) * size.y;
			const float x = inset + (float(i) + 0.5f) * dx;
			const float yTop = envelopeVisual ? y : std::min(midY, y);
			const float yBottom = envelopeVisual ? size.y : std::max(midY, y);
			const float x0 = std::max(0.f, x - 0.5f * dx);
			const float x1 = std::min(size.x, x + 0.5f * dx);
			const float u0 = x0 / std::max(size.x, 1.f);
			const float u1 = x1 / std::max(size.x, 1.f);
			const float textureScaleY = envelopeVisual ? 0.5f : 1.f;
			const float v0 = textureScaleY * yTop / std::max(size.y, 1.f);
			const float v1 = textureScaleY * yBottom / std::max(size.y, 1.f);
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
			// A single strip cannot represent a true reversal without overlapping
			// itself. Keep that exceptional case bounded; all ordinary bends use a
			// width-preserving miter so the body does not pinch at the vertex.
			if (turnDot < -0.82f) {
				return nNext.mult(halfW);
			}
			const Vec j = safeNormalize(nPrev.plus(nNext), nNext);
			// Limit the miter to 1.6x half-width. This retains substantially more
			// thickness than the former bevel blend without producing needle-like
			// corners at acute authored turns.
			const float denom = std::max(0.625f, std::abs(j.x * nNext.x + j.y * nNext.y));
			return j.mult(halfW / denom);
		};
		for (size_t i = 0; i < pts.size(); ++i) off[i] = joinOffset(i, halfW);
		return off;
	}

	int bodySampleCountForSize(Vec size, bool shaderPath) {
		return module
			? wyrm_render::glBodySampleCount(
				size, module->pointCount, getAbsoluteZoom(), shaderPath)
			: 128;
	}

	void drawBodyGl(Vec size, bool shaderPath, bool includeBase, bool includeGlow) {
		if (!module || module->pointCount < 2) return;
		const wyrm_render::BodyMaterial& material = wyrm_render::bodyMaterial();
		const int sampleCount = bodySampleCountForSize(size, shaderPath);
		if (!geometryCache) {
			geometryCache = std::make_shared<wyrm_render::DisplayGeometryCache>();
		}
		geometryCache->ensure(module, size, sampleCount,
			wyrm_render::DisplayGeometryRequirement::PointsOnly);
		const std::vector<Vec>& samples = geometryCache->points;
		if (samples.size() < 2u) return;
		if (bodyStripGeometryRevision != geometryCache->revision
			|| bodyStripShaderPath != shaderPath
			|| (shaderPath && shaderBodyStripVertices.size() != samples.size() * 2u)
			|| (!shaderPath && fallbackBodyStripVertices[0].size() != samples.size() * 2u)) {
			if (shaderPath) {
				const float halfW = 0.5f * material.layers[0].widthPx;
				const std::vector<Vec> offsets = computeBodyJoinOffsets(samples, halfW);
				shaderBodyStripVertices.resize(samples.size() * 2u);
				for (size_t i = 0; i < samples.size(); ++i) {
					const Vec left = samples[i].minus(offsets[i]);
					const Vec right = samples[i].plus(offsets[i]);
					shaderBodyStripVertices[i * 2u] = BodyStripVertex {left.x, left.y, 0.f, 0.f};
					shaderBodyStripVertices[i * 2u + 1u] = BodyStripVertex {right.x, right.y, 1.f, 0.f};
				}
			}
			else {
				for (size_t layer = 0; layer < fallbackBodyStripVertices.size(); ++layer) {
					const float halfW = 0.5f * material.layers[layer].widthPx;
					const std::vector<Vec> offsets = computeBodyJoinOffsets(samples, halfW);
					std::vector<BodyStripVertex>& vertices = fallbackBodyStripVertices[layer];
				vertices.resize(samples.size() * 2u);
				for (size_t i = 0; i < samples.size(); ++i) {
					const Vec left = samples[i].minus(offsets[i]);
					const Vec right = samples[i].plus(offsets[i]);
					vertices[i * 2u] = BodyStripVertex {left.x, left.y, 0.f, 0.f};
					vertices[i * 2u + 1u] = BodyStripVertex {right.x, right.y, 1.f, 0.f};
				}
				}
			}
			bodyStripGeometryRevision = geometryCache->revision;
			bodyStripShaderPath = shaderPath;
		}
		auto drawCachedStrip = [&](const std::vector<BodyStripVertex>& vertices,
		                           const wyrm_render::BodyLayerMaterial& layer,
		                           bool provideSide) {
			if (vertices.empty()) return;
			glColor4ub(layer.r, layer.g, layer.b, layer.a);
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, sizeof(BodyStripVertex), &vertices[0].x);
			if (provideSide) {
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, sizeof(BodyStripVertex), &vertices[0].u);
			}
			glDrawArrays(GL_TRIANGLE_STRIP, 0, GLsizei(vertices.size()));
			if (provideSide) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
		};

		if (includeBase) {
			if (shaderPath) {
				drawCachedStrip(shaderBodyStripVertices, material.layers[0], true);
			}
			else {
				for (size_t layer = 0; layer < fallbackBodyStripVertices.size(); ++layer) {
					drawCachedStrip(fallbackBodyStripVertices[layer], material.layers[layer], false);
				}
			}
		}
		(void) includeGlow;
	}

	~WyrmGlRendererWidget() override {
		// DAW plugin editors can destroy/recreate their GL context around the
		// Rack UI. Avoid driver calls from widget teardown; resources are
		// reclaimed by the editor/context owner.
		resetBodyShaderState();
		resetWaveColumnTextureState();
	}

	void step() override {
		if (!module) {
			return;
		}
		const int mode = module->renderMode.load(std::memory_order_relaxed);
		const bool renderGl = (mode == WYRM_RENDER_OPENGL || mode == WYRM_RENDER_OPENGL_SHDR);
		visible = renderGl;
		if (!visible) {
			redrawStateInitialized = false;
			module->perfWyrmGlUs.store(0.f, std::memory_order_relaxed);
			return;
		}
		const uint32_t waveVersion = module->waveVersion.load(std::memory_order_acquire);
		const int rockStateIndex = module->activeRockStateIndex.load(std::memory_order_acquire);
		const int pointCount = module->pointCount;
		const bool envelopeMode = module->envelopeMode.load(std::memory_order_relaxed);
		const float slitherAmount = levi_math::clamp01(
			module->displaySlitherAmount.load(std::memory_order_relaxed));
		const float slitherPhase = module->uiSlitherPhase.load(std::memory_order_relaxed);
		const float absoluteZoom = std::max(1.f, getAbsoluteZoom());

		bool dirty = !redrawStateInitialized;
		dirty = dirty || mode != lastRenderMode;
		dirty = dirty || waveVersion != lastWaveVersion;
		dirty = dirty || rockStateIndex != lastRockStateIndex;
		dirty = dirty || pointCount != lastPointCount;
		dirty = dirty || envelopeMode != lastEnvelopeMode;
		dirty = dirty || std::fabs(box.size.x - lastDrawSize.x) > 1e-4f;
		dirty = dirty || std::fabs(box.size.y - lastDrawSize.y) > 1e-4f;
		dirty = dirty || std::fabs(absoluteZoom - lastAbsoluteZoom) > 1e-4f;
		dirty = dirty || std::fabs(slitherAmount - lastSlitherAmount) > 1e-5f;
		dirty = dirty || (slitherAmount > 1e-5f
			&& std::fabs(slitherPhase - lastSlitherPhase) > 1e-6f);

		lastRenderMode = mode;
		lastWaveVersion = waveVersion;
		lastRockStateIndex = rockStateIndex;
		lastPointCount = pointCount;
		lastEnvelopeMode = envelopeMode;
		lastSlitherAmount = slitherAmount;
		lastSlitherPhase = slitherPhase;
		lastDrawSize = box.size;
		lastAbsoluteZoom = absoluteZoom;
		redrawStateInitialized = true;

		if (dirty) {
			setDirty();
		}
		// OpenGlWidget::step() deliberately redraws every frame. Use the cached
		// framebuffer behavior now that all live GL invalidation is explicit above.
		widget::FramebufferWidget::step();
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

		if (!module) {
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
		const bool shaderPath = useShdr && bodyShaderReady;

		drawWaveColumnsGl(box.size, shaderPath);
		if (shaderPath) {
			const wyrm_render::BodyMaterial& material = wyrm_render::bodyMaterial();
			auto setLayerColor = [](GLint location, const wyrm_render::BodyLayerMaterial& layer) {
				glUniform4f(location, layer.r / 255.f, layer.g / 255.f, layer.b / 255.f, layer.a / 255.f);
			};
			glUseProgram(bodyShaderProgram);
			glUniform1f(bodyShaderSoftnessLoc, material.edgeSoftness);
			glUniform1f(bodyShaderMiddleRatioLoc, material.layers[1].widthPx / material.layers[0].widthPx);
			glUniform1f(bodyShaderCoreRatioLoc, material.layers[2].widthPx / material.layers[0].widthPx);
			setLayerColor(bodyShaderOuterColorLoc, material.layers[0]);
			setLayerColor(bodyShaderMiddleColorLoc, material.layers[1]);
			setLayerColor(bodyShaderCoreColorLoc, material.layers[2]);
		}
		drawBodyGl(box.size, shaderPath, true, false);
		if (shaderPath) {
			glUseProgram(0);
		}

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		if (measurePerf) {
			const float wyrmGlUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - perfStart).count()) * 0.001f;
			module->perfWyrmGlUs.store(wyrmGlUs, std::memory_order_relaxed);
		}
	}
};

Widget* createWyrmGlRendererWidget(Wyrm* module) {
	return createWyrmGlRendererWidget(
		module,
		std::make_shared<wyrm_render::DisplayGeometryCache>());
}

Widget* createWyrmGlRendererWidget(
	Wyrm* module,
	std::shared_ptr<wyrm_render::DisplayGeometryCache> geometryCache) {
	auto* w = new WyrmGlRendererWidget();
	w->module = module;
	w->geometryCache = geometryCache
		? std::move(geometryCache)
		: std::make_shared<wyrm_render::DisplayGeometryCache>();
	return w;
}

#include "Wyrm.hpp"
#include "WyrmRenderGeometry.hpp"
#include "visual/AdaptiveGlSurface.hpp"

#include <array>
#include <chrono>
#include <string>
#include <nanovg_gl.h>

namespace {

// Retain the fullscreen body as a compile-time visual/performance oracle while
// evaluating the exact same shader over a conservative disjoint tile domain.
constexpr bool kWyrmUseConservativeBodyTiles = true;
constexpr float kWyrmBodyTileWidthPx = 16.f;
constexpr float kWyrmBodyTileRasterGuardPx = 1.f;
constexpr int kWyrmMaxBodySegmentRadius = 16;

} // namespace

struct WyrmGlRendererWidget final : widget::OpenGlWidget {
	struct BodyStripVertex {
		float x;
		float y;
		float u;
		float v;
	};
	struct BodyTile {
		float x0;
		float y0;
		float x1;
		float y1;
	};

	Wyrm* module = nullptr;
	std::shared_ptr<wyrm_render::DisplayGeometryCache> geometryCache;
	GLuint waveColumnTexture = 0;
	int waveColumnTextureW = 0;
	int waveColumnTextureH = 0;
	int waveColumnTextureCount = -1;
	bool waveColumnTextureEnvelope = false;
	GLuint curveTexture = 0;
	int curveTextureCount = 0;
	uint64_t curveTextureRevision = 0;
	GLuint waveShaderProgram = 0;
	GLint waveShaderCurveLoc = -1;
	GLint waveShaderCurveCountLoc = -1;
	GLint waveShaderPointCountLoc = -1;
	GLint waveShaderInsetLoc = -1;
	GLint waveShaderInvHeightLoc = -1;
	GLint waveShaderEnvelopeLoc = -1;
	bool waveShaderInitAttempted = false;
	bool waveShaderReady = false;
	GLuint bodyShaderProgram = 0;
	GLint bodyShaderSoftnessLoc = -1;
	GLint bodyShaderMiddleRatioLoc = -1;
	GLint bodyShaderCoreRatioLoc = -1;
	GLint bodyShaderOuterColorLoc = -1;
	GLint bodyShaderMiddleColorLoc = -1;
	GLint bodyShaderCoreColorLoc = -1;
	GLint bodyShaderCurveLoc = -1;
	GLint bodyShaderCurveCountLoc = -1;
	GLint bodyShaderInsetLoc = -1;
	GLint bodyShaderInvSizeLoc = -1;
	GLint bodyShaderOuterWidthLoc = -1;
	bool bodyShaderInitAttempted = false;
	bool bodyShaderReady = false;
	int bodyShaderSegmentRadius = 0;
	bool redrawStateInitialized = false;
	int lastRenderMode = -1;
	uint32_t lastWaveVersion = 0;
	int lastRockStateIndex = -1;
	int lastPointCount = -1;
	bool lastEnvelopeMode = false;
	float lastSlitherPhase = -1.f;
	float lastSlitherAmount = -1.f;
	Vec lastDrawSize = Vec(-1.f, -1.f);
	uint64_t bodyStripGeometryRevision = 0;
	bool bodyStripShaderPath = false;
	std::array<std::vector<BodyStripVertex>, 3> fallbackBodyStripVertices;
	uint64_t bodyTileGeometryRevision = 0;
	Vec bodyTileSize = Vec(-1.f, -1.f);
	std::vector<BodyTile> bodyTiles;
	std::vector<float> bodyTileMinY;
	std::vector<float> bodyTileMaxY;
	std::vector<uint8_t> bodyTileActive;
	float bodyTileDomainFraction = 1.f;
	int activeBodySegmentCount = 0;
	visual_assets::AdaptiveGlSurface compactSurface;
	visual_assets::AdaptiveGlSurface expandedSurface;
	NVGcontext* rendererVg = nullptr;
	bool fixedSurfaceRenderedLastStep = false;
	uint64_t fixedSurfaceRenderGeneration = 0;
	bool lastFixedSurfaceEnabled = false;

	struct GpuTimerSlot {
		GLuint waveQuery = 0;
		GLuint bodyQuery = 0;
		bool pending = false;
		uint64_t sequence = 0;
		int mode = -1;
		bool envelope = false;
		float slither = 0.f;
		int width = 0;
		int height = 0;
		float bodyDomainFraction = 1.f;
		int bodySegmentCount = 0;
	};
	static constexpr size_t kGpuTimerSlotCount = 6u;
	std::array<GpuTimerSlot, kGpuTimerSlotCount> gpuTimerSlots {};
	size_t nextGpuTimerSlot = 0u;
	uint64_t nextGpuTimerSequence = 1u;
	bool gpuTimerSupportChecked = false;
	bool gpuTimerSupported = false;

	void resetWaveColumnTextureState() {
		waveColumnTexture = 0;
		waveColumnTextureW = 0;
		waveColumnTextureH = 0;
		waveColumnTextureCount = -1;
		waveColumnTextureEnvelope = false;
	}

	void resetCurveTextureState() {
		curveTexture = 0;
		curveTextureCount = 0;
		curveTextureRevision = 0;
	}

	void resetWaveShaderState() {
		waveShaderProgram = 0;
		waveShaderCurveLoc = -1;
		waveShaderCurveCountLoc = -1;
		waveShaderPointCountLoc = -1;
		waveShaderInsetLoc = -1;
		waveShaderInvHeightLoc = -1;
		waveShaderEnvelopeLoc = -1;
		waveShaderInitAttempted = false;
		waveShaderReady = false;
	}

	void resetBodyShaderState() {
		bodyShaderProgram = 0;
		bodyShaderSoftnessLoc = -1;
		bodyShaderMiddleRatioLoc = -1;
		bodyShaderCoreRatioLoc = -1;
		bodyShaderOuterColorLoc = -1;
		bodyShaderMiddleColorLoc = -1;
		bodyShaderCoreColorLoc = -1;
		bodyShaderCurveLoc = -1;
		bodyShaderCurveCountLoc = -1;
		bodyShaderInsetLoc = -1;
		bodyShaderInvSizeLoc = -1;
		bodyShaderOuterWidthLoc = -1;
		bodyShaderInitAttempted = false;
		bodyShaderReady = false;
		bodyShaderSegmentRadius = 0;
	}

	void resetGpuTimerState() {
		for (GpuTimerSlot& slot : gpuTimerSlots) {
			slot.waveQuery = 0;
			slot.bodyQuery = 0;
			slot.pending = false;
		}
		nextGpuTimerSlot = 0u;
		nextGpuTimerSequence = 1u;
		gpuTimerSupportChecked = false;
		gpuTimerSupported = false;
		if (module) {
			module->perfCsvGpuSampleValid.store(false, std::memory_order_relaxed);
		}
	}

	bool ensureGpuTimers() {
		if (!gpuTimerSupportChecked) {
			gpuTimerSupportChecked = true;
			gpuTimerSupported = GLEW_VERSION_3_3 || GLEW_ARB_timer_query;
		}
		if (!gpuTimerSupported) return false;
		for (GpuTimerSlot& slot : gpuTimerSlots) {
			if (slot.waveQuery == 0) glGenQueries(1, &slot.waveQuery);
			if (slot.bodyQuery == 0) glGenQueries(1, &slot.bodyQuery);
			if (slot.waveQuery == 0 || slot.bodyQuery == 0) {
				gpuTimerSupported = false;
				return false;
			}
		}
		return true;
	}

	void resolveGpuTimers() {
		if (!module || !gpuTimerSupported) return;
		GpuTimerSlot* newestResolved = nullptr;
		uint64_t newestWaveNs = 0;
		uint64_t newestBodyNs = 0;
		for (GpuTimerSlot& slot : gpuTimerSlots) {
			if (!slot.pending) continue;
			GLint waveReady = GL_FALSE;
			GLint bodyReady = GL_FALSE;
			glGetQueryObjectiv(slot.waveQuery, GL_QUERY_RESULT_AVAILABLE, &waveReady);
			glGetQueryObjectiv(slot.bodyQuery, GL_QUERY_RESULT_AVAILABLE, &bodyReady);
			if (waveReady != GL_TRUE || bodyReady != GL_TRUE) continue;
			GLuint64 waveNs = 0;
			GLuint64 bodyNs = 0;
			glGetQueryObjectui64v(slot.waveQuery, GL_QUERY_RESULT, &waveNs);
			glGetQueryObjectui64v(slot.bodyQuery, GL_QUERY_RESULT, &bodyNs);
			if (!newestResolved || slot.sequence > newestResolved->sequence) {
				newestResolved = &slot;
				newestWaveNs = uint64_t(waveNs);
				newestBodyNs = uint64_t(bodyNs);
			}
			slot.pending = false;
		}
		if (newestResolved) {
			module->perfCsvGpuWaveNs.store(newestWaveNs, std::memory_order_relaxed);
			module->perfCsvGpuBodyNs.store(newestBodyNs, std::memory_order_relaxed);
			module->perfCsvGpuSampleSequence.store(newestResolved->sequence, std::memory_order_relaxed);
			module->perfCsvGpuSampleMode.store(newestResolved->mode, std::memory_order_relaxed);
			module->perfCsvGpuSampleEnvelope.store(newestResolved->envelope, std::memory_order_relaxed);
			module->perfCsvGpuSampleSlither.store(newestResolved->slither, std::memory_order_relaxed);
			module->perfCsvGpuSampleWidth.store(newestResolved->width, std::memory_order_relaxed);
			module->perfCsvGpuSampleHeight.store(newestResolved->height, std::memory_order_relaxed);
			module->perfCsvGpuBodyDomainFraction.store(
				newestResolved->bodyDomainFraction, std::memory_order_relaxed);
			module->perfCsvGpuBodySegmentCount.store(
				newestResolved->bodySegmentCount, std::memory_order_relaxed);
			module->perfCsvGpuSampleValid.store(true, std::memory_order_relaxed);
		}
	}

	GpuTimerSlot* beginGpuTimerSample(Vec framebufferSize, int mode) {
		if (!module || !ensureGpuTimers()) return nullptr;
		resolveGpuTimers();
		for (size_t offset = 0; offset < gpuTimerSlots.size(); ++offset) {
			const size_t index = (nextGpuTimerSlot + offset) % gpuTimerSlots.size();
			GpuTimerSlot& slot = gpuTimerSlots[index];
			if (slot.pending) continue;
			slot.sequence = nextGpuTimerSequence++;
			slot.mode = mode;
			slot.envelope = module->envelopeMode.load(std::memory_order_relaxed);
			slot.slither = module->displaySlitherAmount.load(std::memory_order_relaxed);
			slot.width = std::max(1, int(std::lround(framebufferSize.x)));
			slot.height = std::max(1, int(std::lround(framebufferSize.y)));
			slot.bodyDomainFraction = 1.f;
			slot.bodySegmentCount = 0;
			nextGpuTimerSlot = (index + 1u) % gpuTimerSlots.size();
			return &slot;
		}
		return nullptr;
	}

	void validateGlResourcesForCurrentContext() {
		if (waveColumnTexture != 0 && !glIsTexture(waveColumnTexture)) {
			resetWaveColumnTextureState();
		}
		if (curveTexture != 0 && !glIsTexture(curveTexture)) {
			resetCurveTextureState();
		}
		if (waveShaderReady && (waveShaderProgram == 0 || !glIsProgram(waveShaderProgram))) {
			resetWaveShaderState();
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

	void ensureWaveShader() {
		if (waveShaderInitAttempted) return;
		waveShaderInitAttempted = true;
		static const char* kVs = R"GLSL(
			#version 120
			varying vec2 vUv;
			void main() {
				gl_Position = ftransform();
				vUv = gl_MultiTexCoord0.xy;
			}
		)GLSL";
		static const char* kFs = R"GLSL(
			#version 120
			varying vec2 vUv;
			uniform sampler2D uCurve;
			uniform float uCurveCount;
			uniform float uPointCount;
			uniform float uInset;
			uniform float uInvHeight;
			uniform float uEnvelope;

			vec4 over(vec4 dst, vec4 src) {
				float outA = src.a + dst.a * (1.0 - src.a);
				vec3 premul = src.rgb * src.a + dst.rgb * dst.a * (1.0 - src.a);
				return vec4(outA > 0.00001 ? premul / outA : vec3(0.0), outA);
			}

			void main() {
				float span = max(0.00001, 1.0 - 2.0 * uInset);
				float phase = (vUv.x - uInset) / span;
				float valid = step(0.0, phase) * step(phase, 1.0);
				float curveU = (clamp(phase, 0.0, 1.0) * (uCurveCount - 1.0) + 0.5) / uCurveCount;
				float curveY = texture2D(uCurve, vec2(curveU, 0.5)).r;
				float aa = max(uInvHeight * 1.25, 0.0001);
				float y = vUv.y;
				float fillMask;
				if (uEnvelope > 0.5) {
					fillMask = smoothstep(curveY - aa, curveY + aa, y);
				}
				else if (curveY < 0.5) {
					fillMask = smoothstep(curveY - aa, curveY + aa, y)
						* (1.0 - smoothstep(0.5, 0.5 + aa, y));
				}
				else {
					fillMask = smoothstep(0.5 - aa, 0.5, y)
						* (1.0 - smoothstep(curveY - aa, curveY + aa, y));
				}
				fillMask *= valid;

				bool positive = y < 0.5;
				vec4 cyanNear = vec4(28.0, 204.0, 217.0, 46.0) / 255.0;
				vec4 cyanFar = vec4(42.0, 228.0, 255.0, 152.0) / 255.0;
				vec4 purpleNear = vec4(115.0, 72.0, 224.0, 50.0) / 255.0;
				vec4 purpleFar = vec4(150.0, 92.0, 255.0, 162.0) / 255.0;
				vec4 color;
				if (uEnvelope > 0.5) {
					// Envelope height is unipolar: use one continuous material field,
					// purple at the floor and cyan at the ceiling.
					color = mix(purpleFar, cyanFar, clamp(1.0 - y, 0.0, 1.0));
				}
				else {
					float depth = positive ? clamp((0.5 - y) * 2.0, 0.0, 1.0)
						: clamp((y - 0.5) * 2.0, 0.0, 1.0);
					color = positive
						? mix(cyanNear, cyanFar, depth)
						: mix(purpleNear, purpleFar, depth);
				}
				color.a *= fillMask;

				float column = floor(clamp(phase, 0.0, 0.999999) * uPointCount);
				if (mod(column, 2.0) >= 1.0 && fillMask > 0.0) {
					vec4 cyanShade = vec4(0.0, 56.0, 72.0, 132.0) / 255.0;
					vec4 purpleShade = vec4(40.0, 24.0, 112.0, 92.0) / 255.0;
					vec4 shade = uEnvelope > 0.5
						? mix(purpleShade, cyanShade, clamp(1.0 - y, 0.0, 1.0))
						: (positive ? cyanShade : purpleShade);
					shade.a *= fillMask;
					color = over(color, shade);
				}

				if (uEnvelope < 0.5) {
					// Match NanoVG's one-logical-pixel midpoint stroke. The old mask
					// retained a full-width core and then feathered for another pixel
					// on each side, making the analytical rule visibly thicker.
					float midMask = 1.0 - smoothstep(0.0, uInvHeight, abs(y - 0.5));
					color = over(color, vec4(240.0 / 255.0, 180.0 / 255.0, 42.0 / 255.0,
						(150.0 / 255.0) * midMask));
				}
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
		waveShaderProgram = glCreateProgram();
		if (!waveShaderProgram) {
			glDeleteShader(vs);
			glDeleteShader(fs);
			return;
		}
		glAttachShader(waveShaderProgram, vs);
		glAttachShader(waveShaderProgram, fs);
		glLinkProgram(waveShaderProgram);
		glDeleteShader(vs);
		glDeleteShader(fs);
		GLint linked = GL_FALSE;
		glGetProgramiv(waveShaderProgram, GL_LINK_STATUS, &linked);
		if (linked != GL_TRUE) {
			glDeleteProgram(waveShaderProgram);
			waveShaderProgram = 0;
			return;
		}
		waveShaderCurveLoc = glGetUniformLocation(waveShaderProgram, "uCurve");
		waveShaderCurveCountLoc = glGetUniformLocation(waveShaderProgram, "uCurveCount");
		waveShaderPointCountLoc = glGetUniformLocation(waveShaderProgram, "uPointCount");
		waveShaderInsetLoc = glGetUniformLocation(waveShaderProgram, "uInset");
		waveShaderInvHeightLoc = glGetUniformLocation(waveShaderProgram, "uInvHeight");
		waveShaderEnvelopeLoc = glGetUniformLocation(waveShaderProgram, "uEnvelope");
		waveShaderReady = waveShaderCurveLoc >= 0 && waveShaderCurveCountLoc >= 0
			&& waveShaderPointCountLoc >= 0 && waveShaderInsetLoc >= 0
			&& waveShaderInvHeightLoc >= 0 && waveShaderEnvelopeLoc >= 0;
		if (!waveShaderReady) {
			glDeleteProgram(waveShaderProgram);
			waveShaderProgram = 0;
		}
	}

	void ensureBodyShader(int segmentRadius) {
		segmentRadius = clamp(segmentRadius, 1, kWyrmMaxBodySegmentRadius);
		if (bodyShaderInitAttempted && bodyShaderSegmentRadius == segmentRadius) return;
		if (bodyShaderProgram != 0) {
			glDeleteProgram(bodyShaderProgram);
		}
		resetBodyShaderState();
		bodyShaderInitAttempted = true;
		bodyShaderSegmentRadius = segmentRadius;
		static const char* kVs = R"GLSL(
			#version 120
			varying vec2 vUv;
			void main() {
				gl_Position = ftransform();
				vUv = gl_MultiTexCoord0.xy;
			}
		)GLSL";
		const std::string fsSource = std::string(
			"#version 120\n#define WYRM_SEGMENT_RADIUS ")
			+ std::to_string(segmentRadius) + "\n" + R"GLSL(
			varying vec2 vUv;
			uniform sampler2D uCurve;
			uniform float uCurveCount;
			uniform float uInset;
			uniform vec2 uInvSize;
			uniform float uOuterWidth;
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
			vec2 curvePoint(float index) {
				float i = clamp(index, 0.0, uCurveCount - 1.0);
				float curveU = (i + 0.5) / uCurveCount;
				float span = max(0.00001, 1.0 - 2.0 * uInset);
				return vec2(
					(uInset + curveU * span) / uInvSize.x,
					texture2D(uCurve, vec2(curveU, 0.5)).r / uInvSize.y);
			}
			vec2 curvePointWithY(float index, float curveY) {
				float i = clamp(index, 0.0, uCurveCount - 1.0);
				float curveU = (i + 0.5) / uCurveCount;
				float span = max(0.00001, 1.0 - 2.0 * uInset);
				return vec2(
					(uInset + curveU * span) / uInvSize.x,
					curveY / uInvSize.y);
			}
			float segmentDistanceSquared(vec2 p, vec2 a, vec2 b) {
				vec2 ab = b - a;
				float denom = max(dot(ab, ab), 0.00001);
				float t = clamp(dot(p - a, ab) / denom, 0.0, 1.0);
				vec2 delta = p - (a + ab * t);
				return dot(delta, delta);
			}
			void main() {
				vec2 p = vec2(vUv.x / uInvSize.x, vUv.y / uInvSize.y);
				float span = max(0.00001, 1.0 - 2.0 * uInset);
				float phase = clamp((vUv.x - uInset) / span, 0.0, 1.0);
				float centerIndex = floor(phase * uCurveCount - 0.5);
		#if WYRM_SEGMENT_RADIUS == 2
				float packedIndex = clamp(centerIndex, 0.0, uCurveCount - 1.0);
				vec4 packedBack = texture2D(uCurve,
					vec2((packedIndex + 0.5) / uCurveCount, 0.5));
				vec2 p0 = curvePointWithY(centerIndex - 2.0, packedBack.b);
				vec2 p1 = curvePointWithY(centerIndex - 1.0, packedBack.g);
				vec2 p2 = curvePointWithY(centerIndex, packedBack.r);
				vec2 p3 = curvePoint(centerIndex + 1.0);
				vec2 p4 = curvePoint(centerIndex + 2.0);
				vec2 p5 = curvePoint(centerIndex + 3.0);
				float distanceSquaredPx = segmentDistanceSquared(p, p0, p1);
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p1, p2));
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p2, p3));
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p3, p4));
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p4, p5));
		#elif WYRM_SEGMENT_RADIUS == 3
				float packedIndex = clamp(centerIndex, 0.0, uCurveCount - 1.0);
				vec4 packedBack = texture2D(uCurve,
					vec2((packedIndex + 0.5) / uCurveCount, 0.5));
				vec2 p0 = curvePointWithY(centerIndex - 3.0, packedBack.a);
				vec2 p1 = curvePointWithY(centerIndex - 2.0, packedBack.b);
				vec2 p2 = curvePointWithY(centerIndex - 1.0, packedBack.g);
				vec2 p3 = curvePointWithY(centerIndex, packedBack.r);
				vec2 p4 = curvePoint(centerIndex + 1.0);
				vec2 p5 = curvePoint(centerIndex + 2.0);
				vec2 p6 = curvePoint(centerIndex + 3.0);
				vec2 p7 = curvePoint(centerIndex + 4.0);
				float distanceSquaredPx = segmentDistanceSquared(p, p0, p1);
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p1, p2));
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p2, p3));
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p3, p4));
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p4, p5));
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p5, p6));
				distanceSquaredPx = min(distanceSquaredPx, segmentDistanceSquared(p, p6, p7));
		#else
				float firstIndex = centerIndex - float(WYRM_SEGMENT_RADIUS);
				vec2 a = curvePoint(firstIndex);
				float distanceSquaredPx = 1000000000000.0;
				for (int offset = 0; offset < (2 * WYRM_SEGMENT_RADIUS + 1); ++offset) {
					vec2 b = curvePoint(firstIndex + float(offset + 1));
					distanceSquaredPx = min(
						distanceSquaredPx, segmentDistanceSquared(p, a, b));
					a = b;
				}
		#endif
				float distancePx = sqrt(distanceSquaredPx);
				float d = distancePx * 2.0 / uOuterWidth;
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
		GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSource.c_str());
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
		bodyShaderCurveLoc = glGetUniformLocation(bodyShaderProgram, "uCurve");
		bodyShaderCurveCountLoc = glGetUniformLocation(bodyShaderProgram, "uCurveCount");
		bodyShaderInsetLoc = glGetUniformLocation(bodyShaderProgram, "uInset");
		bodyShaderInvSizeLoc = glGetUniformLocation(bodyShaderProgram, "uInvSize");
		bodyShaderOuterWidthLoc = glGetUniformLocation(bodyShaderProgram, "uOuterWidth");
		bodyShaderReady = bodyShaderSoftnessLoc >= 0
			&& bodyShaderMiddleRatioLoc >= 0 && bodyShaderCoreRatioLoc >= 0
			&& bodyShaderOuterColorLoc >= 0 && bodyShaderMiddleColorLoc >= 0
			&& bodyShaderCoreColorLoc >= 0 && bodyShaderCurveLoc >= 0
			&& bodyShaderCurveCountLoc >= 0 && bodyShaderInsetLoc >= 0
			&& bodyShaderInvSizeLoc >= 0 && bodyShaderOuterWidthLoc >= 0;
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

	void ensureWaveColumnTexture(Vec size, int count, bool envelopeVisual) {
		const int w = std::max(1, int(std::ceil(size.x)));
		const int h = std::max(1, int(std::ceil(size.y)));
		count = std::max(1, count);
		if (waveColumnTexture != 0 &&
			waveColumnTextureW == w &&
			waveColumnTextureH == h &&
			waveColumnTextureCount == count &&
			waveColumnTextureEnvelope == envelopeVisual) {
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
			const NVGcolor base = envelopeVisual
				? mixColor(negFar, posFar, levi_math::clamp01(1.f - y / std::max(size.y, 1.f)))
				: (positive ? mixColor(posNear, posFar, t) : mixColor(negNear, negFar, t));
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
					const NVGcolor shade = envelopeVisual
						? mixColor(negShade, posShade, levi_math::clamp01(1.f - y / std::max(size.y, 1.f)))
						: (positive ? posShade : negShade);
					compositeOver(shade, out);
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
		waveColumnTextureEnvelope = envelopeVisual;
	}

	bool prepareCurveTexture(Vec size, bool shaderPath) {
		if (!module || module->pointCount <= 0) return false;
		if (!geometryCache) {
			geometryCache = std::make_shared<wyrm_render::DisplayGeometryCache>();
		}
		geometryCache->ensure(module, size, bodySampleCountForSize(size, shaderPath),
			wyrm_render::DisplayGeometryRequirement::PointsOnly);
		const std::vector<Vec>& curve = geometryCache->points;
		if (curve.size() < 2u) return false;

		if (curveTexture == 0 || curveTextureRevision != geometryCache->revision
			|| curveTextureCount != int(curve.size())) {
			std::vector<float> pixels(curve.size() * 4u, 0.f);
			const float invHeight = 1.f / std::max(size.y, 1.f);
			for (size_t i = 0; i < curve.size(); ++i) {
				const size_t offset = i * 4u;
				const float y = clamp(curve[i].y * invHeight, 0.f, 1.f);
				pixels[offset] = y;
				pixels[offset + 1u] = i >= 1u ? pixels[(i - 1u) * 4u] : pixels[0];
				pixels[offset + 2u] = i >= 2u ? pixels[(i - 2u) * 4u] : pixels[0];
				pixels[offset + 3u] = i >= 3u ? pixels[(i - 3u) * 4u] : pixels[0];
			}
			if (curveTexture == 0) glGenTextures(1, &curveTexture);
			glBindTexture(GL_TEXTURE_2D, curveTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, GLsizei(curve.size()), 1,
				0, GL_RGBA, GL_FLOAT, pixels.data());
			curveTextureCount = int(curve.size());
			curveTextureRevision = geometryCache->revision;
		}
		return curveTexture != 0 && curveTextureCount >= 2;
	}

	bool drawWaveAnalyticalGl(Vec size, bool shaderPath) {
		ensureWaveShader();
		if (!waveShaderReady || !prepareCurveTexture(size, shaderPath)) return false;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, curveTexture);
		glEnable(GL_TEXTURE_2D);
		glUseProgram(waveShaderProgram);
		glUniform1i(waveShaderCurveLoc, 0);
		glUniform1f(waveShaderCurveCountLoc, float(curveTextureCount));
		glUniform1f(waveShaderPointCountLoc, float(std::max(1, module->pointCount)));
		glUniform1f(waveShaderInsetLoc, wyrm_render::kPointEdgeInsetPx / std::max(size.x, 1.f));
		glUniform1f(waveShaderInvHeightLoc, 1.f / std::max(size.y, 1.f));
		glUniform1f(waveShaderEnvelopeLoc,
			module->envelopeMode.load(std::memory_order_relaxed) ? 1.f : 0.f);
		glColor4f(1.f, 1.f, 1.f, 1.f);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
		glTexCoord2f(1.f, 0.f); glVertex2f(size.x, 0.f);
		glTexCoord2f(0.f, 1.f); glVertex2f(0.f, size.y);
		glTexCoord2f(1.f, 1.f); glVertex2f(size.x, size.y);
		glEnd();
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		return true;
	}

	void drawWaveColumnsFallbackGl(Vec size, bool shaderPath) {
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
		ensureWaveColumnTexture(size, count, envelopeVisual);
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
			const float textureScaleY = 1.f;
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

	void drawWaveColumnsGl(Vec size, bool shaderPath) {
		if (!drawWaveAnalyticalGl(size, shaderPath)) {
			drawWaveColumnsFallbackGl(size, shaderPath);
		}
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

	int requiredBodySegmentRadius(Vec size, float outerRadius) const {
		if (curveTextureCount < 2) return 0;
		const float spacing = wyrm_render::pointDrawWidth(size) / float(curveTextureCount);
		if (spacing <= 1e-5f) return 0;
		// A segment m indices away has at least (m - 1) * spacing horizontal
		// separation from the fragment's containing segment. Include the boundary
		// segment so every segment capable of nonzero outer coverage is tested.
		return std::max(1, int(std::floor(outerRadius / spacing)) + 1);
	}

	void ensureBodyTiles(Vec size, float outerRadius) {
		if (bodyTileGeometryRevision == geometryCache->revision
			&& bodyTileSize == size) {
			return;
		}
		bodyTileGeometryRevision = geometryCache->revision;
		bodyTileSize = size;
		bodyTiles.clear();
		bodyTileDomainFraction = 0.f;
		const std::vector<Vec>& points = geometryCache->points;
		if (points.size() < 2u || size.x <= 0.f || size.y <= 0.f) return;

		const float tileWidth = std::max(1.f, kWyrmBodyTileWidthPx);
		const int tileCount = std::max(1, int(std::ceil(size.x / tileWidth)));
		bodyTileMinY.assign(size_t(tileCount), size.y);
		bodyTileMaxY.assign(size_t(tileCount), 0.f);
		bodyTileActive.assign(size_t(tileCount), 0u);
		const float support = outerRadius + kWyrmBodyTileRasterGuardPx;
		for (size_t i = 0; i + 1u < points.size(); ++i) {
			const Vec& a = points[i];
			const Vec& b = points[i + 1u];
			const float segmentX0 = std::min(a.x, b.x) - support;
			const float segmentX1 = std::max(a.x, b.x) + support;
			const int firstTile = clamp(int(std::floor(segmentX0 / tileWidth)), 0, tileCount - 1);
			const int lastTile = clamp(int(std::floor(segmentX1 / tileWidth)), 0, tileCount - 1);
			const float segmentMinY = std::min(a.y, b.y);
			const float segmentMaxY = std::max(a.y, b.y);
			for (int tile = firstTile; tile <= lastTile; ++tile) {
				bodyTileActive[size_t(tile)] = 1u;
				bodyTileMinY[size_t(tile)] = std::min(bodyTileMinY[size_t(tile)], segmentMinY);
				bodyTileMaxY[size_t(tile)] = std::max(bodyTileMaxY[size_t(tile)], segmentMaxY);
			}
		}

		float domainArea = 0.f;
		bodyTiles.reserve(size_t(tileCount));
		for (int tile = 0; tile < tileCount; ++tile) {
			if (!bodyTileActive[size_t(tile)]) continue;
			const float x0 = float(tile) * tileWidth;
			const float x1 = std::min(size.x, float(tile + 1) * tileWidth);
			const float y0 = std::max(0.f, bodyTileMinY[size_t(tile)] - support);
			const float y1 = std::min(size.y, bodyTileMaxY[size_t(tile)] + support);
			if (x1 <= x0 || y1 <= y0) continue;
			domainArea += (x1 - x0) * (y1 - y0);
			bodyTiles.push_back(BodyTile {x0, y0, x1, y1});
		}
		bodyTileDomainFraction = clamp(domainArea / (size.x * size.y), 0.f, 1.f);
	}

	void drawBodyGl(Vec size, bool shaderPath, bool includeBase, bool includeGlow) {
		if (!module || module->pointCount < 2) return;
		const wyrm_render::BodyMaterial& material = wyrm_render::bodyMaterial();
		if (shaderPath && prepareCurveTexture(size, shaderPath)) {
			if (kWyrmUseConservativeBodyTiles) {
				ensureBodyTiles(size, 0.5f * material.layers[0].widthPx);
			}
			else {
				bodyTileDomainFraction = 1.f;
			}
			auto setLayerColor = [](GLint location, const wyrm_render::BodyLayerMaterial& layer) {
				glUniform4f(location, layer.r / 255.f, layer.g / 255.f, layer.b / 255.f, layer.a / 255.f);
			};
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, curveTexture);
			glEnable(GL_TEXTURE_2D);
			glUseProgram(bodyShaderProgram);
			glUniform1i(bodyShaderCurveLoc, 0);
			glUniform1f(bodyShaderCurveCountLoc, float(curveTextureCount));
			glUniform1f(bodyShaderInsetLoc,
				wyrm_render::kPointEdgeInsetPx / std::max(size.x, 1.f));
			glUniform2f(bodyShaderInvSizeLoc,
				1.f / std::max(size.x, 1.f), 1.f / std::max(size.y, 1.f));
			glUniform1f(bodyShaderOuterWidthLoc, material.layers[0].widthPx);
			glUniform1f(bodyShaderSoftnessLoc, material.edgeSoftness);
			glUniform1f(bodyShaderMiddleRatioLoc,
				material.layers[1].widthPx / material.layers[0].widthPx);
			glUniform1f(bodyShaderCoreRatioLoc,
				material.layers[2].widthPx / material.layers[0].widthPx);
			setLayerColor(bodyShaderOuterColorLoc, material.layers[0]);
			setLayerColor(bodyShaderMiddleColorLoc, material.layers[1]);
			setLayerColor(bodyShaderCoreColorLoc, material.layers[2]);
			glColor4f(1.f, 1.f, 1.f, 1.f);
			if (kWyrmUseConservativeBodyTiles) {
				glBegin(GL_QUADS);
				for (const BodyTile& tile : bodyTiles) {
					glTexCoord2f(tile.x0 / size.x, tile.y0 / size.y); glVertex2f(tile.x0, tile.y0);
					glTexCoord2f(tile.x1 / size.x, tile.y0 / size.y); glVertex2f(tile.x1, tile.y0);
					glTexCoord2f(tile.x1 / size.x, tile.y1 / size.y); glVertex2f(tile.x1, tile.y1);
					glTexCoord2f(tile.x0 / size.x, tile.y1 / size.y); glVertex2f(tile.x0, tile.y1);
				}
				glEnd();
			}
			else {
				glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
				glTexCoord2f(1.f, 0.f); glVertex2f(size.x, 0.f);
				glTexCoord2f(0.f, 1.f); glVertex2f(0.f, size.y);
				glTexCoord2f(1.f, 1.f); glVertex2f(size.x, size.y);
				glEnd();
			}
			glUseProgram(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
			return;
		}

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
			|| fallbackBodyStripVertices[0].size() != samples.size() * 2u) {
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
			for (size_t layer = 0; layer < fallbackBodyStripVertices.size(); ++layer) {
				drawCachedStrip(fallbackBodyStripVertices[layer], material.layers[layer], false);
			}
		}
		(void) includeGlow;
	}

	void clearFixedSurfaceMetrics() {
		fixedSurfaceRenderGeneration = 0;
		if (module) {
			module->perfFixedSurfaceWidth.store(0, std::memory_order_relaxed);
			module->perfFixedSurfaceHeight.store(0, std::memory_order_relaxed);
			module->perfFixedSurfaceGeneration.store(0, std::memory_order_relaxed);
		}
	}

	void abandonRendererResources() {
		resetGpuTimerState();
		resetBodyShaderState();
		resetWaveShaderState();
		resetCurveTextureState();
		resetWaveColumnTextureState();
		bodyStripGeometryRevision = 0;
		redrawStateInitialized = false;
		rendererVg = nullptr;
	}

	~WyrmGlRendererWidget() override {
		// DAW plugin editors can destroy/recreate their GL context around the
		// Rack UI. Avoid driver calls from widget teardown; resources are
		// reclaimed by the editor/context owner.
		abandonRendererResources();
		compactSurface.reset(false);
		expandedSurface.reset(false);
		clearFixedSurfaceMetrics();
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		OpenGlWidget::onContextDestroy(e);
		abandonRendererResources();
		compactSurface.reset(true);
		expandedSurface.reset(true);
		clearFixedSurfaceMetrics();
	}

	void onContextCreate(const ContextCreateEvent& e) override {
		OpenGlWidget::onContextCreate(e);
		abandonRendererResources();
		compactSurface.reset(false);
		expandedSurface.reset(false);
		clearFixedSurfaceMetrics();
		setDirty();
	}

	bool compactEditor() const {
		return box.size.x <= 300.f;
	}

	visual_assets::AdaptiveGlSurface& activeFixedSurface() {
		return compactEditor() ? compactSurface : expandedSurface;
	}

	const visual_assets::AdaptiveGlSurface& activeFixedSurface() const {
		return compactEditor() ? compactSurface : expandedSurface;
	}

	void step() override {
		if (!module) {
			return;
		}
		const int mode = module->renderMode.load(std::memory_order_relaxed);
		const bool renderGl = (mode == WYRM_RENDER_OPENGL || mode == WYRM_RENDER_OPENGL_SHDR);
		visible = renderGl;
		if (!visible) {
			module->perfFixedSurfaceActive.store(false, std::memory_order_relaxed);
			redrawStateInitialized = false;
			return;
		}
		const uint32_t waveVersion = module->waveVersion.load(std::memory_order_acquire);
		const int rockStateIndex = module->activeRockStateIndex.load(std::memory_order_acquire);
		const int pointCount = module->pointCount;
		const bool envelopeMode = module->envelopeMode.load(std::memory_order_relaxed);
		const float slitherAmount = levi_math::clamp01(
			module->displaySlitherAmount.load(std::memory_order_relaxed));
		const float slitherPhase = module->uiSlitherPhase.load(std::memory_order_relaxed);
		const bool fixedSurfaceEnabled = module->fixedSurfaceExperiment.load(std::memory_order_relaxed);
		module->perfFixedSurfaceActive.store(fixedSurfaceEnabled, std::memory_order_relaxed);
		fixedSurfaceRenderedLastStep = false;
		bool dirty = !redrawStateInitialized;
		dirty = dirty || mode != lastRenderMode;
		dirty = dirty || waveVersion != lastWaveVersion;
		dirty = dirty || rockStateIndex != lastRockStateIndex;
		dirty = dirty || pointCount != lastPointCount;
		dirty = dirty || envelopeMode != lastEnvelopeMode;
		dirty = dirty || std::fabs(box.size.x - lastDrawSize.x) > 1e-4f;
		dirty = dirty || std::fabs(box.size.y - lastDrawSize.y) > 1e-4f;
		dirty = dirty || std::fabs(slitherAmount - lastSlitherAmount) > 1e-5f;
		dirty = dirty || (slitherAmount > 1e-5f
			&& std::fabs(slitherPhase - lastSlitherPhase) > 1e-6f);
		dirty = dirty || fixedSurfaceEnabled != lastFixedSurfaceEnabled;

		lastRenderMode = mode;
		lastWaveVersion = waveVersion;
		lastRockStateIndex = rockStateIndex;
		lastPointCount = pointCount;
		lastEnvelopeMode = envelopeMode;
		lastSlitherAmount = slitherAmount;
		lastSlitherPhase = slitherPhase;
		lastDrawSize = box.size;
		lastFixedSurfaceEnabled = fixedSurfaceEnabled;
		redrawStateInitialized = true;

		if (dirty) {
			activeFixedSurface().markDirty();
			setDirty();
		}
		if (fixedSurfaceEnabled) {
			renderFixedSurfaceIfNeeded();
		}
		// OpenGlWidget::step() deliberately redraws every frame. Use the cached
		// framebuffer behavior now that all live GL invalidation is explicit above.
		widget::FramebufferWidget::step();
	}

	void draw(const DrawArgs& args) override {
		using PerfClock = std::chrono::steady_clock;
		const bool logCsv = module && isDragonKingDebugEnabled() && isWyrmDrawLoggingEnabled();
		const bool fixedSurfaceEnabled = module
			&& module->fixedSurfaceExperiment.load(std::memory_order_relaxed);
		const bool cacheWasDirty = fixedSurfaceEnabled ? fixedSurfaceRenderedLastStep : dirty;
		const PerfClock::time_point start = logCsv ? PerfClock::now() : PerfClock::time_point();
		if (!fixedSurfaceEnabled || !activeFixedSurface().draw(args, box.size)) {
			widget::FramebufferWidget::draw(args);
		}
		if (logCsv) {
			const uint64_t elapsedNs = uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - start).count());
			module->perfCsvGlDrawNs.store(elapsedNs, std::memory_order_relaxed);
			module->perfCsvGlDirty.store(cacheWasDirty, std::memory_order_relaxed);
		}
	}

	void renderGlContent(Vec fbSize, int viewportY = 0) {
		glViewport(0, viewportY, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));
		if (isExtraGlValidationEnabled()) {
			validateGlResourcesForCurrentContext();
		}
		glDisable(GL_SCISSOR_TEST);
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
		const bool logGpu = useShdr && isDragonKingDebugEnabled() && isWyrmDrawLoggingEnabled();
		if (module) module->perfCsvGpuSampleValid.store(false, std::memory_order_relaxed);
		int requiredSegmentRadius = 0;
		bool analyticalBodySupported = false;
		if (useShdr && prepareCurveTexture(box.size, true)) {
			requiredSegmentRadius = requiredBodySegmentRadius(
				box.size, 0.5f * wyrm_render::bodyMaterial().layers[0].widthPx);
			analyticalBodySupported = requiredSegmentRadius > 0
				&& requiredSegmentRadius <= kWyrmMaxBodySegmentRadius;
			if (analyticalBodySupported) {
				ensureBodyShader(requiredSegmentRadius);
			}
		}
		const bool shaderPath = useShdr && analyticalBodySupported
			&& bodyShaderReady && bodyShaderSegmentRadius == requiredSegmentRadius;
		activeBodySegmentCount = shaderPath ? 2 * requiredSegmentRadius + 1 : 0;
		GpuTimerSlot* gpuTimer = logGpu ? beginGpuTimerSample(fbSize, mode) : nullptr;
		bodyTileDomainFraction = 1.f;

		if (gpuTimer) glBeginQuery(GL_TIME_ELAPSED, gpuTimer->waveQuery);
		drawWaveColumnsGl(box.size, shaderPath);
		if (gpuTimer) glEndQuery(GL_TIME_ELAPSED);
		if (gpuTimer) glBeginQuery(GL_TIME_ELAPSED, gpuTimer->bodyQuery);
		drawBodyGl(box.size, shaderPath, true, false);
		if (gpuTimer) glEndQuery(GL_TIME_ELAPSED);
		if (gpuTimer) {
			gpuTimer->bodyDomainFraction = bodyTileDomainFraction;
			gpuTimer->bodySegmentCount = activeBodySegmentCount;
			gpuTimer->pending = true;
		}

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	void drawFramebuffer() override {
		renderGlContent(getFramebufferSize());
	}

	void renderFixedSurfaceIfNeeded() {
		if (!module || !module->fixedSurfaceExperiment.load(std::memory_order_relaxed)) return;
		NVGcontext* vg = (APP && APP->window) ? APP->window->vg : nullptr;
		if (!vg) return;
		if (rendererVg != vg) {
			// Wyrm owns its shader, texture, geometry, and timer-query names.
			// Adaptive surfaces handle only their own context-bound FBO resources.
			abandonRendererResources();
			rendererVg = vg;
			compactSurface.markDirty();
			expandedSurface.markDirty();
		}
		float rackZoom = 1.f;
		if (APP && APP->scene && APP->scene->rackScroll) {
			rackZoom = std::max(APP->scene->rackScroll->getZoom(), 1e-4f);
		}
		const float pixelRatio = (APP && APP->window) ? APP->window->pixelRatio : 1.f;
		visual_assets::AdaptiveGlSurfacePolicy policy;
		visual_assets::AdaptiveGlSurface& surface = activeFixedSurface();
		fixedSurfaceRenderedLastStep = surface.renderIfNeeded(
			vg, box.size, rackZoom, pixelRatio, policy,
			isExtraGlValidationEnabled(),
			[](void* user, Vec activeSize, int viewportY) {
				static_cast<WyrmGlRendererWidget*>(user)->renderGlContent(activeSize, viewportY);
			},
			this);
		if (fixedSurfaceRenderedLastStep) ++fixedSurfaceRenderGeneration;
		module->perfFixedSurfaceWidth.store(surface.activeWidth(), std::memory_order_relaxed);
		module->perfFixedSurfaceHeight.store(surface.activeHeight(), std::memory_order_relaxed);
		module->perfFixedSurfaceGeneration.store(fixedSurfaceRenderGeneration, std::memory_order_relaxed);
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

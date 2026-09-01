#include "Bifurx.hpp"
#include "GlLifecycleUtils.hpp"
#include "visual/AdaptiveGlSurface.hpp"
#include <nanovg_gl.h>
#include <cstddef>
#include <array>

namespace bifurx {

static constexpr size_t kCurveTextureRingSize = 3;

struct BifurxSpectrumGLWidget final : widget::OpenGlWidget, BifurxSpectrumBase {
	struct GlVertex {
		float x, y;
		float r, g, b, a;
	};
	struct GlStrokeQuadVertex {
		float x, y;
		float r, g, b, a;
		float sideDist;
		float radius;
	};

	// Persistent buffers to avoid per-frame allocations
	std::vector<GlVertex> fillVertices;
	std::vector<GlVertex> fillSoftCapVertices;
	std::vector<GlVertex> fillCrestLineVertices;
	std::vector<GlStrokeQuadVertex> fillCrestStrokeVertices;
	std::vector<GlStrokeQuadVertex> strokeQuadVertices;
	std::vector<BifurxCurvePoint> overlayCurvePoints;
	std::vector<GlVertex> expectedCurveLineVertices;
	std::vector<GlStrokeQuadVertex> expectedCurveStrokeVertices;
	std::vector<uint16_t> curveTexels; // RGBA16 normalized, kCurvePointCount wide, reused each frame

	GLuint textureProgram = 0;
	GLuint textureVertex = 0;
	GLuint textureFragment = 0;
	GLuint textureVbo = 0;
	std::array<GLuint, kCurveTextureRingSize> curveTextures {};
	size_t curveTextureIndex = kCurveTextureRingSize - 1u;
	bool textureShaderInitAttempted = false;
	bool textureShaderReady = false;
	GLint textureUniformViewport = -1;
	GLint textureUniformCurveTex = -1;
	GLint textureUniformExpectedWhite = -1;
	GLint textureUniformExpectedCyan = -1;
	GLint textureUniformExpectedPurple = -1;
	GLint textureUniformDisplayOnlyMode = -1;
	GLint textureUniformDisplayOnlyShapeControl = -1;
	GLint textureUniformSpectrumTopY = -1;
	GLint textureUniformSpectrumBottomY = -1;
	GLint textureUniformPlotWidth = -1;
	GLint textureUniformPlotHeight = -1;

	GLuint program = 0; // Legacy unused in fixed-path but kept for struct shape
	GLuint vbo = 0;
	bool shaderInitAttempted = false;
	bool shaderReady = false;
	GLuint shaderProgram = 0;
	GLuint shaderVertex = 0;
	GLuint shaderFragment = 0;
	GLuint shaderVbo = 0;
	GLint shaderUniformViewport = -1;
	GLsizeiptr shaderVboCapacityBytes = 0;
	bool strokeShaderInitAttempted = false;
	bool strokeShaderReady = false;
	GLuint strokeShaderProgram = 0;
	GLuint strokeShaderVertex = 0;
	GLuint strokeShaderFragment = 0;
	GLuint strokeShaderVbo = 0;
	GLint strokeUniformViewport = -1;
	GLsizeiptr strokeShaderVboCapacityBytes = 0;
	bool markerShaderInitAttempted = false;
	bool markerShaderReady = false;
	GLuint markerShaderProgram = 0;
	GLuint markerShaderVertex = 0;
	GLuint markerShaderFragment = 0;
	GLint markerUniformFillRadius = -1;
	GLint markerUniformOutlineRadius = -1;
	GLint markerUniformOutlineHalfWidth = -1;
	int cachedTopLabelFontHandle = -1;
	float cachedTopLabelFontSize = NAN;
	float cachedTopLabelReservedWidth = 0.f;
	bool lastShowModuleResponseOverlay = false;
	bool lastUseGlShaderRenderer = false;
	int lastColorScheme = -1;
	bool lastThreeColorFftGradient = false;
	bool shaderRendererActiveLastFrame = false;
	bool shaderRendererFallbackLastFrame = false;
	bool expectedCurveShaderActiveLastFrame = false;
	bool markerShaderActiveLastFrame = false;
	uint64_t lastDrawNs = 0;
	float lastDrawMsEma = 0.f;
	float lastStepMsEma = 0.f;
	uint64_t lastDrawVertexCount = 0;
	visual_assets::AdaptiveGlSurface fixedSurface;
	NVGcontext* rendererVg = nullptr;
	bool lastFixedSurfaceEnabled = false;

	BifurxSpectrumGLWidget() : BifurxSpectrumBase() {
		const size_t overlaySegmentCount = (kCurvePointCount > 0) ? size_t(kCurvePointCount - 1) : size_t(0);
		const size_t refinedPointReserve = size_t(kCurvePointCount) + 8;
		fillVertices.reserve(overlaySegmentCount * 6);
		fillSoftCapVertices.reserve(overlaySegmentCount * 12);
		fillCrestLineVertices.reserve(overlaySegmentCount * 2);
		fillCrestStrokeVertices.reserve(overlaySegmentCount * 6);
		strokeQuadVertices.reserve(size_t(kCurvePointCount) * 24u);
		overlayCurvePoints.reserve(refinedPointReserve);
		expectedCurveLineVertices.reserve(refinedPointReserve);
		expectedCurveStrokeVertices.reserve((refinedPointReserve - 1u) * 18u + 72u);
		curveTexels.resize(size_t(kCurvePointCount) * 4u, 0); // 4 channels per pixel (RGBA16)
	}

	void releaseShaderResources(bool deleteGlObjects) {
		if (deleteGlObjects && shaderVbo) {
			glDeleteBuffers(1, &shaderVbo);
		}
		shaderVbo = 0;
		if (deleteGlObjects && shaderProgram) {
			glDeleteProgram(shaderProgram);
		}
		shaderProgram = 0;
		if (deleteGlObjects && strokeShaderVbo) {
			glDeleteBuffers(1, &strokeShaderVbo);
		}
		strokeShaderVbo = 0;
		if (deleteGlObjects && strokeShaderProgram) {
			glDeleteProgram(strokeShaderProgram);
		}
		strokeShaderProgram = 0;
		if (deleteGlObjects && markerShaderProgram) {
			glDeleteProgram(markerShaderProgram);
		}
		markerShaderProgram = 0;
		if (deleteGlObjects && shaderVertex) {
			glDeleteShader(shaderVertex);
		}
		shaderVertex = 0;
		if (deleteGlObjects && shaderFragment) {
			glDeleteShader(shaderFragment);
		}
		shaderFragment = 0;
		if (deleteGlObjects && strokeShaderVertex) {
			glDeleteShader(strokeShaderVertex);
		}
		strokeShaderVertex = 0;
		if (deleteGlObjects && strokeShaderFragment) {
			glDeleteShader(strokeShaderFragment);
		}
		strokeShaderFragment = 0;
		if (deleteGlObjects && markerShaderVertex) {
			glDeleteShader(markerShaderVertex);
		}
		markerShaderVertex = 0;
		if (deleteGlObjects && markerShaderFragment) {
			glDeleteShader(markerShaderFragment);
		}
		markerShaderFragment = 0;
		if (deleteGlObjects && textureVbo) {
			glDeleteBuffers(1, &textureVbo);
		}
		textureVbo = 0;
		if (deleteGlObjects) {
			glDeleteTextures(GLsizei(curveTextures.size()), curveTextures.data());
		}
		curveTextures.fill(0);
		curveTextureIndex = kCurveTextureRingSize - 1u;
		if (deleteGlObjects && textureProgram) {
			glDeleteProgram(textureProgram);
		}
		textureProgram = 0;
		if (deleteGlObjects && textureVertex) {
			glDeleteShader(textureVertex);
		}
		textureVertex = 0;
		if (deleteGlObjects && textureFragment) {
			glDeleteShader(textureFragment);
		}
		textureFragment = 0;
		shaderUniformViewport = -1;
		shaderVboCapacityBytes = 0;
		shaderReady = false;
		shaderInitAttempted = false;
		strokeShaderVboCapacityBytes = 0;
		strokeUniformViewport = -1;
		strokeShaderReady = false;
		strokeShaderInitAttempted = false;
		markerUniformFillRadius = -1;
		markerUniformOutlineRadius = -1;
		markerUniformOutlineHalfWidth = -1;
		markerShaderReady = false;
		markerShaderInitAttempted = false;
		textureShaderReady = false;
		textureShaderInitAttempted = false;
	}

	void releaseRendererResources(bool deleteGlObjects) {
		if (deleteGlObjects && vbo) {
			glDeleteBuffers(1, &vbo);
		}
		vbo = 0;
		releaseShaderResources(deleteGlObjects);
		rendererVg = nullptr;
	}

	~BifurxSpectrumGLWidget() {
		// DAW plugin editors can destroy/recreate their GL context around the
		// Rack UI. Avoid driver calls from widget teardown; the context owner
		// reclaims these resources when the editor context is destroyed.
		releaseRendererResources(false);
		fixedSurface.reset(false);
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		OpenGlWidget::onContextDestroy(e);
		releaseRendererResources(true);
		fixedSurface.reset(true);
	}

	void onContextCreate(const ContextCreateEvent& e) override {
		OpenGlWidget::onContextCreate(e);
		// Rack module widgets can survive a DAW editor replacement and miss the
		// old scene's destroy event. Never carry GL names into the new context:
		// they can alias unrelated buffers, programs, or textures there.
		releaseRendererResources(false);
		fixedSurface.reset(false);
		shaderRendererActiveLastFrame = false;
		shaderRendererFallbackLastFrame = false;
		expectedCurveShaderActiveLastFrame = false;
		markerShaderActiveLastFrame = false;
		setDirty();
		fixedSurface.markDirty();
	}

	bool ensureShaderReady() {
		if (shaderInitAttempted) {
			return shaderReady;
		}
		shaderInitAttempted = true;

		static const char* const kVertexShaderSrc = R"GLSL(
			#version 120
			attribute vec2 aPos;
			attribute vec4 aColor;
			uniform vec2 uViewport;
			varying vec4 vColor;
			void main() {
				vec2 ndc = vec2((aPos.x / uViewport.x) * 2.0 - 1.0, 1.0 - (aPos.y / uViewport.y) * 2.0);
				gl_Position = vec4(ndc, 0.0, 1.0);
				vColor = aColor;
			}
		)GLSL";

		static const char* const kFragmentShaderSrc = R"GLSL(
			#version 120
			varying vec4 vColor;
			void main() {
				gl_FragColor = vColor;
			}
		)GLSL";

		auto compileShader = [](GLenum type, const char* src) -> GLuint {
			GLuint shader = glCreateShader(type);
			if (!shader) {
				WARN("BifurxGL shader: glCreateShader failed for type=%u", unsigned(type));
				return 0;
			}
			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);
			GLint status = GL_FALSE;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (status != GL_TRUE) {
				GLint logLen = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
				std::vector<char> logBuf(size_t(std::max(logLen, 1)));
				GLsizei written = 0;
				glGetShaderInfoLog(shader, GLsizei(logBuf.size()), &written, logBuf.data());
				WARN("BifurxGL shader compile failed (type=%u): %s", unsigned(type), logBuf.data());
				glDeleteShader(shader);
				return 0;
			}
			return shader;
		};

		shaderVertex = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
		shaderFragment = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
		if (!shaderVertex || !shaderFragment) {
			if (shaderVertex) {
				glDeleteShader(shaderVertex);
				shaderVertex = 0;
			}
			if (shaderFragment) {
				glDeleteShader(shaderFragment);
				shaderFragment = 0;
			}
			return false;
		}

		shaderProgram = glCreateProgram();
		if (!shaderProgram) {
			WARN("BifurxGL shader: glCreateProgram failed");
			glDeleteShader(shaderVertex);
			glDeleteShader(shaderFragment);
			shaderVertex = 0;
			shaderFragment = 0;
			return false;
		}

		glAttachShader(shaderProgram, shaderVertex);
		glAttachShader(shaderProgram, shaderFragment);
		glBindAttribLocation(shaderProgram, 0, "aPos");
		glBindAttribLocation(shaderProgram, 1, "aColor");
		glLinkProgram(shaderProgram);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint logLen = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLen);
			std::vector<char> logBuf(size_t(std::max(logLen, 1)));
			GLsizei written = 0;
			glGetProgramInfoLog(shaderProgram, GLsizei(logBuf.size()), &written, logBuf.data());
			WARN("BifurxGL shader link failed: %s", logBuf.data());
			glDeleteProgram(shaderProgram);
			glDeleteShader(shaderVertex);
			glDeleteShader(shaderFragment);
			shaderProgram = 0;
			shaderVertex = 0;
			shaderFragment = 0;
			return false;
		}

		shaderUniformViewport = glGetUniformLocation(shaderProgram, "uViewport");
		if (shaderUniformViewport < 0) {
			WARN("BifurxGL shader uniform lookup failed: uViewport");
			glDeleteProgram(shaderProgram);
			glDeleteShader(shaderVertex);
			glDeleteShader(shaderFragment);
			shaderProgram = 0;
			shaderVertex = 0;
			shaderFragment = 0;
			return false;
		}

		glGenBuffers(1, &shaderVbo);
		if (!shaderVbo) {
			WARN("BifurxGL shader: glGenBuffers failed");
			glDeleteProgram(shaderProgram);
			glDeleteShader(shaderVertex);
			glDeleteShader(shaderFragment);
			shaderProgram = 0;
			shaderVertex = 0;
			shaderFragment = 0;
			return false;
		}

		shaderReady = true;
		return true;
	}

	bool ensureStrokeShaderReady() {
		if (strokeShaderInitAttempted) {
			return strokeShaderReady;
		}
		strokeShaderInitAttempted = true;

		static const char* const kVertexShaderSrc = R"GLSL(
			#version 120
			attribute vec2 aPos;
			attribute vec4 aColor;
			attribute float aSideDist;
			attribute float aRadius;
			uniform vec2 uViewport;
			varying vec4 vColor;
			varying float vSideDist;
			varying float vRadius;
			void main() {
				vec2 ndc = vec2((aPos.x / uViewport.x) * 2.0 - 1.0, 1.0 - (aPos.y / uViewport.y) * 2.0);
				gl_Position = vec4(ndc, 0.0, 1.0);
				vColor = aColor;
				vSideDist = aSideDist;
				vRadius = aRadius;
			}
		)GLSL";

		static const char* const kFragmentShaderSrc = R"GLSL(
			#version 120
			varying vec4 vColor;
			varying float vSideDist;
			varying float vRadius;
			void main() {
				float radius = max(vRadius, 0.25);
				float dist = abs(vSideDist);
				float aa = clamp(fwidth(vSideDist), 0.35, 0.75);
				float coverage = 1.0 - smoothstep(radius - aa, radius + aa, dist);
				float alpha = clamp(vColor.a * coverage, 0.0, 1.0);
				gl_FragColor = vec4(vColor.rgb, alpha);
			}
		)GLSL";

		auto compileShader = [](GLenum type, const char* src) -> GLuint {
			GLuint shader = glCreateShader(type);
			if (!shader) {
				WARN("BifurxGL stroke shader: glCreateShader failed for type=%u", unsigned(type));
				return 0;
			}
			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);
			GLint status = GL_FALSE;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (status != GL_TRUE) {
				GLint logLen = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
				std::vector<char> logBuf(size_t(std::max(logLen, 1)));
				GLsizei written = 0;
				glGetShaderInfoLog(shader, GLsizei(logBuf.size()), &written, logBuf.data());
				WARN("BifurxGL stroke shader compile failed (type=%u): %s", unsigned(type), logBuf.data());
				glDeleteShader(shader);
				return 0;
			}
			return shader;
		};

		strokeShaderVertex = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
		strokeShaderFragment = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
		if (!strokeShaderVertex || !strokeShaderFragment) {
			if (strokeShaderVertex) {
				glDeleteShader(strokeShaderVertex);
				strokeShaderVertex = 0;
			}
			if (strokeShaderFragment) {
				glDeleteShader(strokeShaderFragment);
				strokeShaderFragment = 0;
			}
			return false;
		}

		strokeShaderProgram = glCreateProgram();
		if (!strokeShaderProgram) {
			glDeleteShader(strokeShaderVertex);
			glDeleteShader(strokeShaderFragment);
			strokeShaderVertex = 0;
			strokeShaderFragment = 0;
			return false;
		}
		glAttachShader(strokeShaderProgram, strokeShaderVertex);
		glAttachShader(strokeShaderProgram, strokeShaderFragment);
		glBindAttribLocation(strokeShaderProgram, 0, "aPos");
		glBindAttribLocation(strokeShaderProgram, 1, "aColor");
		glBindAttribLocation(strokeShaderProgram, 2, "aSideDist");
		glBindAttribLocation(strokeShaderProgram, 3, "aRadius");
		glLinkProgram(strokeShaderProgram);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(strokeShaderProgram, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint logLen = 0;
			glGetProgramiv(strokeShaderProgram, GL_INFO_LOG_LENGTH, &logLen);
			std::vector<char> logBuf(size_t(std::max(logLen, 1)));
			GLsizei written = 0;
			glGetProgramInfoLog(strokeShaderProgram, GLsizei(logBuf.size()), &written, logBuf.data());
			WARN("BifurxGL stroke shader link failed: %s", logBuf.data());
			glDeleteProgram(strokeShaderProgram);
			glDeleteShader(strokeShaderVertex);
			glDeleteShader(strokeShaderFragment);
			strokeShaderProgram = 0;
			strokeShaderVertex = 0;
			strokeShaderFragment = 0;
			return false;
		}
		strokeUniformViewport = glGetUniformLocation(strokeShaderProgram, "uViewport");
		if (strokeUniformViewport < 0) {
			WARN("BifurxGL stroke shader uniform lookup failed: uViewport");
			glDeleteProgram(strokeShaderProgram);
			glDeleteShader(strokeShaderVertex);
			glDeleteShader(strokeShaderFragment);
			strokeShaderProgram = 0;
			strokeShaderVertex = 0;
			strokeShaderFragment = 0;
			return false;
		}

		glGenBuffers(1, &strokeShaderVbo);
		if (!strokeShaderVbo) {
			glDeleteProgram(strokeShaderProgram);
			glDeleteShader(strokeShaderVertex);
			glDeleteShader(strokeShaderFragment);
			strokeShaderProgram = 0;
			strokeShaderVertex = 0;
			strokeShaderFragment = 0;
			return false;
		}

		strokeShaderReady = true;
		return true;
	}

	bool ensureMarkerShaderReady() {
		if (markerShaderInitAttempted) return markerShaderReady;
		markerShaderInitAttempted = true;

		static const char* const kVertexShaderSrc = R"GLSL(
			#version 120
			varying vec2 vLocal;
			void main() {
				gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
				vLocal = gl_MultiTexCoord0.xy;
			}
		)GLSL";
		static const char* const kFragmentShaderSrc = R"GLSL(
			#version 120
			varying vec2 vLocal;
			uniform float uFillRadius;
			uniform float uOutlineRadius;
			uniform float uOutlineHalfWidth;
			void main() {
				float distancePx = length(vLocal);
				float aa = 0.65;
				float fillCoverage = 1.0 - smoothstep(
					uFillRadius - aa, uFillRadius + aa, distancePx);
				float ringInner = smoothstep(
					uOutlineRadius - uOutlineHalfWidth - aa,
					uOutlineRadius - uOutlineHalfWidth + aa, distancePx);
				float ringOuter = 1.0 - smoothstep(
					uOutlineRadius + uOutlineHalfWidth - aa,
					uOutlineRadius + uOutlineHalfWidth + aa, distancePx);
				vec4 fill = vec4(252.0 / 255.0, 1.0, 1.0, (244.0 / 255.0) * fillCoverage);
				vec4 ring = vec4(8.0 / 255.0, 10.0 / 255.0, 14.0 / 255.0,
					(220.0 / 255.0) * ringInner * ringOuter);
				float outA = ring.a + fill.a * (1.0 - ring.a);
				vec3 outRgb = outA > 0.00001
					? (ring.rgb * ring.a + fill.rgb * fill.a * (1.0 - ring.a)) / outA
					: vec3(0.0);
				gl_FragColor = vec4(outRgb, outA);
			}
		)GLSL";

		auto compileShader = [](GLenum type, const char* src) -> GLuint {
			GLuint shader = glCreateShader(type);
			if (!shader) return 0;
			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);
			GLint status = GL_FALSE;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (status != GL_TRUE) {
				glDeleteShader(shader);
				return 0;
			}
			return shader;
		};

		markerShaderVertex = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
		markerShaderFragment = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
		if (!markerShaderVertex || !markerShaderFragment) return false;
		markerShaderProgram = glCreateProgram();
		if (!markerShaderProgram) return false;
		glAttachShader(markerShaderProgram, markerShaderVertex);
		glAttachShader(markerShaderProgram, markerShaderFragment);
		glLinkProgram(markerShaderProgram);
		GLint linked = GL_FALSE;
		glGetProgramiv(markerShaderProgram, GL_LINK_STATUS, &linked);
		if (linked != GL_TRUE) return false;
		markerUniformFillRadius = glGetUniformLocation(markerShaderProgram, "uFillRadius");
		markerUniformOutlineRadius = glGetUniformLocation(markerShaderProgram, "uOutlineRadius");
		markerUniformOutlineHalfWidth = glGetUniformLocation(markerShaderProgram, "uOutlineHalfWidth");
		markerShaderReady = markerUniformFillRadius >= 0
			&& markerUniformOutlineRadius >= 0
			&& markerUniformOutlineHalfWidth >= 0;
		return markerShaderReady;
	}

	bool ensureTextureShaderReady() {
		if (textureShaderInitAttempted) {
			return textureShaderReady;
		}
		textureShaderInitAttempted = true;

		static const char* const kVertexShaderSrc = R"GLSL(
			#version 120
			attribute vec2 aPos;
			uniform vec2 uViewport;
			varying vec2 vLocalPos;
			void main() {
				vec2 ndc = vec2((aPos.x / uViewport.x) * 2.0 - 1.0, 1.0 - (aPos.y / uViewport.y) * 2.0);
				gl_Position = vec4(ndc, 0.0, 1.0);
				vLocalPos = aPos;
			}
		)GLSL";

		static const char* const kFragmentShaderSrc = R"GLSL(
			#version 120
			varying vec2 vLocalPos;
			uniform sampler2D uCurveTex;
			uniform vec4 uExpectedWhite;
			uniform vec4 uExpectedCyan;
			uniform vec4 uExpectedPurple;
			uniform float uDisplayOnlyMode;
			uniform float uDisplayOnlyShapeControl;
			uniform float uSpectrumTopY;
			uniform float uSpectrumBottomY;
			uniform float uPlotWidth;
			uniform float uPlotHeight;

			vec4 mixColor(vec4 c1, vec4 c2, float t) {
				return mix(c1, c2, t);
			}

			float displayOnlyColorTone(float energy, float shape) {
				float tone = 0.5 + 0.5 * shape;
				float factor = 1.0 - energy;
				return clamp(tone - 0.25 * factor, 0.0, 1.0);
			}

			void main() {
				float x01 = clamp(vLocalPos.x / uPlotWidth, 0.0, 1.0);
				vec4 texColor = texture2D(uCurveTex, vec2(x01, 0.5));
				float curveY = texColor.r * uPlotHeight;
				float avgD = texColor.g * 36.0 - 18.0;
				float energy = texColor.b;

				if (energy <= 0.005) {
					discard;
				}

				vec4 fill;
				if (uDisplayOnlyMode > 0.5) {
					float tone = displayOnlyColorTone(energy, uDisplayOnlyShapeControl);
					fill = mixColor(uExpectedPurple, uExpectedCyan, tone);
				} else {
					float posA = clamp(avgD / 18.0, 0.0, 1.0);
					float negA = clamp(-avgD / 18.0, 0.0, 1.0);
					vec4 tint = uExpectedWhite;
					if (posA > 0.0) {
						tint = mixColor(tint, uExpectedCyan, clamp(posA * 1.40, 0.0, 1.0));
					}
					if (negA > 0.0) {
						tint = mixColor(tint, uExpectedPurple, clamp(negA * 1.25, 0.0, 1.0));
					}
					fill = mixColor(uExpectedWhite, tint, 0.55 + 0.45 * energy);
				}

				float topAlpha = clamp(0.78 + 0.18 * energy, 0.78, 0.96);
				float capAlphaNear = clamp(0.08 + 0.18 * energy, 0.0, 0.26);
				float capAlphaFar = clamp(0.03 + 0.10 * energy, 0.0, 0.13);

				float fillAlpha = 0.0;
				if (vLocalPos.y < uSpectrumTopY || vLocalPos.y > uSpectrumBottomY) {
					discard;
				}

				float signedDist = vLocalPos.y - curveY;
				float aaPx = clamp(fwidth(signedDist), 0.85, 2.2);
				float bodyT = (uSpectrumBottomY > curveY) ? clamp(signedDist / (uSpectrumBottomY - curveY), 0.0, 1.0) : 1.0;
				float bodyAlpha = mix(topAlpha, 1.0, bodyT);
				float crestDist = max(-signedDist, 0.0);
				float alphaNear = capAlphaNear * clamp(1.0 - crestDist / 1.8, 0.0, 1.0);
				float alphaFar = capAlphaFar * clamp(1.0 - crestDist / 3.4, 0.0, 1.0);
				float featherAlpha = alphaNear + alphaFar;
				float bodyCoverage = smoothstep(-aaPx, aaPx, signedDist);
				fillAlpha = mix(featherAlpha, bodyAlpha, bodyCoverage);

				float crestAlpha = clamp(0.16 + 0.18 * energy, 0.0, 0.34);
				float crestRadius = 1.05 + 0.45 * energy;
				vec4 crestColor = mixColor(fill, vec4(236.0/255.0, 244.0/255.0, 250.0/255.0, 1.0), 0.18);

				float dist = abs(vLocalPos.y - curveY);
				float sigma = max(crestRadius * 0.56, 0.001);
				float crestCoverage = exp(-0.5 * (dist * dist) / (sigma * sigma));
				float crestAlphaOut = clamp(crestAlpha * crestCoverage, 0.0, 1.0);

				vec4 cFill = vec4(fill.rgb, fillAlpha);
				vec4 cCrest = vec4(crestColor.rgb, crestAlphaOut);

				float blendedAlpha = cCrest.a + cFill.a * (1.0 - cCrest.a);
				vec3 blendedColor = (blendedAlpha > 0.0001)
					? (cCrest.rgb * cCrest.a + cFill.rgb * cFill.a * (1.0 - cCrest.a)) / blendedAlpha
					: vec3(0.0);
				gl_FragColor = vec4(blendedColor, blendedAlpha);
			}
		)GLSL";

		auto compileShader = [](GLenum type, const char* src) -> GLuint {
			GLuint shader = glCreateShader(type);
			if (!shader) {
				WARN("BifurxGL texture shader: glCreateShader failed for type=%u", unsigned(type));
				return 0;
			}
			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);
			GLint status = GL_FALSE;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
			if (status != GL_TRUE) {
				GLint logLen = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
				std::vector<char> logBuf(size_t(std::max(logLen, 1)));
				GLsizei written = 0;
				glGetShaderInfoLog(shader, GLsizei(logBuf.size()), &written, logBuf.data());
				WARN("BifurxGL texture shader compile failed (type=%u): %s", unsigned(type), logBuf.data());
				glDeleteShader(shader);
				return 0;
			}
			return shader;
		};

		textureVertex = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
		textureFragment = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
		if (!textureVertex || !textureFragment) {
			if (textureVertex) { glDeleteShader(textureVertex); textureVertex = 0; }
			if (textureFragment) { glDeleteShader(textureFragment); textureFragment = 0; }
			return false;
		}

		textureProgram = glCreateProgram();
		if (!textureProgram) {
			glDeleteShader(textureVertex);
			glDeleteShader(textureFragment);
			textureVertex = 0;
			textureFragment = 0;
			return false;
		}

		glAttachShader(textureProgram, textureVertex);
		glAttachShader(textureProgram, textureFragment);
		glBindAttribLocation(textureProgram, 0, "aPos");
		glLinkProgram(textureProgram);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(textureProgram, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE) {
			GLint logLen = 0;
			glGetProgramiv(textureProgram, GL_INFO_LOG_LENGTH, &logLen);
			std::vector<char> logBuf(size_t(std::max(logLen, 1)));
			GLsizei written = 0;
			glGetProgramInfoLog(textureProgram, GLsizei(logBuf.size()), &written, logBuf.data());
			WARN("BifurxGL texture shader link failed: %s", logBuf.data());
			glDeleteProgram(textureProgram);
			glDeleteShader(textureVertex);
			glDeleteShader(textureFragment);
			textureProgram = 0;
			textureVertex = 0;
			textureFragment = 0;
			return false;
		}

		textureUniformViewport = glGetUniformLocation(textureProgram, "uViewport");
		textureUniformCurveTex = glGetUniformLocation(textureProgram, "uCurveTex");
		textureUniformExpectedWhite = glGetUniformLocation(textureProgram, "uExpectedWhite");
		textureUniformExpectedCyan = glGetUniformLocation(textureProgram, "uExpectedCyan");
		textureUniformExpectedPurple = glGetUniformLocation(textureProgram, "uExpectedPurple");
		textureUniformDisplayOnlyMode = glGetUniformLocation(textureProgram, "uDisplayOnlyMode");
		textureUniformDisplayOnlyShapeControl = glGetUniformLocation(textureProgram, "uDisplayOnlyShapeControl");
		textureUniformSpectrumTopY = glGetUniformLocation(textureProgram, "uSpectrumTopY");
		textureUniformSpectrumBottomY = glGetUniformLocation(textureProgram, "uSpectrumBottomY");
		textureUniformPlotWidth = glGetUniformLocation(textureProgram, "uPlotWidth");
		textureUniformPlotHeight = glGetUniformLocation(textureProgram, "uPlotHeight");

		glGenBuffers(1, &textureVbo);
		glGenTextures(GLsizei(curveTextures.size()), curveTextures.data());

		for (GLuint curveTexture : curveTextures) {
			glBindTexture(GL_TEXTURE_2D, curveTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// Pre-allocate every ring slot so frame updates only replace texels.
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, kCurvePointCount, 1, 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		textureShaderReady = true;
		return true;
	}

	void appendStrokePolyline(const std::vector<GlVertex>& lineVerts, float radius, std::vector<GlStrokeQuadVertex>* out) {
		if (!out) return;
		if (lineVerts.size() < 2) return;

		const float pad = std::max(radius * 3.f, 0.75f);
		struct StrokePair {
			GlStrokeQuadVertex left;
			GlStrokeQuadVertex right;
		};

		auto normalized = [](float x, float y) {
			const float lenSq = x * x + y * y;
			if (lenSq <= 1e-12f) {
				return Vec(0.f, 0.f);
			}
			const float invLen = 1.f / std::sqrt(lenSq);
			return Vec(x * invLen, y * invLen);
		};
		auto isNearlyZero = [](const Vec& v) {
			return (v.x * v.x + v.y * v.y) <= 1e-12f;
		};
		auto leftNormal = [&](const GlVertex& a, const GlVertex& b) {
			Vec dir = normalized(b.x - a.x, b.y - a.y);
			if (isNearlyZero(dir)) {
				return Vec(0.f, -1.f);
			}
			return Vec(-dir.y, dir.x);
		};
		auto leftNormalFromDir = [](const Vec& dir) {
			return Vec(-dir.y, dir.x);
		};
		auto isSevereJoin = [&](size_t i) {
			if (i == 0u || i + 1u >= lineVerts.size()) return false;
			const Vec prevDir = normalized(
				lineVerts[i].x - lineVerts[i - 1u].x,
				lineVerts[i].y - lineVerts[i - 1u].y);
			const Vec nextDir = normalized(
				lineVerts[i + 1u].x - lineVerts[i].x,
				lineVerts[i + 1u].y - lineVerts[i].y);
			if (isNearlyZero(prevDir) || isNearlyZero(nextDir)) return true;
			return prevDir.x * nextDir.x + prevDir.y * nextDir.y < 0.25f;
		};

		auto makeStrokePair = [&](size_t i) {
			const GlVertex& p = lineVerts[i];
			Vec normal(0.f, -1.f);
			float miterScale = pad;
			if (i == 0) {
				normal = leftNormal(lineVerts[0], lineVerts[1]);
			}
			else if (i + 1 == lineVerts.size()) {
				normal = leftNormal(lineVerts[i - 1], lineVerts[i]);
			}
			else {
				const Vec prevDir = normalized(lineVerts[i].x - lineVerts[i - 1].x, lineVerts[i].y - lineVerts[i - 1].y);
				const Vec nextDir = normalized(lineVerts[i + 1].x - lineVerts[i].x, lineVerts[i + 1].y - lineVerts[i].y);
				const Vec prevNormal = leftNormalFromDir(prevDir);
				const Vec nextNormal = leftNormalFromDir(nextDir);
				const float dirDot = clamp(prevDir.x * nextDir.x + prevDir.y * nextDir.y, -1.f, 1.f);
				Vec miter = normalized(prevNormal.x + nextNormal.x, prevNormal.y + nextNormal.y);
				bool useBevelFallback = false;
				if (isNearlyZero(prevDir) || isNearlyZero(nextDir) || isNearlyZero(miter)) {
					useBevelFallback = true;
				}
				float denom = 0.f;
				if (!useBevelFallback) {
					denom = std::fabs(miter.x * nextNormal.x + miter.y * nextNormal.y);
					if (dirDot < 0.25f || denom <= 0.45f) {
						useBevelFallback = true;
					}
				}
				if (useBevelFallback) {
					normal = nextNormal;
					miterScale = pad;
				}
				else {
					normal = miter;
					miterScale = std::min(pad / denom, pad * 1.55f);
				}
			}

			const float ox = normal.x * miterScale;
			const float oy = normal.y * miterScale;
			return StrokePair {
				{p.x - ox, p.y - oy, p.r, p.g, p.b, p.a, -pad, radius},
				{p.x + ox, p.y + oy, p.r, p.g, p.b, p.a, pad, radius}
			};
		};

		StrokePair a = makeStrokePair(0u);
		for (size_t i = 1; i < lineVerts.size(); ++i) {
			if (isSevereJoin(i)) {
				// A tight notch can reverse direction over less than one screen pixel.
				// Connecting the opposing side pairs folds the miter strip across
				// itself, so terminate the incoming segment and restart at the cusp.
				appendStrokeSegment(lineVerts[i - 1u], lineVerts[i], radius, out);
				a = makeStrokePair(i);
				continue;
			}
			const StrokePair b = makeStrokePair(i);
			out->push_back(a.left);
			out->push_back(a.right);
			out->push_back(b.right);
			out->push_back(a.left);
			out->push_back(b.right);
			out->push_back(b.left);
			a = b;
		}
	}

	void drawExpectedCurveShader(float w, float h) {
		expectedCurveShaderActiveLastFrame = false;
		if (!state.hasPreview || !ensureStrokeShaderReady()) return;

		calculateRefinedCurvePoints(&overlayCurvePoints, w, h);
		if (overlayCurvePoints.size() < 2u) return;
		BifurxMarkerLayout layout;
		getCachedMarkerLayout(&layout, w, h);
		const float padY = std::max(4.f, h * 0.035f);
		const float labelBandHeight = std::max(5.2f, h * 0.072f);
		const float spectrumBottomY = std::max(
			padY * 0.35f + 1.f,
			h - labelBandHeight - std::max(0.05f, h * 0.0008f));

		expectedCurveLineVertices.clear();
		expectedCurveStrokeVertices.clear();
		auto appendGuideLayer = [&](float r, float g, float b, float a, float radius) {
			for (int i = 0; i < 2; ++i) {
				if (!layout.markers[i].visible) continue;
				appendStrokeSegment(
					{layout.markers[i].x, spectrumBottomY, r, g, b, a},
					{layout.markers[i].x, layout.markers[i].yMarker, r, g, b, a},
					radius, &expectedCurveStrokeVertices);
				appendStrokeSegment(
					{layout.markers[i].x, layout.markers[i].yMarker + kPeakMarkerFillRadius + 0.45f, r, g, b, a},
					{layout.markers[i].x, layout.guideYBottom, r, g, b, a},
					radius, &expectedCurveStrokeVertices);
			}
		};
		for (const BifurxCurvePoint& point : overlayCurvePoints) {
			expectedCurveLineVertices.push_back({
				w * point.x01, point.y,
				6.f / 255.f, 8.f / 255.f, 12.f / 255.f, 230.f / 255.f
			});
		}
		appendStrokePolyline(expectedCurveLineVertices, 1.45f, &expectedCurveStrokeVertices);
		appendGuideLayer(6.f / 255.f, 8.f / 255.f, 12.f / 255.f, 230.f / 255.f, 1.40f);

		for (GlVertex& point : expectedCurveLineVertices) {
			point.r = 235.f / 255.f;
			point.g = 204.f / 255.f;
			point.b = 128.f / 255.f;
			point.a = 244.f / 255.f;
		}
		appendStrokePolyline(expectedCurveLineVertices, 0.90f, &expectedCurveStrokeVertices);
		appendGuideLayer(235.f / 255.f, 204.f / 255.f, 128.f / 255.f, 244.f / 255.f, 0.85f);

		for (GlVertex& point : expectedCurveLineVertices) {
			point.r = 255.f / 255.f;
			point.g = 242.f / 255.f;
			point.b = 202.f / 255.f;
			point.a = 250.f / 255.f;
		}
		appendStrokePolyline(expectedCurveLineVertices, 0.38f, &expectedCurveStrokeVertices);
		appendGuideLayer(255.f / 255.f, 242.f / 255.f, 202.f / 255.f, 250.f / 255.f, 0.36f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		drawStrokeQuadsShader(expectedCurveStrokeVertices, w, h);
		expectedCurveShaderActiveLastFrame = true;
	}

	void drawMarkerCirclesShader(float w, float h) {
		markerShaderActiveLastFrame = false;
		if (!state.hasPreview || !ensureMarkerShaderReady()) return;
		BifurxMarkerLayout layout;
		getCachedMarkerLayout(&layout, w, h);
		bool hasVisibleMarker = false;
		for (int i = 0; i < 2; ++i) {
			hasVisibleMarker = hasVisibleMarker || layout.markers[i].visible;
		}
		if (!hasVisibleMarker) return;

		const float outlineRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius;
		const float outlineHalfWidth = 0.5f * kPeakMarkerOutlineStrokeWidth;
		const float extent = outlineRadius + outlineHalfWidth + 0.65f;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(markerShaderProgram);
		glUniform1f(markerUniformFillRadius, kPeakMarkerFillRadius);
		glUniform1f(markerUniformOutlineRadius, outlineRadius);
		glUniform1f(markerUniformOutlineHalfWidth, outlineHalfWidth);
		glBegin(GL_QUADS);
		for (int i = 0; i < 2; ++i) {
			if (!layout.markers[i].visible) continue;
			const float x = layout.markers[i].x;
			const float y = layout.markers[i].yMarker;
			glTexCoord2f(-extent, -extent); glVertex2f(x - extent, y - extent);
			glTexCoord2f( extent, -extent); glVertex2f(x + extent, y - extent);
			glTexCoord2f( extent,  extent); glVertex2f(x + extent, y + extent);
			glTexCoord2f(-extent,  extent); glVertex2f(x - extent, y + extent);
		}
		glEnd();
		glUseProgram(0);
		markerShaderActiveLastFrame = true;
	}

	void appendStrokeSegment(const GlVertex& a, const GlVertex& b, float radius, std::vector<GlStrokeQuadVertex>* out) {
		if (!out) return;
		const float dx = b.x - a.x;
		const float dy = b.y - a.y;
		const float lenSq = dx * dx + dy * dy;
		if (lenSq <= 1e-10f) return;
		const float invLen = 1.f / std::sqrt(lenSq);
		const float nx = -dy * invLen;
		const float ny = dx * invLen;
		const float pad = std::max(radius * 3.f, 0.75f);

		const GlStrokeQuadVertex aLeft {a.x - nx * pad, a.y - ny * pad, a.r, a.g, a.b, a.a, -pad, radius};
		const GlStrokeQuadVertex aRight {a.x + nx * pad, a.y + ny * pad, a.r, a.g, a.b, a.a, pad, radius};
		const GlStrokeQuadVertex bLeft {b.x - nx * pad, b.y - ny * pad, b.r, b.g, b.b, b.a, -pad, radius};
		const GlStrokeQuadVertex bRight {b.x + nx * pad, b.y + ny * pad, b.r, b.g, b.b, b.a, pad, radius};
		out->push_back(aLeft);
		out->push_back(aRight);
		out->push_back(bRight);
		out->push_back(aLeft);
		out->push_back(bRight);
		out->push_back(bLeft);
	}

	void drawStrokeQuadsShader(const std::vector<GlStrokeQuadVertex>& verts, float w, float h) {
		if (!strokeShaderReady || verts.empty()) return;
		glUseProgram(strokeShaderProgram);
		glUniform2f(strokeUniformViewport, std::max(w, 1.f), std::max(h, 1.f));
		glBindBuffer(GL_ARRAY_BUFFER, strokeShaderVbo);
		const GLsizeiptr bytes = GLsizeiptr(verts.size() * sizeof(GlStrokeQuadVertex));
		if (bytes > strokeShaderVboCapacityBytes) {
			glBufferData(GL_ARRAY_BUFFER, bytes, verts.data(), GL_DYNAMIC_DRAW);
			strokeShaderVboCapacityBytes = bytes;
		}
		else {
			glBufferData(GL_ARRAY_BUFFER, strokeShaderVboCapacityBytes, nullptr, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, verts.data());
		}
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GlStrokeQuadVertex), (const GLvoid*) offsetof(GlStrokeQuadVertex, x));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GlStrokeQuadVertex), (const GLvoid*) offsetof(GlStrokeQuadVertex, r));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GlStrokeQuadVertex), (const GLvoid*) offsetof(GlStrokeQuadVertex, sideDist));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GlStrokeQuadVertex), (const GLvoid*) offsetof(GlStrokeQuadVertex, radius));
		glDrawArrays(GL_TRIANGLES, 0, GLsizei(verts.size()));
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}

	void drawVertsShader(const std::vector<GlVertex>& verts, GLenum primitive, float lineWidth, float w, float h) {
		if (!shaderReady || verts.empty()) return;
		glUseProgram(shaderProgram);
		glUniform2f(shaderUniformViewport, std::max(w, 1.f), std::max(h, 1.f));
		glBindBuffer(GL_ARRAY_BUFFER, shaderVbo);
		const GLsizeiptr bytes = GLsizeiptr(verts.size() * sizeof(GlVertex));
		if (bytes > shaderVboCapacityBytes) {
			glBufferData(GL_ARRAY_BUFFER, bytes, verts.data(), GL_DYNAMIC_DRAW);
			shaderVboCapacityBytes = bytes;
		}
		else {
			glBufferData(GL_ARRAY_BUFFER, shaderVboCapacityBytes, nullptr, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, verts.data());
		}
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GlVertex), (const GLvoid*) offsetof(GlVertex, x));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GlVertex), (const GLvoid*) offsetof(GlVertex, r));
		if (primitive == GL_LINE_STRIP || primitive == GL_LINES) {
			glLineWidth(std::max(1.f, lineWidth));
		}
		glDrawArrays(primitive, 0, GLsizei(verts.size()));
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}

	void validateShaderResourcesForCurrentContext() {
		if (shaderReady && !gl_lifecycle::isValidProgramBufferPair(shaderProgram, shaderVbo)) {
			shaderProgram = 0;
			shaderVbo = 0;
			shaderVertex = 0;
			shaderFragment = 0;
			shaderUniformViewport = -1;
			shaderVboCapacityBytes = 0;
			shaderReady = false;
			shaderInitAttempted = false;
		}
		if (strokeShaderReady &&
			!gl_lifecycle::isValidProgramBufferPair(strokeShaderProgram, strokeShaderVbo)) {
			strokeShaderProgram = 0;
			strokeShaderVbo = 0;
			strokeShaderVertex = 0;
			strokeShaderFragment = 0;
			strokeUniformViewport = -1;
			strokeShaderVboCapacityBytes = 0;
			strokeShaderReady = false;
			strokeShaderInitAttempted = false;
		}
		if (markerShaderReady && (markerShaderProgram == 0 || !glIsProgram(markerShaderProgram))) {
			markerShaderProgram = 0;
			markerShaderVertex = 0;
			markerShaderFragment = 0;
			markerUniformFillRadius = -1;
			markerUniformOutlineRadius = -1;
			markerUniformOutlineHalfWidth = -1;
			markerShaderReady = false;
			markerShaderInitAttempted = false;
		}
		if (textureShaderReady &&
			(!gl_lifecycle::isValidProgramBufferPair(textureProgram, textureVbo) ||
			 !gl_lifecycle::areValidTextures({curveTextures[0], curveTextures[1], curveTextures[2]}))) {
			textureProgram = 0;
			textureVbo = 0;
			curveTextures.fill(0);
			curveTextureIndex = kCurveTextureRingSize - 1u;
			textureVertex = 0;
			textureFragment = 0;
			textureShaderReady = false;
			textureShaderInitAttempted = false;
		}
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
		if (!isVisible()) return;
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = isDragonKingDebugEnabled();
		const PerfClock::time_point perfStepStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		// OpenGlWidget::step() dirties its framebuffer unconditionally. Bifurx
		// already has an explicit invalidation policy below, so retain the cached
		// image between meaningful visual changes. This is especially important
		// while Rack animates between zoom levels, when unconditional redraws can
		// amplify framebuffer resize stalls.
		widget::FramebufferWidget::step();
		if (!module) return;
		if (module->renderMode != Bifurx::RENDER_OPENGL) return;

		float uiFrameSec = 1.f / 60.f;
		if (APP && APP->window) {
			const float frameSec = float(APP->window->getLastFrameDuration());
			if (std::isfinite(frameSec) && frameSec > 0.f) {
				uiFrameSec = clamp(frameSec, 1.f / 240.f, 1.f / 20.f);
			}
		}
		BifurxRenderTickResult tick;
		tick = runRenderTick(uiFrameSec);
		if (tick.curvePrepUs > 0.f) {
			lastCurvePrepUs = tick.curvePrepUs;
		}
		if (tick.overlayPrepUs > 0.f) {
			lastOverlayPrepUs = tick.overlayPrepUs;
		}
		const bool showModuleResponseOverlayNow = module->showModuleResponseOverlay.load(std::memory_order_relaxed);
		const bool useGlShaderRendererNow = module->useGlShaderRenderer.load(std::memory_order_relaxed);
		const int colorSchemeNow = int(module->colorScheme);
		const bool fixedSurfaceEnabledNow = module->fixedGlSurfaceEnabled.load(std::memory_order_relaxed);
		bool contentDirty = tick.previewUpdated || tick.analysisUpdated || tick.animationActive;

		// Shared dirty policy with NanoVG path: redraw on new data or active animation.
		if (showModuleResponseOverlayNow != lastShowModuleResponseOverlay) {
			lastShowModuleResponseOverlay = showModuleResponseOverlayNow;
			contentDirty = true;
		}
		if (useGlShaderRendererNow != lastUseGlShaderRenderer) {
			lastUseGlShaderRenderer = useGlShaderRendererNow;
			contentDirty = true;
		}
		if (colorSchemeNow != lastColorScheme) {
			lastColorScheme = colorSchemeNow;
			contentDirty = true;
		}
		const bool threeColorFftGradientNow = module->threeColorFftGradient.load(std::memory_order_relaxed);
		if (threeColorFftGradientNow != lastThreeColorFftGradient) {
			lastThreeColorFftGradient = threeColorFftGradientNow;
			contentDirty = true;
		}
		if (fixedSurfaceEnabledNow != lastFixedSurfaceEnabled) {
			lastFixedSurfaceEnabled = fixedSurfaceEnabledNow;
			contentDirty = true;
		}
		if (contentDirty) {
			fixedSurface.markDirty();
			setDirty();
		}
		// Rack owns the current UI GL context during step(), before NanoVG begins
		// the visible frame. draw() later samples only the completed front image.
		if (fixedSurfaceEnabledNow) {
			renderFixedSurfaceIfNeeded();
		}
		if (measurePerf) {
			const float stepMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - perfStepStart).count()) * 1e-6f;
			lastStepMsEma = (lastStepMsEma > 0.f) ? (lastStepMsEma + (stepMs - lastStepMsEma) * 0.18f) : stepMs;
		}
	}

	void renderGlContent(Vec fbSize, int viewportY = 0) {
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = isDragonKingDebugEnabled();
		const PerfClock::time_point perfDrawStart = measurePerf ? PerfClock::now() : PerfClock::time_point();

		if (!module || module->renderMode != Bifurx::RENDER_OPENGL) return;
		if (!vbo) glGenBuffers(1, &vbo);
		if (isExtraGlValidationEnabled()) {
			validateShaderResourcesForCurrentContext();
		}

		const int activeWidth = std::max(1, int(std::lround(fbSize.x)));
		const int activeHeight = std::max(1, int(std::lround(fbSize.y)));
		glViewport(0, viewportY, activeWidth, activeHeight);
		// The adaptive surface keeps a maximum-density backing allocation. Limit
		// clears to the active zoom-dependent prefix instead of clearing the full
		// 3x-capacity texture every animated frame.
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, viewportY, activeWidth, activeHeight);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);

		const float w = box.size.x, h = box.size.y;
		if (!(w > 0.f && h > 0.f)) return;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, w, h, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		const float padY = std::max(4.f, h * 0.035f);
		const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
		const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
		
		const float displayMaxDbfs = state.displayTopDbfs;
		const float displayMinDbfs = displayMaxDbfs - kDisplayDbfsSpan;
		auto spectrumYForDbfs = [&](float dbfs) { return rescale(clamp(dbfs, displayMinDbfs, displayMaxDbfs), displayMinDbfs, displayMaxDbfs, spectrumBottomY, spectrumTopY); };
		const bool displayOnlyMode = isBifurxDisplayOnlyMode(state.previewState.mode);
		const bool useShaderRenderer = module->useGlShaderRenderer.load(std::memory_order_relaxed) && ensureTextureShaderReady();
		shaderRendererActiveLastFrame = useShaderRenderer;
		shaderRendererFallbackLastFrame = module->useGlShaderRenderer.load(std::memory_order_relaxed) && !useShaderRenderer;
		expectedCurveShaderActiveLastFrame = false;
		markerShaderActiveLastFrame = false;

		fillVertices.clear();
		fillSoftCapVertices.clear();
		fillCrestLineVertices.clear();
		fillCrestStrokeVertices.clear();
		expectedCurveStrokeVertices.clear();
		const float displayOnlyShapeControl = module ? clamp(module->params[Bifurx::FM_AMT_PARAM].getValue(), -1.f, 1.f) : 0.f;

		if (useShaderRenderer) {
			if (state.hasOverlay) {
				// 1. Fill persistent texel buffer (no heap allocation)
				for (int i = 0; i < kCurvePointCount; ++i) {
					const float y = spectrumYForDbfs(state.overlayOutputDbfs[i]);
					const float normY   = clamp(y / h, 0.f, 1.f);
					const float normD   = clamp((state.overlayModuleDb[i] + 18.f) / 36.f, 0.f, 1.f);
					const float energy  = clamp((state.overlayOutputDbfs[i] - displayMinDbfs) / (displayMaxDbfs - displayMinDbfs), 0.f, 1.f);
					const size_t base = size_t(i) * 4u;
					curveTexels[base + 0] = static_cast<uint16_t>(normY  * 65535.f + 0.5f);
					curveTexels[base + 1] = static_cast<uint16_t>(normD  * 65535.f + 0.5f);
					curveTexels[base + 2] = static_cast<uint16_t>(energy * 65535.f + 0.5f);
					curveTexels[base + 3] = 65535;
				}

				// 2. Update pre-allocated texture (no storage reallocation)
				curveTextureIndex = (curveTextureIndex + 1u) % curveTextures.size();
				const GLuint curveTexture = curveTextures[curveTextureIndex];
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, curveTexture);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kCurvePointCount, 1, GL_RGBA, GL_UNSIGNED_SHORT, curveTexels.data());

				// 3. Set up uniforms and program
				glUseProgram(textureProgram);
				glUniform2f(textureUniformViewport, std::max(w, 1.f), std::max(h, 1.f));
				glUniform1i(textureUniformCurveTex, 0);

				const BifurxColors palette = BifurxColors::get(
					module ? module->colorScheme : Bifurx::SCHEME_DEFAULT,
					module ? module->threeColorFftGradient.load(std::memory_order_relaxed) : false);
				glUniform4f(textureUniformExpectedWhite, palette.white.r, palette.white.g, palette.white.b, palette.white.a);
				glUniform4f(textureUniformExpectedCyan, palette.high.r, palette.high.g, palette.high.b, palette.high.a);
				glUniform4f(textureUniformExpectedPurple, palette.low.r, palette.low.g, palette.low.b, palette.low.a);
				glUniform1f(textureUniformDisplayOnlyMode, displayOnlyMode ? 1.f : 0.f);
				glUniform1f(textureUniformDisplayOnlyShapeControl, displayOnlyShapeControl);
				glUniform1f(textureUniformSpectrumTopY, spectrumTopY);
				glUniform1f(textureUniformSpectrumBottomY, spectrumBottomY);
				glUniform1f(textureUniformPlotWidth, w);
				glUniform1f(textureUniformPlotHeight, h);

				// 4. Draw quad
				struct SimpleVertex {
					float x, y;
				};
				std::array<SimpleVertex, 4> quadVerts = {{
					{0.f, 0.f},
					{w, 0.f},
					{0.f, spectrumBottomY},
					{w, spectrumBottomY}
				}};

				glBindBuffer(GL_ARRAY_BUFFER, textureVbo);
				// Orphan VBO before upload
				glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), nullptr, GL_DYNAMIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVerts), quadVerts.data());

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SimpleVertex), (const GLvoid*)0);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

				glDisableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindTexture(GL_TEXTURE_2D, 0);
				glUseProgram(0);
			}
		}
		else {
			// CPU geometry generation logic fallback
			if (state.hasOverlay) {
				for (int i = 0; i < kCurvePointCount - 1; i++) {
					const float avgD = 0.5f * (state.overlayModuleDb[i] + state.overlayModuleDb[i + 1]);
					const float avgO = 0.5f * (state.overlayOutputDbfs[i] + state.overlayOutputDbfs[i + 1]);
					const float energy = levi_math::clamp01(rescale(avgO, displayMinDbfs, displayMaxDbfs, 0.f, 1.f));
					if (energy <= 0.005f) continue;
					
					float posA = levi_math::clamp01(avgD / 18.f), negA = levi_math::clamp01(-avgD / 18.f);
					const BifurxColors palette = BifurxColors::get(
						module ? module->colorScheme : Bifurx::SCHEME_DEFAULT,
						module ? module->threeColorFftGradient.load(std::memory_order_relaxed) : false);
					NVGcolor expectedWhite = palette.white;
					NVGcolor expectedCyan = palette.high;
					NVGcolor expectedPurple = palette.low;
					NVGcolor fill;
					if (displayOnlyMode) {
						fill = mixColor(expectedPurple, expectedCyan, displayOnlyColorTone(energy, displayOnlyShapeControl));
					}
					else {
						NVGcolor tint = expectedWhite; 
						if (posA > 0.f) tint = mixColor(tint, expectedCyan, levi_math::clamp01(posA * 1.40f)); 
						if (negA > 0.f) tint = mixColor(tint, expectedPurple, levi_math::clamp01(negA * 1.25f));
						fill = mixColor(expectedWhite, tint, 0.55f + 0.45f * energy);
					}
					
					float x0 = w * (float(i) / float(kCurvePointCount - 1));
					float x1 = w * (float(i + 1) / float(kCurvePointCount - 1));
					float y0 = spectrumYForDbfs(state.overlayOutputDbfs[i]);
					float y1 = spectrumYForDbfs(state.overlayOutputDbfs[i + 1]);
					const float topAlpha = clamp(0.78f + 0.18f * energy, 0.78f, 0.96f);

					fillVertices.push_back({x0, y0, fill.r, fill.g, fill.b, topAlpha});
					fillVertices.push_back({x1, y1, fill.r, fill.g, fill.b, topAlpha});
					fillVertices.push_back({x0, spectrumBottomY, fill.r, fill.g, fill.b, 1.0f});
					fillVertices.push_back({x1, y1, fill.r, fill.g, fill.b, topAlpha});
					fillVertices.push_back({x1, spectrumBottomY, fill.r, fill.g, fill.b, 1.0f});
					fillVertices.push_back({x0, spectrumBottomY, fill.r, fill.g, fill.b, 1.0f});

					// Feather the crest to mimic NanoVG's softer anti-aliased top blend.
					const float featherPxNear = 1.8f;
					const float capAlphaNear = clamp(0.08f + 0.18f * energy, 0.f, 0.26f);
					fillSoftCapVertices.push_back({x0, y0, fill.r, fill.g, fill.b, capAlphaNear});
					fillSoftCapVertices.push_back({x1, y1, fill.r, fill.g, fill.b, capAlphaNear});
					fillSoftCapVertices.push_back({x0, y0 - featherPxNear, fill.r, fill.g, fill.b, 0.0f});
					fillSoftCapVertices.push_back({x1, y1, fill.r, fill.g, fill.b, capAlphaNear});
					fillSoftCapVertices.push_back({x1, y1 - featherPxNear, fill.r, fill.g, fill.b, 0.0f});
					fillSoftCapVertices.push_back({x0, y0 - featherPxNear, fill.r, fill.g, fill.b, 0.0f});

					const float featherPxFar = 3.4f;
					const float capAlphaFar = clamp(0.03f + 0.10f * energy, 0.f, 0.13f);
					fillSoftCapVertices.push_back({x0, y0, fill.r, fill.g, fill.b, capAlphaFar});
					fillSoftCapVertices.push_back({x1, y1, fill.r, fill.g, fill.b, capAlphaFar});
					fillSoftCapVertices.push_back({x0, y0 - featherPxFar, fill.r, fill.g, fill.b, 0.0f});
					fillSoftCapVertices.push_back({x1, y1, fill.r, fill.g, fill.b, capAlphaFar});
					fillSoftCapVertices.push_back({x1, y1 - featherPxFar, fill.r, fill.g, fill.b, 0.0f});
					fillSoftCapVertices.push_back({x0, y0 - featherPxFar, fill.r, fill.g, fill.b, 0.0f});

					const float crestAlpha = clamp(0.16f + 0.18f * energy, 0.f, 0.34f);
					const float crestRadius = 1.05f + 0.45f * energy;
					const NVGcolor crest = mixColor(fill, nvgRGB(236, 244, 250), 0.18f);
					fillCrestLineVertices.push_back({x0, y0, crest.r, crest.g, crest.b, crestAlpha});
					fillCrestLineVertices.push_back({x1, y1, crest.r, crest.g, crest.b, crestAlpha});
					appendStrokeSegment(
						{x0, y0, crest.r, crest.g, crest.b, crestAlpha},
						{x1, y1, crest.r, crest.g, crest.b, crestAlpha},
						crestRadius,
						&fillCrestStrokeVertices
					);
				}
			}

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			if (!fillVertices.empty()) {
				glVertexPointer(2, GL_FLOAT, sizeof(GlVertex), &fillVertices[0].x);
				glColorPointer(4, GL_FLOAT, sizeof(GlVertex), &fillVertices[0].r);
				glDrawArrays(GL_TRIANGLES, 0, fillVertices.size());
			}
			if (!fillSoftCapVertices.empty()) {
				glVertexPointer(2, GL_FLOAT, sizeof(GlVertex), &fillSoftCapVertices[0].x);
				glColorPointer(4, GL_FLOAT, sizeof(GlVertex), &fillSoftCapVertices[0].r);
				glDrawArrays(GL_TRIANGLES, 0, fillSoftCapVertices.size());
			}
			if (!fillCrestLineVertices.empty()) {
				glLineWidth(1.25f);
				glVertexPointer(2, GL_FLOAT, sizeof(GlVertex), &fillCrestLineVertices[0].x);
				glColorPointer(4, GL_FLOAT, sizeof(GlVertex), &fillCrestLineVertices[0].r);
				glDrawArrays(GL_LINES, 0, fillCrestLineVertices.size());
				glLineWidth(1.f);
			}

			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		if (useShaderRenderer && !displayOnlyMode) {
			drawExpectedCurveShader(w, h);
			drawMarkerCirclesShader(w, h);
		}
		if (useShaderRenderer) {
			lastDrawVertexCount = (state.hasOverlay ? 4u : 0u) + uint64_t(expectedCurveStrokeVertices.size());
		}
		else {
			lastDrawVertexCount = uint64_t(fillVertices.size() + fillSoftCapVertices.size() + fillCrestLineVertices.size() + fillCrestStrokeVertices.size());
		}

		if (measurePerf) {
			lastDrawNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfDrawStart).count();
			const float drawMs = std::max(0.f, float(double(lastDrawNs) * 1e-6));
			lastDrawMsEma = (lastDrawMsEma > 0.f) ? (lastDrawMsEma + (drawMs - lastDrawMsEma) * 0.18f) : drawMs;
		}
	}

	void drawFramebuffer() override {
		renderGlContent(getFramebufferSize());
	}

	void renderFixedSurfaceIfNeeded() {
		if (!module || !module->fixedGlSurfaceEnabled.load(std::memory_order_relaxed)) return;
		NVGcontext* vg = (APP && APP->window) ? APP->window->vg : nullptr;
		if (!vg) return;
		if (rendererVg != vg) {
			// Module-owned GL names must not survive a missed context-destroy
			// notification. Surface handles are managed independently below.
			releaseRendererResources(false);
			rendererVg = vg;
			fixedSurface.markDirty();
		}
		float rackZoom = 1.f;
		if (APP && APP->scene && APP->scene->rackScroll) {
			rackZoom = std::max(APP->scene->rackScroll->getZoom(), 1e-4f);
		}
		const float pixelRatio = (APP && APP->window)
			? APP->window->pixelRatio : 1.f;
		visual_assets::AdaptiveGlSurfacePolicy policy;
		policy.maxDensity = 3.f;
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto renderStart = measurePerf
			? std::chrono::steady_clock::now()
			: std::chrono::steady_clock::time_point();
		const bool rendered = fixedSurface.renderIfNeeded(
			vg, box.size, rackZoom, pixelRatio, policy,
			isExtraGlValidationEnabled(),
			[](void* user, Vec activeSize, int viewportY) {
				static_cast<BifurxSpectrumGLWidget*>(user)->renderGlContent(activeSize, viewportY);
			},
			this);
		if (rendered && measurePerf) {
			lastSurfaceRenderUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				std::chrono::steady_clock::now() - renderStart).count()) * 1e-3f;
		}
	}

	void draw(const DrawArgs& args) override {
		if (module && module->fixedGlSurfaceEnabled.load(std::memory_order_relaxed)
			&& fixedSurface.draw(args, box.size)) return;
		widget::FramebufferWidget::draw(args);
	}

	void drawNanoVG(const DrawArgs& args) override {
		if (!module || module->renderMode != Bifurx::RENDER_OPENGL) return;
		if (!state.hasPreview) return;
		
		const float w = box.size.x, h = box.size.y;
		const bool displayOnlyMode = isBifurxDisplayOnlyMode(state.previewState.mode);
		BifurxMarkerLayout layout;
		if (!displayOnlyMode) {
			getCachedMarkerLayout(&layout, w, h);
		}

		const float padY = std::max(4.f, h * 0.035f);
		const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
		const float spectrumTopY = padY * 0.35f;
		const float spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
		auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
		if (!displayOnlyMode && !expectedCurveShaderActiveLastFrame) {
			calculateRefinedCurvePoints(&overlayCurvePoints, w, h);
		}
		else {
			overlayCurvePoints.clear();
		}

		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, std::max(1.f, w), std::max(1.f, spectrumBottomY + 1.f));

		if (!displayOnlyMode && state.hasOverlay && module->showModuleResponseOverlay.load(std::memory_order_relaxed)) {
			NVGcolor ml = mixColor(nvgRGB(206, 210, 216), nvgRGB(28, 204, 217), 0.35f);
			ml.a = 0.95f;
			nvgBeginPath(args.vg);
			for (int i = 0; i < kCurvePointCount; ++i) {
				const float x = w * (float(i) / float(kCurvePointCount - 1));
				const float y = responseYForDb(state.overlayModuleDb[i]);
				if (i == 0) nvgMoveTo(args.vg, x, y);
				else nvgLineTo(args.vg, x, y);
			}
			nvgLineJoin(args.vg, NVG_ROUND);
			nvgLineCap(args.vg, NVG_ROUND);
			nvgStrokeWidth(args.vg, 1.4f);
			nvgStrokeColor(args.vg, ml);
			nvgStroke(args.vg);
		}

		if (!displayOnlyMode) {
			if (!expectedCurveShaderActiveLastFrame) {
				for (int i = 0; i < 2; i++) {
					if (!layout.markers[i].visible) continue;
					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, layout.markers[i].x, spectrumBottomY);
					nvgLineTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker);
					nvgStrokeColor(args.vg, nvgRGBA(6, 8, 12, 210));
					nvgStrokeWidth(args.vg, 2.2f);
					nvgStroke(args.vg);
					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, layout.markers[i].x, spectrumBottomY);
					nvgLineTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker);
					nvgStrokeColor(args.vg, nvgRGBA(235, 204, 128, 244));
					nvgStrokeWidth(args.vg, 1.7f);
					nvgStroke(args.vg);
					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker + kPeakMarkerFillRadius + 0.45f);
					nvgLineTo(args.vg, layout.markers[i].x, layout.guideYBottom);
					nvgStrokeColor(args.vg, nvgRGBA(6, 8, 12, 210));
					nvgStrokeWidth(args.vg, 2.2f);
					nvgStroke(args.vg);
					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker + kPeakMarkerFillRadius + 0.45f);
					nvgLineTo(args.vg, layout.markers[i].x, layout.guideYBottom);
					nvgStrokeColor(args.vg, nvgRGBA(235, 204, 128, 244));
					nvgStrokeWidth(args.vg, 1.7f);
					nvgStroke(args.vg);
				}
			}

			if (!expectedCurveShaderActiveLastFrame) {
				nvgBeginPath(args.vg);
				for (size_t i = 0; i < overlayCurvePoints.size(); ++i) {
					const float x = w * overlayCurvePoints[i].x01;
					const float y = overlayCurvePoints[i].y;
					if (i == 0) nvgMoveTo(args.vg, x, y);
					else nvgLineTo(args.vg, x, y);
				}
				nvgLineJoin(args.vg, NVG_ROUND);
				nvgLineCap(args.vg, NVG_ROUND);
				nvgStrokeWidth(args.vg, 2.2f);
				nvgStrokeColor(args.vg, nvgRGBA(6, 8, 12, 210));
				nvgStroke(args.vg);
				nvgBeginPath(args.vg);
				for (size_t i = 0; i < overlayCurvePoints.size(); ++i) {
					const float x = w * overlayCurvePoints[i].x01;
					const float y = overlayCurvePoints[i].y;
					if (i == 0) nvgMoveTo(args.vg, x, y);
					else nvgLineTo(args.vg, x, y);
				}
				nvgLineJoin(args.vg, NVG_ROUND);
				nvgLineCap(args.vg, NVG_ROUND);
				nvgStrokeWidth(args.vg, 1.7f);
				nvgStrokeColor(args.vg, nvgRGBA(235, 204, 128, 244));
				nvgStroke(args.vg);
			}
		}
		nvgResetScissor(args.vg);
		nvgRestore(args.vg);

		// 1. Vertical Guide Lines
		if (!displayOnlyMode) {
			for (int i = 0; i < 2; i++) {
				if (!layout.markers[i].visible) continue;
				if (!markerShaderActiveLastFrame) {
					nvgBeginPath(args.vg);
					nvgCircle(args.vg, layout.markers[i].x, layout.markers[i].yMarker, kPeakMarkerFillRadius);
					nvgFillColor(args.vg, nvgRGBA(252, 255, 255, 244));
					nvgFill(args.vg);
					
					nvgBeginPath(args.vg);
					nvgCircle(args.vg, layout.markers[i].x, layout.markers[i].yMarker, kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius);
					nvgStrokeColor(args.vg, nvgRGBA(8, 10, 14, 220));
					nvgStrokeWidth(args.vg, kPeakMarkerOutlineStrokeWidth);
					nvgStroke(args.vg);
				}
			}

			nvgFontSize(args.vg, layout.labelFontSize);
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			for (int i = 0; i < 2; i++) {
				if (!layout.markers[i].visible) continue;
				nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240));
				nvgText(args.vg, layout.labelX[i], layout.labelY + 0.75f, layout.markers[i].label, nullptr);
				nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250));
				nvgText(args.vg, layout.labelX[i], layout.labelY, layout.markers[i].label, nullptr);
			}
		}

		// 4. dBFS Label
		nvgFontSize(args.vg, std::max(7.f, h * 0.05f));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		char topLabel[32];
		std::snprintf(topLabel, sizeof(topLabel), "%+5.1f dBFS", state.displayTopDbfs);
		const float topLabelReservedWidth = getTopLabelReservedWidth(args, std::max(7.f, h * 0.05f));
		nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
		nvgText(args.vg, 1.5f + topLabelReservedWidth, 1.f, topLabel, nullptr);

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

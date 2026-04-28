#include "Bifurx.hpp"
#include "DebugTerminalTransport.hpp"
#include <nanovg_gl.h>
#include <cstddef>
#include <unordered_map>

namespace bifurx {

static constexpr double kDebugTerminalSubmitIntervalSec = 1.0 / 8.0;
static constexpr double kActiveDrawWindowSec = 1.0;
static std::unordered_map<uint32_t, double> gDebugTerminalLastSubmitSec;

struct BifurxSpectrumGLWidget final : widget::OpenGlWidget, BifurxSpectrumBase {
	struct GlVertex {
		float x, y;
		float r, g, b, a;
	};

	// Persistent buffers to avoid per-frame allocations
	std::vector<GlVertex> fillVertices;
	std::vector<GlVertex> fillSoftCapVertices;
	std::vector<GlVertex> curveVertices;
	std::vector<GlVertex> cyanVertices;
	std::vector<BifurxCurvePoint> refinedPoints;

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
	int cachedTopLabelFontHandle = -1;
	float cachedTopLabelFontSize = NAN;
	float cachedTopLabelReservedWidth = 0.f;
	bool lastShowModuleResponseOverlay = false;
	bool lastUseGlShaderRenderer = false;
	bool shaderRendererActiveLastFrame = false;
	bool shaderRendererFallbackLastFrame = false;
	uint64_t lastDrawNs = 0;
	float lastDrawMsEma = 0.f;
	uint64_t lastDrawVertexCount = 0;
	float lastCurvePrepUs = 0.f;
	float lastOverlayPrepUs = 0.f;
	double lastDrawActivitySec = -1.0;

	BifurxSpectrumGLWidget() : BifurxSpectrumBase() {
		const size_t overlaySegmentCount = (kCurvePointCount > 0) ? size_t(kCurvePointCount - 1) : size_t(0);
		const size_t refinedPointReserve = size_t(kCurvePointCount) + 8;
		fillVertices.reserve(overlaySegmentCount * 6);
		fillSoftCapVertices.reserve(overlaySegmentCount * 12);
		curveVertices.reserve(refinedPointReserve);
		cyanVertices.reserve(size_t(kCurvePointCount));
		refinedPoints.reserve(refinedPointReserve);
	}

	void releaseShaderResources() {
		if (shaderVbo) {
			glDeleteBuffers(1, &shaderVbo);
			shaderVbo = 0;
		}
		if (shaderProgram) {
			glDeleteProgram(shaderProgram);
			shaderProgram = 0;
		}
		if (shaderVertex) {
			glDeleteShader(shaderVertex);
			shaderVertex = 0;
		}
		if (shaderFragment) {
			glDeleteShader(shaderFragment);
			shaderFragment = 0;
		}
		shaderUniformViewport = -1;
		shaderVboCapacityBytes = 0;
		shaderReady = false;
		shaderInitAttempted = false;
	}

	~BifurxSpectrumGLWidget() {
		if (vbo) glDeleteBuffers(1, &vbo);
		releaseShaderResources();
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
		if (module->renderMode != Bifurx::RENDER_OPENGL) return;

		float uiFrameSec = 1.f / 60.f;
		if (APP && APP->window) {
			const float frameSec = float(APP->window->getLastFrameDuration());
			if (std::isfinite(frameSec) && frameSec > 0.f) {
				uiFrameSec = clamp(frameSec, 1.f / 240.f, 1.f / 20.f);
			}
		}
		const double nowSec = system::getTime();
		const bool activelyDrawnRecently = (lastDrawActivitySec > 0.0) && ((nowSec - lastDrawActivitySec) <= kActiveDrawWindowSec);
		BifurxRenderTickResult tick;
		if (activelyDrawnRecently) {
			tick = runRenderTick(uiFrameSec);
			if (tick.curvePrepUs > 0.f) {
				lastCurvePrepUs = tick.curvePrepUs;
			}
			if (tick.overlayPrepUs > 0.f) {
				lastOverlayPrepUs = tick.overlayPrepUs;
			}
		}
		const bool showModuleResponseOverlayNow = module->showModuleResponseOverlay;
		const bool useGlShaderRendererNow = module->useGlShaderRenderer;

		// Shared dirty policy with NanoVG path: redraw on new data or active animation.
		if (tick.previewUpdated || tick.analysisUpdated || tick.animationActive) {
			setDirty();
		}
		if (showModuleResponseOverlayNow != lastShowModuleResponseOverlay) {
			lastShowModuleResponseOverlay = showModuleResponseOverlayNow;
			setDirty();
		}
		if (useGlShaderRendererNow != lastUseGlShaderRenderer) {
			lastUseGlShaderRenderer = useGlShaderRendererNow;
			setDirty();
		}

		if (isDragonKingDebugEnabled() && module && module->renderMode == Bifurx::RENDER_OPENGL) {
			double nowSec = system::getTime();
			uint32_t debugId = module->debugInstanceId;
			double& lastSubmitSec = gDebugTerminalLastSubmitSec[debugId];
			if (lastSubmitSec <= 0.0 || (nowSec - lastSubmitSec) >= kDebugTerminalSubmitIntervalSec) {
				const int filterMode = clamp(int(state.previewState.mode), 0, kBifurxModeCount - 1);
				lastSubmitSec = nowSec;
				debug_terminal::submitBifurxUiMetrics(
					debugId,
					lastDrawMsEma,
					filterMode,
					true, // opengl
					state.lastPreviewSeq,
					state.lastAnalysisSeq,
					lastDrawVertexCount,
					lastCurvePrepUs,
					lastOverlayPrepUs
				);
			}
		}
	}

	void drawFramebuffer() override {
		lastDrawActivitySec = system::getTime();
		using PerfClock = std::chrono::steady_clock;
		const PerfClock::time_point perfDrawStart = PerfClock::now();

		if (!module || module->renderMode != Bifurx::RENDER_OPENGL) return;
		if (!vbo) glGenBuffers(1, &vbo);

		Vec fbSize = getFramebufferSize();
		glViewport(0, 0, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		const float w = box.size.x, h = box.size.y;
		if (!(w > 0.f && h > 0.f)) return;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, w, h, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_SCISSOR_TEST);

		const float padY = std::max(4.f, h * 0.035f);
		const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
		const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
		
		const float displayMaxDbfs = state.displayTopDbfs;
		const float displayMinDbfs = displayMaxDbfs - kDisplayDbfsSpan;
		auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
		auto spectrumYForDbfs = [&](float dbfs) { return rescale(clamp(dbfs, displayMinDbfs, displayMaxDbfs), displayMinDbfs, displayMaxDbfs, spectrumBottomY, spectrumTopY); };

		fillVertices.clear();
		fillSoftCapVertices.clear();
		curveVertices.clear();
		cyanVertices.clear();
		const bool showModuleResponse = module && module->showModuleResponseOverlay;

		// 1. FFT Fill Overlay
		if (state.hasOverlay) {
			for (int i = 0; i < kCurvePointCount - 1; i++) {
				const float avgD = 0.5f * (state.overlayModuleDb[i] + state.overlayModuleDb[i + 1]);
				const float avgO = 0.5f * (state.overlayOutputDbfs[i] + state.overlayOutputDbfs[i + 1]);
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
			}
		}

		// 2. Cyan Module Response
		if (state.hasOverlay && showModuleResponse) {
			NVGcolor expectedWhite = nvgRGB(206, 210, 216);
			NVGcolor expectedCyan = nvgRGB(28, 204, 217);
			NVGcolor cyanColor = mixColor(expectedWhite, expectedCyan, 0.35f);
			cyanColor.a = 0.95f;
			for (int i = 0; i < kCurvePointCount; i++) {
				float x = w * (float(i) / float(kCurvePointCount - 1));
				float y = responseYForDb(state.overlayModuleDb[i]);
				cyanVertices.push_back({x, y, cyanColor.r, cyanColor.g, cyanColor.b, cyanColor.a});
			}
		}

		// 3. Main Yellow Filter Curve (with shared refinements)
		calculateRefinedCurvePoints(&refinedPoints, w, h);
		NVGcolor curveColor = nvgRGBA(255, 248, 208, 244);
		for (const auto& p : refinedPoints) {
			curveVertices.push_back({w * p.x01, p.y, curveColor.r, curveColor.g, curveColor.b, curveColor.a});
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		const bool useShaderRenderer = module->useGlShaderRenderer && ensureShaderReady();
		shaderRendererActiveLastFrame = useShaderRenderer;
		shaderRendererFallbackLastFrame = module->useGlShaderRenderer && !useShaderRenderer;
		if (useShaderRenderer) {
			drawVertsShader(fillVertices, GL_TRIANGLES, 1.f, w, h);
			drawVertsShader(fillSoftCapVertices, GL_TRIANGLES, 1.f, w, h);
			drawVertsShader(cyanVertices, GL_LINE_STRIP, 2.2f, w, h);
			drawVertsShader(curveVertices, GL_LINE_STRIP, 3.0f, w, h);
		}
		else {
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

			if (!cyanVertices.empty()) {
				glLineWidth(2.2f);
				glVertexPointer(2, GL_FLOAT, sizeof(GlVertex), &cyanVertices[0].x);
				glColorPointer(4, GL_FLOAT, sizeof(GlVertex), &cyanVertices[0].r);
				glDrawArrays(GL_LINE_STRIP, 0, cyanVertices.size());
			}

			if (!curveVertices.empty()) {
				glLineWidth(3.0f);
				glVertexPointer(2, GL_FLOAT, sizeof(GlVertex), &curveVertices[0].x);
				glColorPointer(4, GL_FLOAT, sizeof(GlVertex), &curveVertices[0].r);
				glDrawArrays(GL_LINE_STRIP, 0, curveVertices.size());
			}

			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
		glDisable(GL_LINE_SMOOTH);
		lastDrawVertexCount = uint64_t(fillVertices.size() + fillSoftCapVertices.size() + curveVertices.size() + cyanVertices.size());

		lastDrawNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfDrawStart).count();
		{
			const float drawMs = std::max(0.f, float(double(lastDrawNs) * 1e-6));
			lastDrawMsEma = (lastDrawMsEma > 0.f) ? (lastDrawMsEma + (drawMs - lastDrawMsEma) * 0.18f) : drawMs;
		}
	}

	void draw(const DrawArgs& args) override {
		widget::OpenGlWidget::draw(args);
	}

	void drawNanoVG(const DrawArgs& args) override {
		if (!module || module->renderMode != Bifurx::RENDER_OPENGL) return;
		if (!state.hasPreview) return;
		
		const float w = box.size.x, h = box.size.y;
		BifurxMarkerLayout layout;
		calculateMarkerLayout(&layout, w, h);

		const float padY = std::max(4.f, h * 0.035f);
		const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
		const float spectrumBottomY = std::max(padY * 0.35f + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));

		// 1. Vertical Guide Lines
		for (int i = 0; i < 2; i++) {
			if (!layout.markers[i].visible) continue;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, layout.markers[i].x, spectrumBottomY);
			nvgLineTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker);
			nvgStrokeColor(args.vg, nvgRGBA(252, 236, 176, 150));
			nvgStrokeWidth(args.vg, 1.05f * 1.45f);
			nvgStroke(args.vg);
		}

		// 2. Peak Markers
		for (int i = 0; i < 2; i++) {
			if (!layout.markers[i].visible) continue;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker + kPeakMarkerFillRadius + 0.45f);
			nvgLineTo(args.vg, layout.markers[i].x, layout.guideYBottom);
			nvgStrokeColor(args.vg, nvgRGBA(252, 236, 176, 170));
			nvgStrokeWidth(args.vg, 1.1f);
			nvgStroke(args.vg);
			
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

		// 3. Labels
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

		// 4. dBFS Label
		nvgFontSize(args.vg, std::max(7.f, h * 0.05f));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		char topLabel[32];
		std::snprintf(topLabel, sizeof(topLabel), "%+5.1f dBFS", state.displayTopDbfs);
		const float topLabelReservedWidth = getTopLabelReservedWidth(args, std::max(7.f, h * 0.05f));
		nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
		nvgText(args.vg, 1.5f + topLabelReservedWidth, 1.f, topLabel, nullptr);

		const char* renderBadge = "GL FIXED";
		if (shaderRendererActiveLastFrame) {
			renderBadge = "GL SHDR";
		}
		else if (shaderRendererFallbackLastFrame) {
			renderBadge = "GL FALLBACK";
		}
		const float badgeFontSize = std::max(6.6f, h * 0.045f);
		nvgFontSize(args.vg, badgeFontSize);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
		nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 220));
		nvgText(args.vg, w - 2.2f + 0.5f, 1.6f + 0.5f, renderBadge, nullptr);
		nvgFillColor(args.vg, nvgRGBA(225, 232, 240, 230));
		nvgText(args.vg, w - 2.2f, 1.6f, renderBadge, nullptr);
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

#pragma once

// Some toolchains don't expose math constants like M_PI by default.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440
#endif

#include <rack.hpp>
#include <chrono>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
// extern Model* modelMyModule;
extern Model* modelIntegralFlux;
extern Model* modelProc;
extern Model* modelTemporalDeck;
extern Model* modelTDScope;
extern Model* modelCrownstep;
extern Model* modelBifurx;
extern Model* modelWyrm;
extern Model* modelSil;
extern Model* modelChronomaw;
extern Model* modelBulkhead;
extern Model* modelUndertow;
extern Model* modelIris;
extern Model* modelNautiloid;
extern Model* modelDeepcache;
extern Model* modelUmi;
extern Model* modelDoorstop;
extern Model* modelChromatide;
extern Model* modelPuffy;
extern Model* modelMandelwake;
extern Model* modelLongplayer;

// Local semantic alias so module code can request a white tiny Befaco knob
// without depending on another plugin's custom class declarations.
struct BefacoTinyKnobWhite : BefacoTinyKnob {};

// Runtime feature flags loaded from `res/dragonking.txt`.
bool isDragonKingDebugEnabled();
bool isDragonKingPreviewWidgetOptionsEnabled();
bool isCrownstepAddMoveEnabled();
bool isClockworkDragDebugLoggingEnabled();
bool isTemporalDeckLifetimeLoggingEnabled();
bool isModuleTeardownLoggingEnabled();
bool isScopeDrawLoggingEnabled();
bool isIntegralFluxDrawLoggingEnabled();
bool isPuffyDrawLoggingEnabled();
bool isExtraGlValidationEnabled();
bool isDragonKingUserFractalParamsEnabled();
void refreshDragonKingDebugEnabled();

struct ModuleTeardownTimer {
	const char* moduleName = nullptr;
	int moduleId = -1;
	bool active = false;
	std::chrono::steady_clock::time_point startedAt;

	explicit ModuleTeardownTimer(const char* moduleName);
	~ModuleTeardownTimer();
	void begin(int moduleId);
};

struct PreviewBuildLogTimer {
	const char* moduleName = nullptr;
	const rack::Module* module = nullptr;
	std::chrono::steady_clock::time_point startedAt;
	double panelDoneMs = -1.0;
	double anchorsDoneMs = -1.0;
	const char* atlasStatus = "n/a";

	PreviewBuildLogTimer(const char* moduleName, const rack::Module* module)
		: moduleName(moduleName)
		, module(module)
		, startedAt(std::chrono::steady_clock::now()) {
	}

	~PreviewBuildLogTimer() {
		if (module != nullptr || !isDragonKingDebugEnabled()) {
			return;
		}
		const auto endedAt = std::chrono::steady_clock::now();
		const double elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(endedAt - startedAt).count() * 1e-3;
		const char* name = moduleName ? moduleName : "unknown";
		if (panelDoneMs >= 0.0 && anchorsDoneMs >= panelDoneMs) {
			const double panelMs = panelDoneMs;
			const double anchorsMs = anchorsDoneMs - panelDoneMs;
			const double restMs = elapsedMs - anchorsDoneMs;
			INFO("Preview build [%s]: total=%.3f ms panel=%.3f ms anchors=%.3f ms rest=%.3f ms atlas=%s",
				name, elapsedMs, panelMs, anchorsMs, restMs, atlasStatus);
			return;
		}
		if (panelDoneMs >= 0.0) {
			const double panelMs = panelDoneMs;
			const double restMs = elapsedMs - panelDoneMs;
			INFO("Preview build [%s]: total=%.3f ms panel=%.3f ms rest=%.3f ms atlas=%s",
				name, elapsedMs, panelMs, restMs, atlasStatus);
			return;
		}
		INFO("Preview build [%s]: total=%.3f ms atlas=%s", name, elapsedMs, atlasStatus);
	}

	void markPanelDone() {
		if (module != nullptr || !isDragonKingDebugEnabled()) {
			return;
		}
		const auto now = std::chrono::steady_clock::now();
		panelDoneMs = std::chrono::duration_cast<std::chrono::microseconds>(now - startedAt).count() * 1e-3;
	}

	void markAnchorsDone() {
		if (module != nullptr || !isDragonKingDebugEnabled()) {
			return;
		}
		const auto now = std::chrono::steady_clock::now();
		anchorsDoneMs = std::chrono::duration_cast<std::chrono::microseconds>(now - startedAt).count() * 1e-3;
	}

	void setAtlasStatus(const char* status) {
		if (status && status[0] != '\0') {
			atlasStatus = status;
		}
	}
};

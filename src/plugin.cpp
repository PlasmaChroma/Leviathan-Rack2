#include "plugin.hpp"
#include "BifurxWorker.hpp"
#include "theme/ThemePersistence.hpp"
#include "visual/VisualAssets.hpp"

#include <atomic>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <mutex>

Plugin* pluginInstance;
static std::atomic<bool> gDragonKingDebugEnabled{false};
static std::atomic<bool> gDragonKingPreviewWidgetOptionsEnabled{false};
static std::atomic<bool> gCrownstepAddMoveEnabled{false};
static std::atomic<bool> gClockworkDragDebugLoggingEnabled{false};
static std::atomic<bool> gTemporalDeckLifetimeLoggingEnabled{false};
static std::atomic<bool> gModuleTeardownLoggingEnabled{false};
static std::atomic<bool> gScopeDrawLoggingEnabled{false};
static std::atomic<bool> gIntegralFluxDrawLoggingEnabled{false};
static std::atomic<bool> gPuffyDrawLoggingEnabled{false};
static std::atomic<bool> gWyrmDrawLoggingEnabled{false};
static std::atomic<bool> gExtraGlValidationEnabled{false};
static std::atomic<bool> gUserFractalParamsEnabled{false};
static std::mutex gModuleTeardownLogMutex;

std::string leviathanPluginUserRootPath() {
	const std::string slug = (pluginInstance && !pluginInstance->slug.empty())
		? pluginInstance->slug
		: "Leviathan";
	return system::join(asset::user(), slug);
}

void refreshDragonKingDebugEnabled() {
	gDragonKingDebugEnabled.store(false, std::memory_order_relaxed);
	gDragonKingPreviewWidgetOptionsEnabled.store(false, std::memory_order_relaxed);
	gCrownstepAddMoveEnabled.store(false, std::memory_order_relaxed);
	gClockworkDragDebugLoggingEnabled.store(false, std::memory_order_relaxed);
	gTemporalDeckLifetimeLoggingEnabled.store(false, std::memory_order_relaxed);
	gModuleTeardownLoggingEnabled.store(false, std::memory_order_relaxed);
	gScopeDrawLoggingEnabled.store(false, std::memory_order_relaxed);
	gIntegralFluxDrawLoggingEnabled.store(false, std::memory_order_relaxed);
	gPuffyDrawLoggingEnabled.store(false, std::memory_order_relaxed);
	gWyrmDrawLoggingEnabled.store(false, std::memory_order_relaxed);
	gExtraGlValidationEnabled.store(false, std::memory_order_relaxed);
	gUserFractalParamsEnabled.store(false, std::memory_order_relaxed);
	if (!pluginInstance) {
		return;
	}
	// This is optional per-user developer configuration, not a distributable
	// plugin asset. Keeping it under Rack's user directory also prevents a
	// locally enabled debug file from being included in a release package.
	const std::string flagPath = system::join(leviathanPluginUserRootPath(), "dragonking.txt");
	std::ifstream flagFile(flagPath);
	if (!flagFile.good()) {
		return;
	}
	json_error_t error;
	json_t* root = json_load_file(flagPath.c_str(), 0, &error);
	if (!root) {
		gDragonKingDebugEnabled.store(true, std::memory_order_relaxed);
		return;
	}
	if (json_is_object(root)) {
		json_t* debugJ = json_object_get(root, "debug");
		json_t* previewWidgetOptionsJ = json_object_get(root, "PreviewWidgetOptions");
		json_t* crownstepAddMoveJ = json_object_get(root, "CrownstepAddMove");
		json_t* clockworkDragLoggingJ = json_object_get(root, "clockworkDragLogging");
		json_t* temporalDeckLifetimeLoggingJ = json_object_get(root, "temporalDeckLifetimeLogging");
		json_t* moduleTeardownLoggingJ = json_object_get(root, "moduleTeardownLogging");
		json_t* scopeDrawLoggingJ = json_object_get(root, "ScopeDrawLogging");
		json_t* integralFluxDrawLoggingJ = json_object_get(root, "IntegralFluxDrawLogging");
		json_t* puffyDrawLoggingJ = json_object_get(root, "PuffyDrawLogging");
		json_t* wyrmDrawLoggingJ = json_object_get(root, "WyrmDrawLogging");
		json_t* extraGlValidationJ = json_object_get(root, "extraGlValidation");
		json_t* userFractalParamsJ = json_object_get(root, "UserFractalParams");
		if (!extraGlValidationJ) {
			extraGlValidationJ = json_object_get(root, "ExtraGlValidation");
		}
		gDragonKingDebugEnabled.store(debugJ == nullptr || json_is_true(debugJ), std::memory_order_relaxed);
		gDragonKingPreviewWidgetOptionsEnabled.store(json_boolean_value(previewWidgetOptionsJ), std::memory_order_relaxed);
		gCrownstepAddMoveEnabled.store(json_boolean_value(crownstepAddMoveJ), std::memory_order_relaxed);
		gClockworkDragDebugLoggingEnabled.store(json_boolean_value(clockworkDragLoggingJ), std::memory_order_relaxed);
		gTemporalDeckLifetimeLoggingEnabled.store(json_boolean_value(temporalDeckLifetimeLoggingJ), std::memory_order_relaxed);
		gModuleTeardownLoggingEnabled.store(json_boolean_value(moduleTeardownLoggingJ), std::memory_order_relaxed);
		gScopeDrawLoggingEnabled.store(json_boolean_value(scopeDrawLoggingJ), std::memory_order_relaxed);
		gIntegralFluxDrawLoggingEnabled.store(json_boolean_value(integralFluxDrawLoggingJ), std::memory_order_relaxed);
		gPuffyDrawLoggingEnabled.store(json_boolean_value(puffyDrawLoggingJ), std::memory_order_relaxed);
		gWyrmDrawLoggingEnabled.store(json_boolean_value(wyrmDrawLoggingJ), std::memory_order_relaxed);
		gExtraGlValidationEnabled.store(json_boolean_value(extraGlValidationJ), std::memory_order_relaxed);
		gUserFractalParamsEnabled.store(json_boolean_value(userFractalParamsJ), std::memory_order_relaxed);
	}
	json_decref(root);
}

bool isDragonKingDebugEnabled() {
	return gDragonKingDebugEnabled.load(std::memory_order_relaxed);
}

bool isDragonKingPreviewWidgetOptionsEnabled() {
	return gDragonKingPreviewWidgetOptionsEnabled.load(std::memory_order_relaxed);
}

bool isCrownstepAddMoveEnabled() {
	return gCrownstepAddMoveEnabled.load(std::memory_order_relaxed);
}

bool isClockworkDragDebugLoggingEnabled() {
	return gClockworkDragDebugLoggingEnabled.load(std::memory_order_relaxed);
}

bool isTemporalDeckLifetimeLoggingEnabled() {
	return gTemporalDeckLifetimeLoggingEnabled.load(std::memory_order_relaxed);
}

bool isModuleTeardownLoggingEnabled() {
	return gModuleTeardownLoggingEnabled.load(std::memory_order_relaxed);
}

bool isScopeDrawLoggingEnabled() {
	return gScopeDrawLoggingEnabled.load(std::memory_order_relaxed);
}

bool isIntegralFluxDrawLoggingEnabled() {
	return gIntegralFluxDrawLoggingEnabled.load(std::memory_order_relaxed);
}

bool isPuffyDrawLoggingEnabled() {
	return gPuffyDrawLoggingEnabled.load(std::memory_order_relaxed);
}

bool isWyrmDrawLoggingEnabled() {
	return gWyrmDrawLoggingEnabled.load(std::memory_order_relaxed);
}

bool isExtraGlValidationEnabled() {
	return gExtraGlValidationEnabled.load(std::memory_order_relaxed);
}

bool isDragonKingUserFractalParamsEnabled() {
	return gUserFractalParamsEnabled.load(std::memory_order_relaxed);
}

ModuleTeardownTimer::ModuleTeardownTimer(const char* moduleName)
	: moduleName(moduleName) {
}

void ModuleTeardownTimer::begin(int id) {
	if (!isModuleTeardownLoggingEnabled()) {
		return;
	}
	moduleId = id;
	active = true;
	startedAt = std::chrono::steady_clock::now();
	INFO("Leviathan: module teardown begin: %s id=%d", moduleName ? moduleName : "unknown", moduleId);
}

ModuleTeardownTimer::~ModuleTeardownTimer() {
	if (!active || !isModuleTeardownLoggingEnabled()) {
		return;
	}
	const auto endedAt = std::chrono::steady_clock::now();
	const double elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(endedAt - startedAt).count() * 1e-3;
	const std::string dir = leviathanPluginUserRootPath();
	system::createDirectories(dir);
	const std::string path = system::join(dir, "module_teardown.csv");

	std::lock_guard<std::mutex> lock(gModuleTeardownLogMutex);
	std::ifstream existing(path);
	const bool writeHeader = !existing.good();
	existing.close();

	std::ofstream out(path, std::ios::app);
	if (!out.is_open()) {
		WARN("Leviathan: failed to open module teardown log: %s", path.c_str());
		return;
	}
	if (writeHeader) {
		out << "unix_time_sec,module_name,module_id,total_ms\n";
	}
	out << std::time(nullptr) << ','
	    << (moduleName ? moduleName : "unknown") << ','
	    << moduleId << ','
	    << std::fixed << std::setprecision(3)
	    << elapsedMs << '\n';
}


void init(Plugin* p) {
	pluginInstance = p;
	refreshDragonKingDebugEnabled();
	visual_assets::loadSettings();
	leviathan::theme::persistence::initializeFromUserStorage();

	// Add modules here
	// p->addModel(modelMyModule);
	p->addModel(modelIntegralFlux);
	p->addModel(modelProc);
	p->addModel(modelTemporalDeck);
	p->addModel(modelTDScope);
    p->addModel(modelUndertow);
    p->addModel(modelDeepcache);
	p->addModel(modelIris);
	p->addModel(modelNautiloid);
    p->addModel(modelPuffy);
	p->addModel(modelCrownstep);
	p->addModel(modelBifurx);
	p->addModel(modelWyrm);
	p->addModel(modelSil);
	p->addModel(modelChronomaw);
	p->addModel(modelBulkhead);
	p->addModel(modelUmi);
	p->addModel(modelDoorstop);
	p->addModel(modelChromatide);
	p->addModel(modelMandelwake);
	p->addModel(modelCantor);
	p->addModel(modelTheme);
	p->addModel(modelOctavia);
	p->addModel(modelOctaviaConsole);
	p->addModel(modelSibyl);
	p->addModel(modelMoirai);
	p->addModel(modelPhonex);
	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}

void destroy() {
	// Explicit plugin-lifecycle shutdown avoids static-destruction order hazards across TUs.
	leviathan::theme::persistence::saveToUserStorage();
	visual_assets::saveSettings();
	bifurx::shutdownBifurxRenderService();
}

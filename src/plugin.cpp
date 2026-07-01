#include "plugin.hpp"
#include "BifurxWorker.hpp"
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
static std::atomic<bool> gClockworkDragDebugLoggingEnabled{false};
static std::atomic<bool> gTemporalDeckLifetimeLoggingEnabled{false};
static std::atomic<bool> gModuleTeardownLoggingEnabled{false};
static std::mutex gModuleTeardownLogMutex;

void refreshDragonKingDebugEnabled() {
	gDragonKingDebugEnabled.store(false, std::memory_order_relaxed);
	gDragonKingPreviewWidgetOptionsEnabled.store(false, std::memory_order_relaxed);
	gClockworkDragDebugLoggingEnabled.store(false, std::memory_order_relaxed);
	gTemporalDeckLifetimeLoggingEnabled.store(false, std::memory_order_relaxed);
	gModuleTeardownLoggingEnabled.store(false, std::memory_order_relaxed);
	if (!pluginInstance) {
		return;
	}
	const std::string flagPath = asset::plugin(pluginInstance, "res/dragonking.txt");
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
		json_t* clockworkDragLoggingJ = json_object_get(root, "clockworkDragLogging");
		json_t* temporalDeckLifetimeLoggingJ = json_object_get(root, "temporalDeckLifetimeLogging");
		json_t* moduleTeardownLoggingJ = json_object_get(root, "moduleTeardownLogging");
		gDragonKingDebugEnabled.store(!debugJ || json_boolean_value(debugJ), std::memory_order_relaxed);
		gDragonKingPreviewWidgetOptionsEnabled.store(json_boolean_value(previewWidgetOptionsJ), std::memory_order_relaxed);
		gClockworkDragDebugLoggingEnabled.store(json_boolean_value(clockworkDragLoggingJ), std::memory_order_relaxed);
		gTemporalDeckLifetimeLoggingEnabled.store(json_boolean_value(temporalDeckLifetimeLoggingJ), std::memory_order_relaxed);
		gModuleTeardownLoggingEnabled.store(json_boolean_value(moduleTeardownLoggingJ), std::memory_order_relaxed);
	}
	json_decref(root);
}

bool isDragonKingDebugEnabled() {
	return gDragonKingDebugEnabled.load(std::memory_order_relaxed);
}

bool isDragonKingPreviewWidgetOptionsEnabled() {
	return gDragonKingPreviewWidgetOptionsEnabled.load(std::memory_order_relaxed);
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
	const std::string dir = system::join(asset::user(), "Leviathan");
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

	// Add modules here
	// p->addModel(modelMyModule);
	p->addModel(modelIntegralFlux);
	p->addModel(modelProc);
	p->addModel(modelTemporalDeck);
	p->addModel(modelTDScope);
	//p->addModel(modelCrownstep);
	//p->addModel(modelBifurx);
	//p->addModel(modelWyrm);
	//p->addModel(modelSil);
	//p->addModel(modelChronomaw);
	//p->addModel(modelBulkhead);
	p->addModel(modelUndertow);
	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}

void destroy() {
	// Explicit plugin-lifecycle shutdown avoids static-destruction order hazards across TUs.
	visual_assets::saveSettings();
	bifurx::shutdownBifurxRenderService();
}
